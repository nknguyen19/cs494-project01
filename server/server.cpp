#include "server.h"
#include "utils.h"
#include <string.h> //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h> //close
#include <arpa/inet.h> //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
#include <fcntl.h>
#include <iostream>
#include <algorithm>

using namespace std;

#define DEFAULT_MAX_CLIENTS 30

Server::Server() : max_clients(DEFAULT_MAX_CLIENTS) {
	// initialise all client_socket[] to 0 so not checked
	client_socket = vector<int>(max_clients, 0);

	//create a master socket
	if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	//set master socket to allow multiple connections
	int opt = 1;
	if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	// non-blocking socket for accept call 
	if (fcntl(master_socket, F_SETFL, O_NONBLOCK) < 0)
	{
		perror("fcntl");
		exit(EXIT_FAILURE);
	}

	// type of socket created
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	//bind the socket to localhost port
	if (bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	cout << "Listener on port " << PORT << endl;

	//try to specify maximum of 3 pending connections for the master socket
	if (listen(master_socket, 3) < 0)
	{
		// perror("listen");
		exit(EXIT_FAILURE);
	}
}

Server::~Server() {
	for (Game* g: games) delete g;
}

void Server::run() {
	// accept the incoming connection
	int addrlen = sizeof(address);
	cout << "Waiting for connections..." << endl;

	while (TRUE) {
		// clear the socket set
		FD_ZERO(&readfds);

		// add master socket to set
		FD_SET(master_socket, &readfds);
		int max_sd = master_socket;
		int sd;

		// add child sockets to set
		for (int i = 0; i < max_clients; i++)
		{
			// socket descriptor
			sd = client_socket[i];

			// if valid socket descriptor then add to read list
			if (sd > 0)
				FD_SET(sd, &readfds);

			// highest file descriptor number, need it for the select function
			if (sd > max_sd)
				max_sd = sd;
		}

		// wait for an activity on one of the sockets, timeout is NULL, so wait indefinitely
		int activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
		if ((activity < 0) && (errno != EINTR))
		{
			cout << "select error" << endl;
		}

		// If something happened on the master socket, then its an incoming connection
		if (FD_ISSET(master_socket, &readfds))
		{	
			int new_socket;
			if ((new_socket = accept(master_socket, 
				(struct sockaddr *)&address, 
				(socklen_t*)&addrlen)) < 0)
			{
				perror("accept");
				exit(EXIT_FAILURE);
			}

			// inform user of socket number - used in send and receive commands
			cout << "New connection, socket fd is " 
				<< new_socket << ", ip is : " 
				<< inet_ntoa(address.sin_addr) 
				<< ", port : " 
				<< ntohs(address.sin_port) << endl;

			// // send new connection greeting message
			// string message = "Welcome to the server. Type your name and press enter to continue.";
			// if (send(new_socket, message.c_str(), message.length(), 0) != message.length())
			// {
			// 	perror("send");
			// }

			// add new socket to array of sockets
			for (int i = 0; i < max_clients; i++)
			{
				// if position is empty
				if (client_socket[i] == 0)
				{
					client_socket[i] = new_socket;
					cout << "Adding to list of sockets as " << i << endl;
					break;
				}
			}

			// send new connection greeting message
			send(new_socket, "OK", 2, 0);
		}

		// else its some IO operation on some other socket
		for (int i = 0; i < max_clients; i++)
		{
			sd = client_socket[i];

			if (FD_ISSET(sd, &readfds))
			{
				// Check if it was for closing, and also read the incoming message
				int valread = read(sd, buffer, 1024);
				if (valread == 0)
				{
					// Somebody disconnected, get his details and print
					getpeername(sd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
					cout << "Host disconnected, ip " 
						<< inet_ntoa(address.sin_addr) 
						<< ", port " 
						<< ntohs(address.sin_port) << endl;

					// If he's in some game, remove him
					if (descriptor.count(sd)) remove(sd);

					// Close the socket and mark as 0 in list for reuse
					close(sd);
					client_socket[i] = 0;
				}

				// Echo back the message that came in
				else
				{
					// set the string terminating NULL byte on the end of the data read
					buffer[valread] = '\0';
					cout << "Message received: " << buffer << endl;
					executeCommand(string(buffer), sd);
				}
			}
		}
	}
}

void Server::executeCommand(string message, int socket_id) {
	string command = parseMessageCommand(message);
	string content = parseMessageContent(message);

	if (command == "REGISTER")
		handleRegisterCommand(socket_id, content);
	else if (command == "ANSWER")
		handleAnswerCommand(socket_id, content);
	else if (command == "MOVE")
		handleMoveCommand(socket_id);
	else if (command == "LOGOUT")
		handleLogoutCommand(socket_id);
	else if (command == "INFO")
		handleInfoCommand(socket_id);
	else {
		cout << "Invalid command" << endl;
		string message = "400 Invalid command.\n";
		send(socket_id, message.c_str(), message.length(), 0);
	}
}

void Server::handleRegisterCommand(int socket_id, string nickname) {
	try {
		// already registered
		if (descriptor.count(socket_id))
			throw("400 You've already registered in a game.\n");

		// validate nickname
		if (!validateNickname(nickname))
			throw("400 Nickname is invalid. Please try again.\n");

		// check if nickname is already taken
		for (auto p: descriptor)
			if (p.second.second->getNickname() == nickname)
				throw("400 Nickname is already taken. Please try again.\n");

		// register nickname
		Player* player = new Player(socket_id, nickname);
		Game* game = nullptr;
		for (int id = 0; id < games.size(); ++id)
			if (games[id]->addPlayer(player)) {
				descriptor[socket_id] = { game = games[id], player };
				break;
			}

		// if no game is available, create a new game
		if (game == nullptr) {
			games.push_back(new Game(player));
			descriptor[socket_id] = { game = games[games.size() - 1], player };
		}

		// send success message
		string message{"200 Nickname registered successfully.\n"};
		send(socket_id, message.c_str(), message.length(), 0);

		game->notifyAllPlayers();
	} catch (const char* message) {
		send(socket_id, message, strlen(message), 0);
		return;
	}
}

void Server::handleInGameCommand(int socket_id) {
	if (!descriptor.count(socket_id))
		throw("400 You must register first.\n");
	auto d = descriptor[socket_id];
	if (!d.first->isPlaying())
		throw("400 The game has not started.\n");
	if (!d.second->isInTurn())
		throw("400 Not your turn.\n");
}

void Server::handleAnswerCommand(int socket_id, string answer) {
	try {
		handleInGameCommand(socket_id);
		if (answer.substr(0, 7) == "TIMEOUT") {
			auto d = descriptor[socket_id];
			d.first->submitAnswer(' ');
			d.first->notifyAllPlayers();
			if (d.first->isFinished())
				remove(d.first);
			return;
		}
		if (answer.length() != 1 || 
			toupper(answer[0]) < 'A' || 'D' < toupper(answer[0])
		)
			throw("400 Invalid answer. Answer must be A, B, C or D (case insensitive).\n");
		
		auto d = descriptor[socket_id];
		string message;
		if (d.first->submitAnswer(answer[0]))
			message = "200 CORRECT\n";
		else
			message = "200 INCORRECT\n";
		send(socket_id, message.c_str(), message.length(), 0);
		d.first->notifyAllPlayers();
		if (d.first->isFinished())
			remove(d.first);
	} catch (const char* message) {
		send(socket_id, message, strlen(message), 0);
		return;
	}
}

void Server::handleMoveCommand(int socket_id) {
	try {
		handleInGameCommand(socket_id);
		auto d = descriptor[socket_id];
		d.first->moveTurn();
		string message = "200 OK\n";
		send(socket_id, message.c_str(), message.length(), 0);
		d.first->notifyAllPlayers();
	} catch (const char* message) {
		send(socket_id, message, strlen(message), 0);
		return;
	}
}

void Server::handleLogoutCommand(int socket_id) {
	try {
		if (!descriptor.count(socket_id))
			throw("400 You haven't registered to any game.\n");
		remove(socket_id);
		string message = "200 Logout successfully.\n";
		send(socket_id, message.c_str(), message.length(), 0);
	} catch (const char* message) {
		send(socket_id, message, strlen(message), 0);
		return;
	}
}

void Server::handleInfoCommand(int socket_id) {
	string message;
	if (descriptor.count(socket_id))
		message = "200\n" + descriptor[socket_id].second->getInfoMessage();
	else
		message = "200 You haven't registered to any game.\n";
	send(socket_id, message.c_str(), message.length(), 0);
}

void Server::remove(Game* game) {
	auto ptr = find(games.begin(), games.end(), game);
	if (ptr == games.end())
		throw("404 Somehow this game does not exist.\n");

	vector<Player*> in_game = game->getInGamePlayers();
	for (Player* player: in_game)
		descriptor.erase(player->getSocketId());

	delete game;
	games.erase(ptr);
}

void Server::remove(int socket_id) {
	auto d = descriptor[socket_id];
	descriptor.erase(socket_id);
	d.first->remove(d.second);
	if (d.first->isFinished())
		remove(d.first);
}