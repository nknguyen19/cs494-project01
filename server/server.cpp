#include "server.h"
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
#include "utils.h"

using namespace std;

Server::Server(): max_clients(30), client_socket(vector<int>(30))
{
	//initialise all client_socket[] to 0 so not checked
	for (int i = 0; i < max_clients; i++)
	{
		client_socket[i] = 0;
	}

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

Server::~Server()
{

}

void Server::run()
{
	// accept the incoming connection
	int addrlen = sizeof(address);
	cout << "Waiting for connections ..." << endl;

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

			// send new connection greeting message
			string message = "Welcome to the server. Type your name and press enter to continue.\n";
			if (send(new_socket, message.c_str(), message.length(), 0) != message.length())
			{
				perror("send");
			}

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

					// Close the socket and mark as 0 in list for reuse
					close(sd);
					client_socket[i] = 0;
				}

				// Echo back the message that came in
				else
				{
					// set the string terminating NULL byte on the end of the data read
					// buffer[valread] = '\0';
					executeCommand(string(buffer), sd);
				}
			}
		}
	}
}

void Server::registerPlayer(int client_socket, string content)
{
		// validate nickname
		if (!validateNickname(content))
		{
			string message = "400 Nickname is invalid. Please try again.\n";
			send(client_socket, message.c_str(), message.length(), 0);
			return;
		}

		// check if nickname is already taken
		bool isNicknameExist = false;
		for (auto game: games)
		{	
			if (game->isNicknameExist(content))
			{
				isNicknameExist = true;
				break;
			}
		}
		if (isNicknameExist)
		{
			string message = "400 Nickname is already taken. Please try again.\n";
			send(client_socket, message.c_str(), message.length(), 0);
			return;
		}

		// register nickname
		int game_id = -1;
		bool hasJoinedGame = false;
		Player* player = new Player(client_socket, content);

		for (int id = 0; id < games.size(); ++id)
		{
			if (games[id]->addPlayer(player))
			{
				hasJoinedGame = true;
				game_id = id;
				break;
			}
		}
		// if no game is available, create a new game
		if (!hasJoinedGame)
		{
			Game* game = new Game();
			games.push_back(game);
			game->addPlayer(player);
			game_id = games.size() - 1;
		}

		player_id[client_socket] = { game_id, games[game_id]->getCurrentNumberOfPlayers() - 1 };

		// send success message
		string message = "200 Nickname registered successfully.\n";
		send(client_socket, message.c_str(), message.length(), 0);
		if (games[game_id]->isPlaying()) games[game_id]->notifyAllPlayers();
}

void Server::executeCommand(string message, int client_socket)
{
	string command = parseMessageCommand(message);
	string content = parseMessageContent(message);

	if (command == "REGISTER")
	{
		registerPlayer(client_socket, content);
	}
	else if (command == "ANSWER")
		handleAnswerRequest(client_socket, content);
	else if (command == "MOVE")
		handleMoveRequest(client_socket);
	else
	{
		cout << "Invalid command" << endl;
		send(client_socket, "404 Invalid command\n", 20, 0);
	}
}

bool Server::handleInGameRequest(int client_socket) {
	if (!player_id.count(client_socket)) {
		send(
			client_socket,
			"400 You must register first.\n",
			29, 0
		);
		return false;
	}
	else if (!games[player_id[client_socket].first]->isPlaying()) {
		send(
			client_socket,
			"400 The game has not started.\n",
			30, 0
		);
		return false;
	}
	else if (games[player_id[client_socket].first]->getPlayerStatus(player_id[client_socket].second) != Player::INTURN) {
		send(
			client_socket,
			"400 Not your turn.\n",
			19, 0
		);
		return false;
	}

	return true;
}

void Server::handleAnswerRequest(int client_socket, string answer) {
	if (!handleInGameRequest(client_socket)) return;
	int game_id = player_id[client_socket].first;

	if (answer.length() != 1 || (
		(answer[0] < 'A' || 'D' < answer[0]) && 
		(answer[0] < 'a' || 'd' < answer[0])
	))
		send(
			client_socket,
			"400 Invalid answer. Answer must be A, B, C or D (case insensitive).\n",
			68, 0
		);
	else { // 200 OK circumstances
		if (games[game_id]->submitAnswer(answer[0]))
			send(
				client_socket,
				"200 Correct answer. Continue answering.\n",
				40, 0
			);
		else
			send(
				client_socket,
				"200 Incorrect answer. You've been disqualified.\n",
				48, 0
			);
		
		games[game_id]->notifyAllPlayers();
	}
}

void Server::handleMoveRequest(int client_socket) {
	if (!handleInGameRequest(client_socket)) return;
	int game_id = player_id[client_socket].first;
	
	if (games[game_id]->currentPlayerMoveTurn()) {
		send(
			client_socket,
			"200 Move turn sucessfully.\n",
			27, 0
		);

		games[game_id]->notifyAllPlayers();
	}
	else
		send(
			client_socket,
			"400 You've already move your turn once.\n",
			40, 0
		);
}