#!/usr/bin/env python3
import cgi
import cgitb
import html
import os
import sys
import json
from datetime import datetime

cgitb.enable()  # Debugging (disable in production)

try:
    # Get and sanitize form input
    form = cgi.FieldStorage()
    text_input = html.escape(form.getvalue('text', '')).strip()

    # Validate required field
    if not text_input:
        print("Status: 400 Bad Request")
        print("Content-Type: text/html; charset=utf-8\n")
        print("<h1>Bad Request</h1><p>Text input is required</p>")
        sys.exit(0)

    file_path = 'data/data.json'

    # Create directory structure if needed
    try:
        os.makedirs(os.path.dirname(file_path), exist_ok=True)
    except PermissionError as e:
        raise PermissionError(f"Can't create directory: {str(e)}") from e

    # Read existing data
    data = []
    if os.path.exists(file_path):
        try:
            with open(file_path, 'r', encoding='utf-8') as f:
                data = json.load(f)
        except json.JSONDecodeError as e:
            raise json.JSONDecodeError(
                f"Corrupted data file: {str(e)}", e.doc, e.pos
            ) from e

    # Add new entry
    new_entry = {
        "text": text_input,
        "timestamp": datetime.now().strftime("%B %d, %Y, %I:%M %p")
    }
    data.append(new_entry)

    # Write updated data
    try:
        with open(file_path, 'w', encoding='utf-8') as f:
            json.dump(data, f, indent=4)
    except PermissionError as e:
        raise PermissionError(f"Can't write to file: {str(e)}") from e

    # Successful response
    print("Content-Type: text/html; charset=utf-8\n")
    print(f"""
    <html>
    <head>
        <title>Submission Successful</title>
    </head>
    <body>
        <h1>Form Submission Received</h1>
        <p>You submitted: <strong>{text_input}</strong></p>
        <p>Timestamp: <strong>{new_entry['timestamp']}</strong></p>
        <a href="/upload/">Submit another</a>
    </body>
    </html>
    """)

except json.JSONDecodeError as e:
    print("Status: 400 Bad Request")
    print("Content-Type: text/html; charset=utf-8\n")
    print(f"<h1>Invalid Data</h1><p>{html.escape(str(e))}</p>")

except PermissionError as e:
    print("Status: 403 Forbidden")
    print("Content-Type: text/html; charset=utf-8\n")
    print(f"<h1>Access Denied</h1><p>{html.escape(str(e))}</p>")

except FileNotFoundError as e:
    print("Status: 404 Not Found")
    print("Content-Type: text/html; charset=utf-8\n")
    print(f"<h1>File Missing</h1><p>{html.escape(str(e))}</p>")

except OSError as e:  # Catch-all for other filesystem errors
    print("Status: 500 Internal Server Error")
    print("Content-Type: text/html; charset=utf-8\n")
    print(f"<h1>Storage Error</h1><p>{html.escape(str(e))}</p>")

except Exception as e:  # General catch-all
    print("Status: 500 Internal Server Error")
    print("Content-Type: text/html; charset=utf-8\n")
    print(f"<h1>Unexpected Error</h1><p>{html.escape(str(e))}</p>")