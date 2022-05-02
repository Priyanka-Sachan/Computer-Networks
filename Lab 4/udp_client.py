#!/usr/bin/env python3
# ipc_client.py

import socket

HOST = '0.0.0.0'   # The server's hostname or IP address
PORT = 1234        # The port used by the server

with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as clientSocket:

    file_name = input('Enter the file name: ')

    # Encode given file name in UTF-8 format
    data=file_name.encode('utf-8')

    # Send data to server host and defined port
    clientSocket.sendto(data,(HOST,PORT)) 

    # Receive data from server in packets of 4096 bytes (4kB)
    data,server_address = clientSocket.recvfrom(4096)

    # Decode the received content
    file_content=str(data, 'utf-8')
    
    print(file_content)