#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <cmath>
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

    // returns number of bytes received (<= 0 means disconnected / error)
    int receiveMessage(int sockfd, char *message)
    {
        int n = recv(sockfd, message, BUF_SIZE - 1, 0);
        if (n > 0)
            message[n] = '\0';
        return n;
    }
};

class Calculator
{
public:
    // Expects input like: "12 + 5", "7.5 * 2", "10 / 4", "10 % 3", "2 ^ 8"
    static string evaluate(const string &expr)
    {
        istringstream iss(expr);
        double a, b;
        char op;
        string extra;

        if (!(iss >> a >> op >> b) || (iss >> extra))
            return "Error: use format <number> <operator> <number>  (operators: + - * / % ^)";

        double result;
        switch (op)
        {
        case '+': result = a + b; break;
        case '-': result = a - b; break;
        case '*': result = a * b; break;
        case '/':
            if (b == 0) return "Error: division by zero";
            result = a / b;
            break;
        case '%':
            if (b == 0) return "Error: modulo by zero";
            result = fmod(a, b);
            break;
        case '^': result = pow(a, b); break;
        default:
            return string("Error: unknown operator '") + op + "'";
        }

        ostringstream out;
        out.precision(10);
        out << result;
        return out.str();
    }
};

int main()
{
    // Don't let a write to a closed socket kill the program (macOS/Linux raise SIGPIPE)
    signal(SIGPIPE, SIG_IGN);

    // Step 1: Create socket (no WSAStartup needed on macOS/Linux)
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        cout << "Socket creation failed\n";
        return 1;
    }

    // Lets you restart the server immediately without "Bind failed" (address already in use)
    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Step 2: Configure server address
    sockaddr_in serverAddr = {}, clientAddr = {};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    // Step 3: Bind
    if (::bind(sockfd, (sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        cout << "Bind failed\n";
        close(sockfd);
        return 1;
    }

    // Step 4: Listen
    if (listen(sockfd, 5) < 0)
    {
        cout << "Listen failed\n";
        close(sockfd);
        return 1;
    }
    cout << "Calculator server started on port " << PORT << "...\n";
    cout << "Waiting for the client...\n";

    // Step 5: Accept (socklen_t instead of int)
    socklen_t len = sizeof(clientAddr);
    int connfd = accept(sockfd, (sockaddr *)&clientAddr, &len);
    if (connfd < 0)
    {
        cout << "Accept failed\n";
        close(sockfd);
        return 1;
    }
    cout << "Client connected!\n";

    // Step 6: Receive expression -> compute -> send result
    char buffer[BUF_SIZE];
    Chat c1;
    while (true)
    {
        int n = c1.receiveMessage(connfd, buffer);
        if (n <= 0)
        {
            cout << "Client disconnected.\n";
            break;
        }

        cout << "Received: " << buffer << "\n";

        if (strcmp(buffer, "bye") == 0)
        {
            cout << "Client ended communication!\n";
            break;
        }

        string answer = Calculator::evaluate(buffer);
        cout << "Sending : " << answer << "\n";
        c1.sendMessage(connfd, answer.c_str());
    }

    close(connfd);
    close(sockfd);
    return 0;
}
