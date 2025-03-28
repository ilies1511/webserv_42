#!/usr/bin/env python3

# Import necessary CGI modules
import cgi
import html

# Print the required HTTP headers
# print("HTTP/1.1 200 OK")
print("Content-type: text/html\n")

# Print the HTML content
print("<html>")
print("<head><title>CGI Example</title></head>")
print("<body>")
print("<h1>Hello from Webserv!</h1>")

# Use cgi module to get any query parameters passed in the URL
form = cgi.FieldStorage()

# Display the values of any query parameters if present
if "name" in form:

    names = form.getlist("name")
    for user_name in names:
        user_name = html.escape(user_name)
        # user_name = html.escape(form.getvalue("name"))
        print(f"<p>Hello, {user_name}!</p>")
else:
    print("<p>No name parameter provided in the query string.</p>")

print("</body>")
print("</html>")
