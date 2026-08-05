# Offline LAN Messenger

A secure **Offline LAN Messenger** developed during my Summer Training at **DESIDOC, DRDO**. The application enables multiple users connected to the same **Local Area Network (LAN)** to communicate without requiring an internet connection.

The project follows a **Client–Server Architecture** using **TCP Socket Programming** with the **Winsock2** library. It supports real-time messaging, private messaging, group communication, file sharing, chat logging, and a browser-based frontend.

---

## 📌 Project Overview

The Offline LAN Messenger is designed for organizations where internet access is restricted or unavailable. Instead of relying on cloud-based messaging services, the application operates completely inside a Local Area Network, ensuring secure and reliable communication.

The system consists of a centralized server that manages client connections and routes messages between users. A lightweight frontend built using HTML, CSS, and JavaScript provides an easy-to-use interface for communication.

---

## ✨ Features

- Real-time LAN Messaging
- Client–Server Architecture
- Multiple Client Support
- Broadcast Messaging
- Private Messaging
- Group Chat
- File Sharing
- Chat Logging
- XOR-based Message Encryption
- Online User Management
- Server Statistics
- Browser-based User Interface
- Lightweight and Easy to Deploy

---

## 🛠️ Technologies Used

### Backend
- C++
- Winsock2
- TCP Socket Programming
- Multithreading

### Frontend
- HTML5
- CSS3
- JavaScript

### Development Tools
- Visual Studio Code
- MinGW (G++)
- Git
- GitHub

---

## 📂 Project Structure

```
Offline-LAN-Messenger
│
├── backend
│   ├── server.cpp
│   └── ...
│
├── client
│   ├── client.cpp
│   └── ...
│
├── frontend
│   ├── index.html
│   ├── chat.html
│   ├── style.css
│   └── script.js
│
├── logs
├── uploads
├── README.md
└── ...
```

---

## ⚙️ How to Run

### Clone Repository

```bash
git clone https://github.com/katyayanitiwari/Offline-LAN-Messenger.git
```

### Start Server

```bash
cd backend
g++ server.cpp -o server.exe -lws2_32
server.exe
```

### Start Client

```bash
cd client
g++ client.cpp -o client.exe -lws2_32
client.exe
```

Enter your username after connecting.

---

## 📸 Screenshots

Add screenshots here:

- Login Page
- Chat Interface
- Server Console
- Broadcast Messaging
- Private Messaging
- File Sharing

---

## 🚀 Future Enhancements

- AES Encryption
- User Authentication
- Database Integration
- Voice & Video Calling
- Multimedia File Sharing
- Cross-Platform Support
- Mobile Application
- Offline Message Synchronization

---

## 📚 Internship Information

**Organization:** Defence Scientific Information & Documentation Centre (DESIDOC)

**Parent Organization:** Defence Research and Development Organisation (DRDO)

**Training Duration:** 15 June 2026 – 07 August 2026

---

## 👩‍💻 Author

**Katyayani Tiwari**

B.Tech Electronics Engineering (VLSI Design & Technology)

Vivekananda Institute of Professional Studies – Technical Campus (VIPS-TC)

---

## 📄 License

This project is intended for academic and educational purposes.