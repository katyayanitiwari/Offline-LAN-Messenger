#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#include <vector>
#include <string>
#include <map>
#include <sstream>
#include <algorithm>
#include <fstream>
#include <cstring>

#pragma comment(lib, "ws2_32.lib")



using namespace std;
string encryptDecrypt(string text);
void logMessage(string message);

CRITICAL_SECTION clientsLock;

vector<SOCKET> clients;
map<string, SOCKET> userSockets;
map<string, vector<string>> groups;
int totalMessages = 0;
int totalFiles = 0;

//======================
// Broadcast Function
//======================

void broadcastMessage(const string &message, SOCKET sender)
{totalMessages++;
    EnterCriticalSection(&clientsLock);

    for(SOCKET s : clients)
    {
        if(s != sender)
        {
            send(s,
                 message.c_str(),
                 (int)message.length(),
                 0);
        }
    }

    LeaveCriticalSection(&clientsLock);
}

//======================
// Private Message
//======================

void sendPrivateMessage(string sender,
                        string receiver,
                        string message)
{
    EnterCriticalSection(&clientsLock);

    if(userSockets.find(receiver)!=userSockets.end())
    {
        SOCKET target=userSockets[receiver];

        string msg="[Private] "+sender+" : "+message;
        msg = encryptDecrypt(msg);
totalMessages++;
        send(target,
             msg.c_str(),
             (int)msg.length(),
             0);
             logMessage(msg);
    }
    else
    {
        SOCKET senderSocket=userSockets[sender];

        string error="User '"+receiver+"' is not online.";

        send(senderSocket,
             error.c_str(),
             (int)error.length(),
             0);
    }

    LeaveCriticalSection(&clientsLock);
}

//======================
// Online Users List
//======================

void sendUserList(SOCKET clientSocket)
{
    EnterCriticalSection(&clientsLock);

    string list;

    list += "\n";
    list += "========== ONLINE USERS ==========\n";

    int i = 1;

    for(auto user : userSockets)
    {
        list += to_string(i);
        list += ". ";
        list += user.first;
        list += "\n";

        i++;
    }

    list += "==================================\n";

    send(clientSocket,
         list.c_str(),
         (int)list.length(),
         0);

    LeaveCriticalSection(&clientsLock);
}
//==============================
// Create Group
//==============================

void createGroup(string groupName,
                 string username)
{
    EnterCriticalSection(&clientsLock);

    if(groups.find(groupName)==groups.end())
    {
        groups[groupName].push_back(username);

        cout << username
             << " created group "
             << groupName
             << endl;
    }

    LeaveCriticalSection(&clientsLock);
}



//==============================
// Join Group
//==============================

void joinGroup(string groupName,
               string username)
{
    EnterCriticalSection(&clientsLock);

    if(groups.find(groupName)!=groups.end())
    {
        if(find(groups[groupName].begin(),
                groups[groupName].end(),
                username)==groups[groupName].end())
        {
            groups[groupName].push_back(username);
        }
    }

    LeaveCriticalSection(&clientsLock);
}



//==============================
// Group Message
//==============================

void sendGroupMessage(string sender,
                      string groupName,
                      string message)
{
    EnterCriticalSection(&clientsLock);

    if(groups.find(groupName)!=groups.end())
    {
        string finalMessage =
        "[Group:" + groupName + "] "
        + sender + " : " + message;
        finalMessage = encryptDecrypt(finalMessage);
        totalMessages++;

        for(string member : groups[groupName])
        {
            if(userSockets.find(member)!=userSockets.end())
            {
                SOCKET target = userSockets[member];

                send(target,
                     finalMessage.c_str(),
                     (int)finalMessage.length(),
                     0);
                     logMessage(finalMessage);
            }
        }
    }

    LeaveCriticalSection(&clientsLock);
}
//======================
// Activity Logger
//======================

void logMessage(string message)
{
    ofstream file("chat_log.txt", ios::app);

    if(file.is_open())
    {
        file << message << endl;
        file.close();
    }
}
//==============================
// XOR Encryption
//==============================

string encryptDecrypt(string text)
{
    char key = 'K';

    for(int i = 0; i < text.length(); i++)
    {
        text[i] ^= key;
    }

    return text;
}
//==============================
// Send File
//==============================

void sendFile(string sender,
              string receiver,
              string filename,
              vector<char> fileData)
{
    EnterCriticalSection(&clientsLock);

    if(userSockets.find(receiver)!=userSockets.end())
    {
        SOCKET target=userSockets[receiver];

        string header=
        "FILE|"+sender+"|"+filename+"|"+
        to_string(fileData.size());

        send(target,
             header.c_str(),
             (int)header.length(),
             0);

        recv(target,new char[10],10,0);

        send(target,
             fileData.data(),
             (int)fileData.size(),
             0);
 totalFiles++;
        cout<<sender
            <<" sent "
            <<filename
            <<" to "
            <<receiver
            <<endl;
    }

    LeaveCriticalSection(&clientsLock);
}
void sendStats(SOCKET clientSocket)
{
    string stats;

    stats += "\n========== SERVER STATS ==========\n";

    stats += "Online Users : ";
    stats += to_string(userSockets.size());
    stats += "\n";

    stats += "Messages : ";
    stats += to_string(totalMessages);
    stats += "\n";

    stats += "Files Sent : ";
    stats += to_string(totalFiles);
    stats += "\n";

    stats += "==================================\n";

    send(clientSocket,
         stats.c_str(),
         (int)stats.length(),
         0);
}
DWORD WINAPI handleClient(LPVOID param)
{
    SOCKET clientSocket = *(SOCKET*)param;

    delete (SOCKET*)param;
    //========================
    // Receive Username
    //========================

    char username[50];

    int nameBytes = recv(clientSocket,
                         username,
                         sizeof(username)-1,
                         0);
       cout << "recv() returned: " << nameBytes << endl;
       if(nameBytes>0){
          username[nameBytes] = '\0';
         cout << "Username received: " << username << endl;
       }

    if(nameBytes <= 0)
    {
        closesocket(clientSocket);
        return 0;
    }
    username[nameBytes]='\0';


    string user = username;


    EnterCriticalSection(&clientsLock);

    userSockets[user] = clientSocket;

    LeaveCriticalSection(&clientsLock);



    cout<<"\n=====================================\n";
    cout<<user<<" joined the server.\n";
    logMessage(user + " joined the server.");
    cout<<"=====================================\n";


    string joinMessage = ">>> " + user + " joined the chat.";

    broadcastMessage(joinMessage,
                     clientSocket);



    char buffer[1024];


    //========================
    // Message Loop
    //========================

    while(true)
    {

        int bytesReceived = recv(clientSocket,
                                 buffer,
                                 sizeof(buffer)-1,
                                 0);



        if(bytesReceived <= 0)
        {

            cout<<user<<" disconnected.\n";
logMessage(user + " disconnected.");

            string leaveMessage =
            ">>> " + user + " left the chat.";


            broadcastMessage(leaveMessage,
                             clientSocket);


            break;
        }



        buffer[bytesReceived]='\0';


        string text = buffer;
        //==============================
// File Transfer Command
//==============================

if(text.rfind("/sendfile ",0)==0)
{
    stringstream ss(text);

    string command;
    string receiver;
    string filename;
    int filesize;

    ss>>command;
    ss>>receiver;
    ss>>filename;
    ss>>filesize;

    vector<char> fileData(filesize);

    int received=0;

    while(received<filesize)
    {
        int bytes=
        recv(clientSocket,
             &fileData[received],
             filesize-received,
             0);

        if(bytes<=0)
            break;

        received+=bytes;
    }

    sendFile(user,
             receiver,
             filename,
             fileData);

    continue;
}



        //========================
        // Private Chat Command
        // /msg username message
        //========================

        if(text.rfind("/msg ",0)==0)
        {

            stringstream ss(text);


            string command;
            string receiver;
            string msg;


            ss >> command;
            ss >> receiver;


            getline(ss,msg);



            if(msg.length()>0 && msg[0]==' ')
            {
                msg.erase(0,1);
            }



            sendPrivateMessage(user,
                               receiver,
                               msg);


            continue;
        }



        //========================
        // Online Users Command
        // /list
        //========================

        if(text == "/list")
        {

            sendUserList(clientSocket);

            continue;
        }
if(text=="/stats")
{
    sendStats(clientSocket);
    continue;
}
//==============================
// Create Group
// /create groupname
//==============================

if(text.rfind("/create ",0)==0)
{
    stringstream ss(text);

    string command;
    string groupName;

    ss >> command;
    ss >> groupName;

    createGroup(groupName, user);

    string msg = "Group created: " + groupName;

    send(clientSocket,
         msg.c_str(),
         (int)msg.length(),
         0);

    continue;
}


//==============================
// Join Group
// /join groupname
//==============================

if(text.rfind("/join ",0)==0)
{
    stringstream ss(text);

    string command;
    string groupName;

    ss >> command;
    ss >> groupName;

    joinGroup(groupName, user);

    string msg = "Joined group: " + groupName;

    send(clientSocket,
         msg.c_str(),
         (int)msg.length(),
         0);

    continue;
}


//==============================
// Group Message
// /gmsg groupname message
//==============================

if(text.rfind("/gmsg ",0)==0)
{
    stringstream ss(text);

    string command;
    string groupName;
    string msg;

    ss >> command;
    ss >> groupName;

    getline(ss, msg);

    if(msg.length() > 0 && msg[0] == ' ')
    {
        msg.erase(0,1);
    }

    sendGroupMessage(user,
                     groupName,
                     msg);

    continue;
}

        //========================
        // Normal Broadcast Chat
        //========================


        string fullMessage =
        "[" + user + "] : " + text;
        string encrypted = encryptDecrypt(fullMessage);



        cout<<fullMessage<<endl;
        logMessage(fullMessage);



       broadcastMessage(encrypted,
                 clientSocket);

    }



    //========================
    // Remove User
    //========================

    EnterCriticalSection(&clientsLock);


    clients.erase(
        remove(clients.begin(),
               clients.end(),
               clientSocket),
        clients.end()
    );


    userSockets.erase(user);



    LeaveCriticalSection(&clientsLock);



    closesocket(clientSocket);



    return 0;
}
int main()
{
    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        cout << "WSAStartup Failed\n";
        return 1;
    }

    InitializeCriticalSection(&clientsLock);

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocket == INVALID_SOCKET)
    {
        cout << "Socket Creation Failed\n";
        DeleteCriticalSection(&clientsLock);
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr{};

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(5555);

    if (bind(serverSocket,
             (sockaddr*)&serverAddr,
             sizeof(serverAddr)) == SOCKET_ERROR)
    {
        cout << "Bind Failed\n";

        closesocket(serverSocket);
        DeleteCriticalSection(&clientsLock);
        WSACleanup();
        return 1;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        cout << "Listen Failed\n";

        closesocket(serverSocket);
        DeleteCriticalSection(&clientsLock);
        WSACleanup();
        return 1;
    }

    cout << "=====================================\n";
    cout << " Offline LAN Messenger Server\n";
    cout << " Listening on Port 5555...\n";
    cout << "=====================================\n";

    while (true)
    {
        sockaddr_in clientAddr;
        int clientSize = sizeof(clientAddr);

        SOCKET clientSocket = accept(
            serverSocket,
            (sockaddr*)&clientAddr,
            &clientSize);

        if (clientSocket == INVALID_SOCKET)
        {
            cout << "Accept Failed\n";
            continue;
        }

        EnterCriticalSection(&clientsLock);
        clients.push_back(clientSocket);
        LeaveCriticalSection(&clientsLock);

        SOCKET* pClient = new SOCKET(clientSocket);

        HANDLE hThread = CreateThread(
            NULL,
            0,
            handleClient,
            pClient,
            0,
            NULL);

        if (hThread != NULL)
            CloseHandle(hThread);
    }

    DeleteCriticalSection(&clientsLock);

    closesocket(serverSocket);

    WSACleanup();

    return 0;
}