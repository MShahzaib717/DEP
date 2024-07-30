#include <iostream>
#include <thread>
#include <string>
#include <fstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <dirent.h>
#include <sstream>

using namespace std;

class TcpServer {
public:
    TcpServer(int port) {
        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket < 0) {
            cerr << "Error creating socket" << endl;
            exit(1);
        }

        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(port);
        serverAddress.sin_addr.s_addr = INADDR_ANY;

        if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
            cerr << "Error binding socket" << endl;
            exit(1);
        }
    }

    void startListen() {
        if (listen(serverSocket, 3) < 0) {
            cerr << "Error listening for connections" << endl;
            exit(1);
        }

        cout << "Server listening on port 8080..." << endl;

        while (true) {
            struct sockaddr_in clientAddress;
            socklen_t clientAddressLength = sizeof(clientAddress);
            int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);

            if (clientSocket < 0) {
                cerr << "Error accepting connection" << endl;
                continue;
            }
            thread t(&TcpServer::handleRequestThread, this, clientSocket);
            t.detach();
        }
    }

private:
    void handleRequestThread(int clientSocket) {

        string request = readRequest(clientSocket);

        string file = parseRequest(request);

        sendResponse(clientSocket, file);

        close(clientSocket);
    }

    string readRequest(int clientSocket) {
        char buffer[1024];
        string request;

        while (true) {
            int bytesRead = read(clientSocket, buffer, 1024);
            if (bytesRead < 0) {
                cerr << "Error reading from socket" << endl;
                return "";
            }

            request += string(buffer, bytesRead);
            if (request.find("\r\n\r\n") != string::npos) {
                break;
            }
        }

        return request;
    }

    string parseRequest(string request) {
        size_t pos = request.find("GET ");
        if (pos == string::npos) {
            return "404.html";
        }

        pos += 4;
        size_t endPos = request.find(" ", pos);
        if (endPos == string::npos) {
            return "404.html";
        }

        string file = request.substr(pos, endPos - pos);
        if (file == "/") {
            file = "/index.html";
        }

        return file;
    }

    string getContentType(const string& file) {
        size_t dotPos = file.find_last_of(".");
        if (dotPos == string::npos) {
            return "text/plain";
        }

        string extension = file.substr(dotPos + 1);
        if (extension == "html") return "text/html";
        if (extension == "css") return "text/css";
        if (extension == "js") return "application/javascript";
        if (extension == "png") return "image/png";
        if (extension == "jpg" || extension == "jpeg") return "image/jpeg";
        if (extension == "gif") return "image/gif";

        return "text/plain";
    }

    void sendResponse(int clientSocket, string file) {
        ifstream fileStream("." + file);
        if (!fileStream.is_open()) {
            fileStream.open("404.html");
            if (!fileStream.is_open()) {
                cerr << "Error opening file" << endl;
                return;
            }
        }

        string fileContent((istreambuf_iterator<char>(fileStream)), istreambuf_iterator<char>());

        string contentType = getContentType(file);

        stringstream response;
        response << "HTTP/1.1 200 OK\r\n";
        response << "Content-Type: " << contentType << "\r\n";
        response << "Content-Length: " << fileContent.length() << "\r\n";
        response << "\r\n";
        response << fileContent;

        write(clientSocket, response.str().c_str(), response.str().length());
    }

    int serverSocket;
    struct sockaddr_in serverAddress;
};

int main() {
    TcpServer server(8080);
    server.startListen();
    return 0;
}
