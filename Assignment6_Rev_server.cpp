#include <iostream>
#include <string>
#include <cstring>
#include <algorithm>
#include <sys/socket.h>
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
                     const sockaddr_in &clientAddr, socklen_t len)
    {
        sendto(sockfd, message, strlen(message), 0,
               (const sockaddr *)&clientAddr, len);
    }

    // returns bytes received, or -1 on error
    int receiveMessage(int sockfd, char *message,
                       sockaddr_in &clientAddr, socklen_t &len)
    {
        len = sizeof(clientAddr); // must be reset before every recvfrom
        int n = recvfrom(sockfd, message, BUF_SIZE - 1, 0,
                         (sockaddr *)&clientAddr, &len);

        if (n > 0)
            message[n] = '\0';
        else
            message[0] = '\0';
        return n;
    }
};

// Reverses a C-string in place: "hello" -> "olleh"
void reverseString(char *s)
{
    reverse(s, s + strlen(s));
}

int main()
{
    // Step 1: Create a UDP socket (SOCK_DGRAM = datagram socket)
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        cout << "\nSocket creation failed";
        return 1;
    }

    // Lets you restart the server right away without "Bind failed"
    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Step 2: Configure server address
    sockaddr_in serverAddr = {}, clientAddr = {};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    // Step 3: Bind (::bind avoids a clash with std::bind from "using namespace std")
    if (::bind(sockfd, (sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        cout << "\nBind failed";
        close(sockfd);
        return 1;
    }

    cout << "\nServer started...";
    cout << "\nWaiting for the client..." << flush;

    // Step 4: Receive -> reverse -> send back
    char buffer[BUF_SIZE];
    socklen_t len = sizeof(clientAddr);
    Chat c1;

    while (true)
    {
        int n = c1.receiveMessage(sockfd, buffer, clientAddr, len);
        if (n < 0)
        {
            cout << "\nReceive failed";
            break;
        }

        cout << "\nClient: " << buffer;

        if (strcmp(buffer, "bye") == 0)
        {
            cout << "\nClient ended communication!";
            break;
        }

        reverseString(buffer);

        cout << "\nReversed message: " << buffer;

        c1.sendMessage(sockfd, buffer, clientAddr, len);
    }

    cout << endl;
    close(sockfd);
    return 0;
}
