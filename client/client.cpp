#include <stdio.h>
#include <string.h>  
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>   
#include <arpa/inet.h>    
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <iostream>
#include <netdb.h>

using namespace std;

int main()
{
    // Create socket
    int sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        cerr << "Can't create a socket! Quitting" << endl;
        return -1;
    }

    // Create a hint structure for the server we're connecting with
    int port = 54000;
    string ipAddress = "127.0.0.1"; // localhost

    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

    // Connect to server
    int connectRes = connect(sock, (sockaddr*)&hint, sizeof(hint));
    if (connectRes == -1)
    {
        return 1;
    }

    // Do-while loop to send and receive data
    char buf[4096];
    string userInput;

    do {
        // Prompt the user for some text
        cout << "> ";
        getline(cin, userInput);

        // Send the text
        int sendRes = send(sock, userInput.c_str(), userInput.size() + 1, 0);
        if (sendRes == -1)
        {
            cerr << "Could not send to server! Whoops!\r" << endl;
        }

        // Wait for response
        memset(buf, 0, 4096);
        int bytesReceived = recv(sock, buf, 4096, 0);
        if (bytesReceived == -1)
        {
            cerr << "There was an error getting response from server\r" << endl;
        }
        else
        {
            // Display response
            cout << "SERVER> " << string(buf, 0, bytesReceived) << "\r" << endl;
        }
    } while (true);

    // Close the socket
    close(sock);
    
    return 0;
}