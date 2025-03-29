#!/usr/bin/env python3
import socket
import argparse

#!/usr/bin/env python3
import socket
import argparse

"""
	Usage:
		explicit
		./client.py --host localhost --port 9035 --method GET --uri /
		./client.py --host localhost --port 9035 --method DELETE --uri /files/sample.txt
		./client.py --host localhost --port 9035 --method POST --uri /upload

		implicit via default values (host, port)
		./client.py --method DELETE --uri /files/sample.txt

	In .zshrc Config:
		run_client() {
			if [ "$#" -eq 0 ]; then
				echo "Usage: run_client [METHOD] [URI] OR run_client [HOST] [PORT] [METHOD] [URI]"
				return 1
			elif [ "$#" -eq 2 ]; then
				# Zwei Parameter: METHOD und URI; Host und Port auf Standard setzen
				local host="localhost"
				local port="9035"
				local method="$1"
				local uri="$2"
			elif [ "$#" -eq 4 ]; then
				# Vier Parameter: HOST, PORT, METHOD, URI
				local host="$1"
				local port="$2"
				local method="$3"
				local uri="$4"
			else
				echo "Usage: run_client [METHOD] [URI] OR run_client [HOST] [PORT] [METHOD] [URI]"
				return 1
			fi

			echo "Using host: $host, port: $port, method: $method, uri: $uri"

			# Starte den Python-Client
			./src/Tests/baba_test.py --host "$host" --port "$port" --method "$method" --uri "$uri"
		}
"""

def start_client(host, port, request_text):
	"""
	Startet einen TCP-Client, verbindet sich mit dem angegebenen Server,
	sendet die Request und gibt die empfangene Response aus.
	"""
	try:
		with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client_socket:
			client_socket.connect((host, port))
			print(f"Connected to server at {host}:{port}")
			client_socket.sendall(request_text.encode())

			# Lese die Response in einem Loop, bis der Server die Verbindung schließt
			response = b""
			while True:
				data = client_socket.recv(4096)
				if not data:
					break
				response += data

			print("Received from server:")
			print(response.decode(errors='replace'))
	except Exception as e:
		print(f"Error: {e}")

def build_request(method, uri, host, port):
	"""
	Baut eine einfache HTTP-Request-Zeichenkette.
	Verwendet CRLF als Zeilenende.
	"""
	# Erstelle die einzelnen Zeilen des Requests
	request_lines = [
		f"{method} {uri} HTTP/1.1",
		f"Host: {host}:{port}",
		"Connection: close",  # Einfachheitshalber
		"",
		""
	]
	return "\r\n".join(request_lines)

def main():
	parser = argparse.ArgumentParser(
		description="Ein einfacher TCP-Client, um HTTP DELETE/GET/POST-Requests zu testen."
	)
	parser.add_argument("--host", default="localhost", help="Server-Hostname (default: localhost)")
	parser.add_argument("--port", type=int, default=9035, help="Server-Port (default: 9035)")
	parser.add_argument("--method", default="GET", help="HTTP-Methode (default: GET)")
	parser.add_argument("--uri", default="/", help="Request-URI (default: /)")
	args = parser.parse_args()

	request_text = build_request(args.method, args.uri, args.host, args.port)
	print("Request text:")
	print(request_text)
	start_client(args.host, args.port, request_text)

if __name__ == "__main__":
	main()
