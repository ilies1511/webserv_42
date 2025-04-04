#!/usr/bin/env python3
import http.cookies as Cookie
import os
import uuid
from datetime import datetime, timedelta

# Create SimpleCookie instance
#cookie = Cookie.SimpleCookie()

# Load existing cookies from client


#if 'HTTP_COOKIE' in os.environ:
#    cookie.load(os.environ['HTTP_COOKIE'])

# Check for existing session cookie
had_id = True
session_id = os.getenv("session_id")
#if 'session_id' in cookie:
#    session_id = cookie['session_id'].value
#else:
if session_id is None or session_id == "":
    had_id = False
    # Generate new session ID if none exists
    session_id = str(uuid.uuid4())
    #cookie['session_id'] = session_id

    ## FIX: Use absolute timestamp for expiration
    #expires_time = datetime.now() + timedelta(seconds=5)
    ## cookie['session_id']['expires'] = expires_time.strftime("%a, %d-%b-%Y %H:%M:%S GMT")
    #cookie['session_id']['Max-Age'] = 15

print(f"Set-Cookie: session_id={session_id}; Max-Age=5\r")
# Output headers
#print(cookie.output())
# print("Set-Cookie: name=jo; Max-Age=15")
print("Content-Type: text/html\r")
print("\r")

# HTML content
print("""
<html>
<head><title>Session Test</title></head>
<body>
    <h1>Cookie Session Test</h1>
    {} 
    <p>Session ID: {}</p>
    <p>Refresh the page to test session persistence</p>
</body>
</html>
""".format(
    '<h2>Welcome Back!</h2>' if had_id else '<h2>New Session Created!</h2>',
    session_id
    #'<h2>Welcome Back!</h2>' if 'session_id' in cookie else '<h2>New Session Created!</h2>',
    #session_id
))