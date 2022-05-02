import socket,csv
from _thread import *

SERVER_HOST = '127.0.0.1'
SERVER_PORT = 2004
threadCount = 0

# Initialize TCP socket
serverSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Binding server socket to host address & port
try:
    serverSocket.bind((SERVER_HOST, SERVER_PORT))
except socket.error as e:
    print(str(e))

# Listen for upcoming connections
serverSocket.listen(5)
print(f'[+] Listening as {SERVER_HOST}:{SERVER_PORT}.')

# Reading csv file
csv_file = open('db.csv')
csv_reader = list(csv.reader(csv_file, delimiter=','))
print('[+] CSV file loaded.')

# Function run by each thread for a client
def listen_for_client(connection,address):

    msg='****** WELCOME TO PORTAL ******'
    connection.send(msg.encode('utf-8'))

    while True:
        try:
            # Keep listening for a message from `client socket` 
            query = connection.recv(1024).decode('utf-8')
            flag = False
            
            # Enumerating through csv file to get data
            for row in csv_reader:
                if row[0].lower() == query:
                    flag=True
                    break
            data = row[1] if flag else 'Not found'
            connection.send(data.encode('utf-8'))

        except Exception as e:
            # Error, hence disconnect the client
            print(f'[!] Error: {e}')  
            print(f'[-] {address} disconnected.')
            break

    connection.close()

while True:

    # Accept incoming request from clients
    clientSocket, clientAddress = serverSocket.accept()
    print(f'[+] {clientAddress} connected.')
    
    # Start a new thread for each client
    start_new_thread(listen_for_client, (clientSocket, clientAddress))
    threadCount += 1
    print(f'Thread Number: {threadCount}')

# Close server socket    
serverSocket.close()