#!/usr/bin/env python3
import cgi
import cgitb
import os
import html

cgitb.enable()

form = cgi.FieldStorage()
filename = form.getvalue('filename', '')
DATA_DIR = "data/"

print("Content-Type: text/html\n")
print("<html><head><title>Delete File</title></head><body>")

if not filename:
    print("<h1>Error</h1><p>No filename specified</p>")
else:
    filename = html.escape(filename)
    filepath = os.path.join(DATA_DIR, os.path.basename(filename))
    try:
        if os.path.exists(filepath) and os.path.isfile(filepath):
            os.remove(filepath)
            print(f"""
            <h1>File Deleted</h1>
            <p>{filename} has been deleted</p>
            <p><a href='showFiles.py'>Back to file list</a></p>
            """)
        else:
            print(f"<h1>Error</h1><p>File {filename} not found</p>")
    except Exception as e:
        print(f"<h1>Error</h1><p>Could not delete file: {str(e)}</p>")

print("</body></html>")
