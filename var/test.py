#!/usr/local/bin/python3

import socket

request = """POST / HTTP/1.1\r
content-length: 10\r
\r
1234567890
"""


def start_client(host='localhost', port=9035):
	"""Starts a TCP client that connects to the specified server."""
	with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client_socket:
		client_socket.connect((host, port))
		print(f"Connected to server at {host}:{port}")

		client_socket.sendall(request.encode())
		data = client_socket.recv(1024)
		print(f"Received from server: {data.decode()}")

start_client()
