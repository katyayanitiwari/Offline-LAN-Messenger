#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <cstring>
#include <fstream>
#include <vector>
#include <sstream>

#pragma comment(lib, "ws2_32.lib")
using namespace std;
string encryptDecrypt(string text)
{
    char key = 'K';

    for(int i = 0; i < text.length(); i++)
    {
        text[i] ^= key;
    }

    return text;
}
DWORD WINAPI receiveMessages(LPVOID param)
{
    SOCKET clientSocket = *(SOCKET*)param;

    char buffer[1024];

    while (true)
    {
        memset(buffer, 0, sizeof(buffer));

        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

        if (bytesReceived <= 0)
        {
            std::cout << "\n=====================================\n";
            std::cout << "Disconnected from server.\n";
            std::cout << "=====================================\n";
            break;
        }

        buffer[bytesReceived] = '\0';
        string message = buffer;
        message = encryptDecrypt(message);

//==============================
// File Header Detection
//==============================

if(message.rfind("FILE|",0)==0)
{
    stringstream ss(message);

    string temp;
    string sender;
    string filename;
    string sizeStr;

    getline(ss,temp,'|');
    getline(ss,sender,'|');
    getline(ss,filename,'|');
    getline(ss,sizeStr,'|');

    int filesize = stoi(sizeStr);

    cout << "\nReceiving file from "
         << sender
         << "...\n";

    // ACK server ko bhejo
    send(clientSocket,"OK",2,0);

    vector<char> fileData(filesize);

    int received = 0;

    while(received < filesize)
    {
        int bytes = recv(
            clientSocket,
            &fileData[received],
            filesize - received,
            0);

        if(bytes <= 0)
            break;

        received += bytes;
    }

    ofstream file(filename, ios::binary);

    if(file.is_open())
    {
        file.write(fileData.data(), received);
        file.close();

        cout << "File received successfully!\n";
        cout << "Saved as : "
             << filename
             << endl;
    }
    else
    {
        cout << "Unable to save file.\n";
    }

    continue;
}

        std::cout << "\n" << message << endl;
        std::cout << "You : ";
        std::cout.flush();
    }

    return 0;
}
//==============================
// Send File
//==============================

void sendFile(SOCKET clientSocket,
              string receiver,
              string filename)
{
    ifstream file(filename, ios::binary);

    if(!file.is_open())
    {
        cout << "File not found.\n";
        return;
    }

    file.seekg(0, ios::end);

    int fileSize = file.tellg();

    file.seekg(0, ios::beg);

    vector<char> buffer(fileSize);

    file.read(buffer.data(), fileSize);

    file.close();

    string header =
    "/sendfile " +
    receiver + " " +
    filename + " " +
    to_string(fileSize);

    send(clientSocket,
         header.c_str(),
         (int)header.length(),
         0);

    Sleep(100);

    send(clientSocket,
         buffer.data(),
         fileSize,
         0);

    cout << "File sent successfully.\n";
}

int main()
{
    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cout << "WSAStartup Failed\n";
        return 1;
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (clientSocket == INVALID_SOCKET)
    {
        std::cout << "Socket Creation Failed\n";
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr{};

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(5555);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(clientSocket,
                (sockaddr*)&serverAddr,
                sizeof(serverAddr)) == SOCKET_ERROR)
    {
        std::cout << "Connection Failed : "
                  << WSAGetLastError()
                  << std::endl;

        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "=====================================\n";
    std::cout << " Connected to LAN Messenger Server\n";
    std::cout << "=====================================\n";

    char username[50];

    std::cout << "Enter Username : ";
    std::cin.getline(username, sizeof(username));

    send(clientSocket,
         username,
         strlen(username),
         0);

    HANDLE hThread = CreateThread(
        NULL,
        0,
        receiveMessages,
        &clientSocket,
        0,
        NULL);

    char message[1024];

    while (true)
    {
        std::cout << "You : ";

        std::cin.getline(message, sizeof(message));

        if (strlen(message) == 0)
            continue;

        if (strcmp(message, "exit") == 0)
            break;
            if(strncmp(message, "/sendfile ", 10) == 0)
{
    stringstream ss(message);

    string command;
    string receiver;
    string filename;

    ss >> command;
    ss >> receiver;
    ss >> filename;

    sendFile(clientSocket,
             receiver,
             filename);

    continue;
}

        if (send(clientSocket,
                 message,
                 strlen(message),
                 0) == SOCKET_ERROR)
        {
            std::cout << "Send Failed\n";
            break;
        }
    }

    closesocket(clientSocket);

    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);

    WSACleanup();

    return 0;
}