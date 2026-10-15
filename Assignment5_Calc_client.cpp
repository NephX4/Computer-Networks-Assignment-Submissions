#include <iostream>
#include <string>
#include <cstring>
#include <csignal>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
 
#define PORT 8080
#define BUF_SIZE 256
 
using namespace std;
 
class Chat
{
public:
    void sendMessage(int sockfd, const char *message)
    {
        send(sockfd, message, strlen(message), 0);
    }
 
    int receiveMessage(int sockfd, char *message)
    {
        int n = recv(sockfd, message, BUF_SIZE - 1, 0);
        if (n > 0)
            message[n] = '\0';
        return n;
    }
};
 
int main()
{
    // Don't let a write to a closed socket kill the program
    signal(SIGPIPE, SIG_IGN);
 
    // Step 1: Create socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        cout << "Socket creation failed\n";
        return 1;
    }
 
    // Step 2: Server address (change 127.0.0.1 to the server machine's IP if needed)
    sockaddr_in serverAddr = {};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
 
    // Step 3: Connect
    if (connect(sockfd, (sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        cout << "Connection failed\n";
        close(sockfd);
        return 1;
    }
 
    cout << "Connected to calculator server.\n";
    cout << "Enter expressions like: 12 + 5, 7.5 * 2, 10 / 4, 10 % 3, 2 ^ 8\n";
    cout << "Type 'bye' to quit.\n";
 
    // Step 4: Send expression, receive result
    char buffer[BUF_SIZE];
    Chat c1;
    while (true)
    {
        cout << "\nExpression: ";
        cin.getline(buffer, BUF_SIZE);
 
        if (cin.eof())
            strcpy(buffer, "bye"); // Ctrl+D: quit cleanly
        else if (strlen(buffer) == 0)
            continue; // don't send empty input
 
        c1.sendMessage(sockfd, buffer);
 
        if (strcmp(buffer, "bye") == 0)
        {
            cout << "Client ended the communication!\n";
            break;
        }
 
        int n = c1.receiveMessage(sockfd, buffer);
        if (n <= 0)
        {
            cout << "Server disconnected.\n";
            break;
        }
        cout << "Result    : " << buffer << "\n";
    }
 
    close(sockfd);
    return 0;
}
