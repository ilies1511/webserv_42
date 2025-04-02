#!/usr/bin/env python3
import cgi
import cgitb
import html
import os
import sys
import json
from datetime import datetime

cgitb.enable()  # Debugging

try:
    form = cgi.FieldStorage()
    text_input = html.escape(form.getvalue('text', ''))

    if text_input:
        file_path = 'data/data.json'
        os.makedirs(os.path.dirname(file_path), exist_ok=True)

        # Read existing data if the file exists
        if os.path.exists(file_path):
            with open(file_path, 'r', encoding='utf-8') as f:
                try:
                    data = json.load(f)
                except json.JSONDecodeError:
                    data = []  # Reset to empty list if file is corrupted
        else:
            data = []

        # Generate human-readable timestamp
        timestamp = datetime.now().strftime("%B %d, %Y, %I:%M %p")  # e.g., "April 1, 2025, 12:34 PM"

        # Append new entry
        new_entry = {
            "text": text_input,
            "timestamp": timestamp
        }
        data.append(new_entry)

        # Write back JSON data
        with open(file_path, 'w', encoding='utf-8') as f:
            json.dump(data, f, indent=4)

    # Success: Send HTML response
    print("Content-Type: text/html; charset=utf-8\n")
    print()
    print(f"""
    <html>
    <head>
        <title>Form Response</title>
    </head>
    <body>
        <h1>Form Submission Received</h1>
        <p>You submitted: <strong>{text_input}</strong></p>
        <p>Timestamp: <strong>{new_entry['timestamp']}</strong></p>
        <a href="/upload/">Submit another</a>
    </body>
    </html>
    """)

except Exception as e:
    sys.exit(1)  # Explicitly signal failure to parent
