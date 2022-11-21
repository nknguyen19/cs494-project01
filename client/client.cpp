#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>   
#include <arpa/inet.h>    
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <iostream>
#include <netdb.h>

Client::Client()
{
    // Create a socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        cerr << "Can't create a socket! Quitting" << endl;
        return;
    }

    // Create a hint structure for the server we're connecting with
    hint.sin_family = AF_INET;
    hint.sin_port = htons(PORT);
    inet_pton(AF_INET, IP, &hint.sin_addr);

    // Connect to the server on the socket
    int connectRes = connect(sock, (sockaddr*)&hint, sizeof(hint));
    if (connectRes == -1)
    {
        cerr << "Can't connect to server, Err #" << errno << endl;
        return;
    }
}

void Client::run(){
    // Do-while loop to send and receive data
    do {
        // Prompt the user for some text
        cout << "> ";
        getline(cin, userInput);

        if (userInput.size() > 0) // Make sure the user has typed in something
        {
            // Send the text
            int sendRes = send(sock, userInput.c_str(), userInput.size() + 1, 0);
            if (sendRes == -1)
            {
                cerr << "Could not send to server! Err #" << errno << endl;
            }
        }

        // Wait for response
        memset(buffer, 0, 4096);
        int bytesReceived = recv(sock, buffer, 4096, 0);
        if (bytesReceived > 0)
        {
            // Echo response to console
            cout << "SERVER> " << string(buffer, 0, bytesReceived) << endl;
        }
        else 
        {
            cerr << "There was a connection issue" << endl;
        }
    } while (true);

    // Close the socket
    close(sock);
}
