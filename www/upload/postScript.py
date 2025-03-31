#!/usr/bin/env python3
import cgi
import cgitb
import html

# Enable error reporting for debugging
cgitb.enable()

# Get form data
form = cgi.FieldStorage()

# Get and sanitize input
text_input = html.escape(form.getvalue('text', ''))
if text_input:
    with open('data/data.txt', 'a') as f:
        f.write(f"{text_input}\n")

# Print HTTP headers
print("Content-Type: text/html; charset=utf-8")
print()

# Print HTML response
print("""
<html>
<head>
    <title>Form Response</title>
</head>
<body>
    <h1>Form Submission Received</h1>
    <p>You submitted: <strong>{}</strong></p>
    <a href="/">Submit another</a>
</body>
</html>
""".format(text_input))