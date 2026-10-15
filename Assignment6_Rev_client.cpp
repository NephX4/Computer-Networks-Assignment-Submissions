#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080
#define BUF_SIZE 100

using namespace std;

class Chat
{
public:
    void sendMessage(int sockfd, const char *message,
                     const sockaddr_in &serverAddr)
    {
        sendto(sockfd, message, strlen(message), 0,
               (const sockaddr *)&serverAddr, sizeof(serverAddr));
    }

    // returns bytes received, or -1 on error / timeout
    int receiveMessage(int sockfd, char *message,
                       sockaddr_in &serverAddr, socklen_t &len)
    {
        len = sizeof(serverAddr);
        int n = recvfrom(sockfd, message, BUF_SIZE - 1, 0,
                         (sockaddr *)&serverAddr, &len);

        if (n > 0)
            message[n] = '\0';
        else
            message[0] = '\0';
        return n;
    }
};

int main()
{
    // Step 1: Create a UDP socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        cout << "\nSocket creation failed";
        return 1;
    }

    // UDP has no connection, so if the server isn't running recvfrom would wait forever.
    // A 5-second receive timeout avoids that.
    timeval tv = {};
    tv.tv_sec = 5;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    // Step 2: Server address (change 127.0.0.1 to the server machine's IP if needed)
    sockaddr_in serverAddr = {};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

    // Step 3: Send message, receive reversed reply
    char buffer[BUF_SIZE];
    socklen_t len = sizeof(serverAddr);
    Chat c1;

    cout << "Type text to reverse. Type 'bye' to quit.";

    while (true)
    {
        cout << "\nClient: " << flush;
        cin.getline(buffer, BUF_SIZE);
        if (cin.eof())
            strcpy(buffer, "bye"); // Ctrl+D: quit cleanly
        else if (strlen(buffer) == 0)
            continue; // don't send empty messages

        c1.sendMessage(sockfd, buffer, serverAddr);

        if (strcmp(buffer, "bye") == 0)
        {
            cout << "\nClient ended communication!";
            break;
        }

        int n = c1.receiveMessage(sockfd, buffer, serverAddr, len);
        if (n < 0)
        {
            cout << "\nNo response from server (is it running?)";
            continue;
        }

        cout << "\nServer (reversed): " << buffer;
    }

    cout << endl;
    close(sockfd);
    return 0;
}
