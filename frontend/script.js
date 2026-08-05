// Username

const username = localStorage.getItem("username");

if (!username) {
    window.location.href = "index.html";
}

document.getElementById("username").innerText = username;


// Chat Box

const chatBox = document.getElementById("chat-box");


// Online Users (Demo)

const users = [
    "Katyayani",
    "Vansh",
    "Rahul",
    "Priya"
];

const usersDiv = document.getElementById("users");

users.forEach(user => {

    let div = document.createElement("div");

    div.className = "user";

    div.innerHTML =
        "🟢 " + user;

    usersDiv.appendChild(div);

});


// Send Message

function sendMessage() {

    let input = document.getElementById("message");

    let text = input.value.trim();

    if (text === "")
        return;

    let msg = document.createElement("div");

    msg.className = "message me";

    msg.innerHTML =
        "<b>You :</b> " + text;

    chatBox.appendChild(msg);

    chatBox.scrollTop = chatBox.scrollHeight;

    input.value = "";

    /*
    Future Backend Connection

    fetch("http://127.0.0.1:8080/send",{
        method:"POST",
        headers:{
            "Content-Type":"application/json"
        },
        body:JSON.stringify({
            username:username,
            message:text
        })
    });

    */

}


// Enter Key

document.getElementById("message")
.addEventListener("keypress", function (e) {

    if (e.key === "Enter") {

        sendMessage();

    }

});


// Logout

function logout() {

    localStorage.removeItem("username");

    window.location.href = "index.html";

}


// Demo Incoming Messages

setInterval(() => {

    let replies = [
        "Hello 👋",
        "How are you?",
        "Welcome to DRDO LAN Messenger",
        "Testing successful ✅",
        "Server Connected"
    ];

    let random =
        replies[Math.floor(Math.random() * replies.length)];

    let div = document.createElement("div");

    div.className = "message";

    div.innerHTML =
        "<b>Server :</b> " + random;

    chatBox.appendChild(div);

    chatBox.scrollTop = chatBox.scrollHeight;

}, 10000);