#!/usr/bin/env python3

import socket
import os
import threading

HOST = ''  
PORT = 1234  

def readFile(file):
    with open(file, 'rb') as f:
        return f.read()

def runConnectionSocket(socket,mode):
    global currentRequests
    connectionSocket = socket
    with connectionSocket:
        if mode==1:
            message = str(connectionSocket.recv(4096), 'utf-8')
            # print(message)
            request, path, version = message.splitlines()[0].split()
            print(request, path, version)
            if path=='/':
                header='HTTP/1.0 200 OK\n'+'Content-Type: text/html\n'+'\n' 
                content = readFile(os.curdir+defaultFile)
            elif os.path.isfile(os.curdir + path):
                _, fileExtension = os.path.splitext(path)
                if fileExtension in ['.jpg','.jpeg','.png']:
                    contentType='image/jpeg'
                else:
                    contentType='text/html'
                header='HTTP/1.0 200 OK\n'+'Content-Type: '+contentType+'\n'+'\n' 
                content = readFile(os.curdir+path)
            else:
                header='HTTP/1.0 404 Not Found\n'+'Content-Type: text/html\n'+'\n' 
                content = readFile(os.curdir+'/Error404.html')
        elif mode==2:
            header='HTTP/1.0 403 Forbidden\n'+'Content-Type: text/html\n'+'\n' 
            content = readFile(os.curdir+'/Error403.html')
        elif mode==3:
            header='HTTP/1.0 503 Service Unavailable\n'+'Content-Type: text/html\n'+'\n'
            content = readFile(os.curdir+'/Error503.html')
        connectionSocket.sendall(header.encode()+content)
    currentRequests-=1


class ClientThread(threading.Thread):
    def __init__(self, client_address, client_socket,mode):
        threading.Thread.__init__(self)
        self.client_socket = client_socket
        self.client_address = client_address
        self.mode = mode
        print("New connection added : %s" % str(client_address))

    def run(self):
        runConnectionSocket(self.client_socket,self.mode)

def load_properties(filepath, sep='=', comment_char='#'):
    props = {}
    with open(filepath, "rt") as f:
        for line in f:
            l = line.strip()
            if l and not l.startswith(comment_char):
                key_value = l.split(sep)
                key = key_value[0].strip()
                value = sep.join(key_value[1:]).strip().strip('"') 
                props[key] = value 
    return props

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as serverSocket:
    serverSocket.bind((HOST, PORT))
    serverSocket.listen()

    prop = load_properties("config.properties")
    maxRequests = int(prop.get("max_request"))
    currentRequests = 0
    defaultFile = prop.get("default_html")
    defaultPath = prop.get("default_path")
    blockedIP = prop.get("ip_block").split(",")

    while True:
        print('\nWaiting for connection...')
        connectionSocket, clientAddress = serverSocket.accept()
        clientIP,clientHost = clientAddress
        if clientIP not in blockedIP and currentRequests<maxRequests:
            new_thread = ClientThread(clientAddress, connectionSocket,1)
        elif clientIP in blockedIP:
            new_thread = ClientThread(clientAddress, connectionSocket,2)
        elif currentRequests >= maxRequests:
            new_thread = ClientThread(clientAddress, connectionSocket,3)
        currentRequests+=1
        new_thread.start()
        