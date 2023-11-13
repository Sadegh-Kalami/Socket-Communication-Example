import socket

# Create a socket object
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Define the server address and port
server_address = ('localhost', 12345)

# Connect to the server
client_socket.connect(server_address)
print('Connected to {}:{}'.format(*server_address))
# Send a message to the server
message = "Python Client"
client_socket.send(message.encode())

# Receive the response from the server
response = client_socket.recv(1024)
print('Response from server:', response.decode())
# Close the socket
client_socket.close()