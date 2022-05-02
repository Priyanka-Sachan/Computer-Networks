import socket, pickle
from threading import Thread
from datetime import datetime

class Message:
    # init method or constructor 
    def __init__(self, username, timestamp,text):
        self.username = username
        self.timestamp=timestamp
        self.text=text
   
    # Print message
    def print(self):
        print(f"[{self.timestamp}] {self.username}: {self.text}")

SERVER_HOST = "127.0.0.1"
SERVER_PORT = 1234

# Initialize TCP socket
clientSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Connect to the server
print(f"[-] Connecting to {SERVER_HOST}:{SERVER_PORT}...")
clientSocket.connect((SERVER_HOST, SERVER_PORT))
print("[+] Connected.")

def listen_for_messages():
    while True:
        data = clientSocket.recv(1024)
        message =pickle.loads(data)
        message.print()

# Prompt the client for username
username = input("Username: ")

# Create a thread that listens for messages to this client & prints them
listenServer = Thread(target=listen_for_messages)
# make the thread daemon so it ends whenever the main thread ends
listenServer.daemon = True
# start the thread
listenServer.start()

# Listen for messages input by the client
while True:

    text =  input()
    print("\033[A                             \033[A")

    # Exit the program
    if text.lower() == 'q':
        break

    # Get timestamp of the message
    timestamp = datetime.now().strftime('%d %b %Y, %H:%M') 

    # Create an instance of Message() to send to server.
    message = Message(username,timestamp,text)

    # Pickle the message object and send it to the server
    data = pickle.dumps(message)
    clientSocket.send(data)

# Close the socket
clientSocket.close()