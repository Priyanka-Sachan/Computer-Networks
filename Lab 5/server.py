import socket
from threading import Thread

SERVER_HOST = "0.0.0.0"
SERVER_PORT = 1234

# Initialize list/set of all connected client's sockets
clientSockets = set()

# Initialize TCP socket
serverSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# Make the port as reusable port
serverSocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
# Bind the socket to the address we specified
serverSocket.bind((SERVER_HOST, SERVER_PORT))
# Listen for upcoming connections
serverSocket.listen(5)
print(f"[-] Listening as {SERVER_HOST}:{SERVER_PORT}")

def listen_for_client(socket):
    while True:
        try:
            # Keep listening for a message from `socket` 
            data = socket.recv(1024)
        except Exception as e:
            # Client no longer connected, hence remove from set
            print(f"[!] Error: {e}")
            clientSockets.remove(socket)

        # Iterate over all connected sockets to send the message
        for clientSocket in clientSockets:
            clientSocket.send(data)

def add_new_client(socket):
    # Add the new connected client to connected sockets
    clientSockets.add(socket)
    # Start a new thread that listens for each client's messages
    client = Thread(target=listen_for_client, args=(socket,))
    # Make the thread daemon so it ends whenever the main thread ends
    client.daemon = True
    # Start the thread
    client.start()

# Listen and accept new connections all the time
while True:
    clientSocket, clientAddress = serverSocket.accept()
    print(f"[+] {clientAddress} connected.")
    add_new_client(clientSocket)

# Close client sockets
for clientSocket in clientSockets:
    clientSocket.close()

# Close server socket
serverSocket.close()
