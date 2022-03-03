#!/usr/bin/env python3

import socket
import os
import threading

HOST = '' 
PORT = 1234  

def readFile(file):
    with open(file, 'rb') as f:
        return f.read()

def runConnectionSocket(socket):
    connectionSocket = socket
    with connectionSocket:
        message = str(connectionSocket.recv(4096), 'utf-8')
        request, path, version = message.splitlines()[0].split()
        print(request, path, version)

        if os.path.isfile(os.curdir + path):
            header='HTTP/1.0 200 OK\r\n'+'Content-Type: text/html\r\n'+'\r\n' 
            content = readFile(os.curdir+path)
        else:
            header='HTTP/1.0 404 Not Found\r\n'+'Content-Type: text/html\r\n'+'\r\n' 
            content = readFile(os.curdir+'/Error404.html')
        connectionSocket.sendall(header.encode()+content)

class ClientThread(threading.Thread):
    def __init__(self, client_address, client_socket):
        threading.Thread.__init__(self)
        self.client_socket = client_socket
        self.client_address = client_address
        print("New connection added : %s" % str(client_address))

    def run(self):
        runConnectionSocket(self.client_socket)

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as serverSocket:
    serverSocket.bind((HOST, PORT))
    serverSocket.listen()

    while True:
        print('Waiting for connection...')
        connectionSocket, clientAddress = serverSocket.accept()
        new_thread = ClientThread(clientAddress, connectionSocket)
        new_thread.start()
