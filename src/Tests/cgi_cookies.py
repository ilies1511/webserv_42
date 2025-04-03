import os

#print("Status:1.1 200 OK\r")

body = os.environ.get("test0")
print("Set-Cookie: test0=cookie_body0\r")
print("Set-Cookie: test1=cookie_body1\r")
print("Set-Cookie: test2=cookie_body2\r")
print("Set-Cookie: test3=cookie_body3\r")
print("Set-Cookie: test4=cookie_body4\r")

if body is None or body == "":

    body = "no cookie"

print(f"Content-Length: {len(body)}\r")
print("\r")
print(body)
