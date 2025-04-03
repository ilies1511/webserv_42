#!/usr/bin/env python3
import cgi
import cgitb
import json
import os
import sys

cgitb.enable()  # Debugging (outputs errors to the browser)

file_path = 'data/data.json'

try:
    # Check if the JSON file exists and load data
    if os.path.exists(file_path):
        with open(file_path, 'r', encoding='utf-8') as f:
            try:
                data = json.load(f)
            except json.JSONDecodeError:
                data = []  # Reset if corrupted
    else:
        data = []

    # Generate HTML content
    table_rows = "".join(
        f"<tr><td>{entry.get('text', 'No text')}</td><td>{entry.get('timestamp', 'Unknown')}</td></tr>"
        for entry in data
    )

    html_content = f"""
    <html>
    <head>
        <title>Stored Data</title>
        <style>
            /* ... (keep your existing styles) ... */
        </style>
    </head>
    <body>
        <h1>Stored Submissions</h1>
        <div class="container">
            <table>
                <tr>
                    <th>Submitted Text</th>
                    <th>Timestamp</th>
                </tr>
                {table_rows if table_rows else '<tr><td colspan="2">No submissions yet</td></tr>'}
            </table>
            <a href="/upload/" class="go-back">Go Back</a>
        </div>
    </body>
    </html>
    """

    # Successful response (200 OK)
    print("Content-Type: text/html; charset=utf-8")
    print()
    print(html_content)

except json.JSONDecodeError as e:
    # Handle corrupt JSON specifically
    print("Status: 400 Bad Request")
    print("Content-Type: text/html; charset=utf-8")
    print()
    print(f"<h1>Invalid Data Format</h1><p>Corrupted JSON file: {str(e)}</p>")

except PermissionError as e:
    # Handle file permission issues
    print("Status: 403 Forbidden")
    print("Content-Type: text/html; charset=utf-8")
    print()
    print("<h1>Access Denied</h1><p>Check file permissions</p>")

except FileNotFoundError as e:
    # Handle missing file
    print("Status: 404 Not Found")
    print("Content-Type: text/html; charset=utf-8")
    print()
    print("<h1>Data File Missing</h1>")