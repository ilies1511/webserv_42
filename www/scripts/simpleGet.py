#!/usr/bin/env python3

# Import necessary CGI modules
import cgi
import html
import datetime

# while True:
#     pass

current_time = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
# Print the required HTTP headers
print("Status: 200 OK")
print("Content-Type: text/html\n")

# Print the HTML content
print("<html>")
print("<head><title>CGI Example</title></head>")
print("<body>")
print("<h1>Python CGI!</h1>")
print(f"<p>Current Time: {current_time}</p>")
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
