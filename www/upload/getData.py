#!/usr/bin/env python3
import cgi
import cgitb
import json
import os
import sys

cgitb.enable()  # Debugging

file_path = 'data/data.json'

# Set HTTP response headers
print("Content-Type: text/html; charset=utf-8\n")

try:
    # Check if the JSON file exists
    if os.path.exists(file_path):
        with open(file_path, 'r', encoding='utf-8') as f:
            try:
                data = json.load(f)
            except json.JSONDecodeError:
                data = []  # Reset to empty list if file is corrupted
    else:
        data = []

    # Generate HTML table rows
    table_rows = ""
    for entry in data:
        text = entry.get("text", "No text")
        timestamp = entry.get("timestamp", "Unknown time")
        table_rows += f"<tr><td>{text}</td><td>{timestamp}</td></tr>"

    # Generate full HTML page with Go Back button
    print(f"""
    <html>
    <head>
        <title>Stored Data</title>
        <style>
            body {{
                font-family: Arial, sans-serif;
                background-color: #f4f4f4;
                padding: 20px;
            }}
            h1 {{
                color: #333;
                text-align: center;
            }}
            table {{
                width: 80%;
                margin: 20px auto;
                border-collapse: collapse;
                background: white;
                border-radius: 8px;
                overflow: hidden;
                box-shadow: 0px 0px 10px rgba(0, 0, 0, 0.1);
            }}
            th, td {{
                padding: 12px;
                border-bottom: 1px solid #ddd;
                text-align: left;
            }}
            th {{
                background-color: #007BFF;
                color: white;
            }}
            tr:hover {{
                background-color: #f1f1f1;
            }}
            .container {{
                text-align: center;
            }}
            .go-back {{
                display: inline-block;
                background: #cce5ff;
                padding: 8px 15px;
                text-decoration: none;
                color: black;
                border-radius: 4px;
                border: 1px solid #999;
                margin: 20px 0;
                cursor: pointer;
            }}
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
    """)

except Exception as e:
    sys.exit(1)
    print(f"<h1>Error</h1><p>Unable to read data: {e}</p>")