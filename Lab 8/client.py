import socket

SERVER_HOST = '127.0.0.1'
SERVER_PORT = 2004

# Initialize TCP socket
clientSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Connect to the server
print(f'[-] Connecting to {SERVER_HOST}:{SERVER_PORT}...')
try:
    clientSocket.connect((SERVER_HOST, SERVER_PORT))
    print('[+] Connected.')
except socket.error as e:
    print(str(e))
    
msg = clientSocket.recv(1024).decode('utf-8')
print(msg)
print('[*] Enter q for exiting the chatbot.')

# Listen for messages input by the client
while True:

    query = input('\nEnter ailment: ').lower()
    # Exit the program
    if query== 'q':
        break
    clientSocket.sendall(query.encode('utf-8'))

    sol = clientSocket.recv(1024).decode('utf-8').replace('\\n','\n')
    print(f'Available Medicines: \n{sol}')

# Close the socket
clientSocket.close()