#!/usr/bin/env python3

import socket
import os

HOST = '0.0.0.0' 
PORT = 1234  

# Function to read a file in server
def readFile(file):
    with open(file, 'rb') as f:
        return f.read()

with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as serverSocket:

    # Binding the socket with server
    serverSocket.bind((HOST, PORT))
    print('Server up and listening ...')

    while True:
        print('Waiting for connection...')

        # Receiving data from a client
        message, clientAddress = serverSocket.recvfrom(1024)
        print('Connected with client ',clientAddress)
        file_name = str(message, 'utf-8')
        path=os.path.join(os.path.dirname(__file__),file_name)

        # If given file present in server, Send its content, Else send `File Not Found` error 
        if os.path.isfile(path):
            content = readFile(path)
        else:
            content=b'ERROR: File not found' 
        serverSocket.sendto(content,clientAddress)     
