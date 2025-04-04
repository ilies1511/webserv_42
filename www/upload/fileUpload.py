#!/usr/bin/env python3
import cgi
import cgitb
import os

# Enable error reporting
cgitb.enable()

# Configuration
UPLOAD_DIR = "data/"
MAX_SIZE = 10 * 1024 * 1024  # 10MB limit
ALLOWED_TYPES = {'image/png', 'image/jpeg', 'application/pdf'}

# Ensure upload directory exists
os.makedirs(UPLOAD_DIR, exist_ok=True)

# Get form data
form = cgi.FieldStorage()

print("Content-Type: text/html\n\n")
print("""<html>
<head><title>Upload Result</title></head>
<body>
<h1>Upload Result</h1>""")

# Check if file was submitted
if 'file' not in form:
    print("<p style='color: red;'>Error: No file submitted</p>")
else:
    fileitem = form['file']

    # Validate file
    if not fileitem.filename:
        print("<p style='color: red;'>Error: No file selected</p>")
    else:
        # Security checks
        filename = os.path.basename(fileitem.filename)
        filepath = os.path.join(UPLOAD_DIR, filename)
        content_type = fileitem.headers.get('Content-Type', '').lower()
        file_size = int(fileitem.headers.get('Content-Length', 0))

        # Validate file type and size
        if content_type not in ALLOWED_TYPES:
            print(f"<p style='color: red;'>Error: File type {content_type} not allowed</p>")
        elif file_size > MAX_SIZE:
            print(f"<p style='color: red;'>Error: File too large ({file_size//1024}KB)</p>")
        else:
            # Save the file
            try:
                with open(filepath, 'wb') as f:
                    while True:
                        chunk = fileitem.file.read(100000)
                        if not chunk:
                            break
                        f.write(chunk)
                print(f"<p>File '{filename}' uploaded successfully!</p>")
                print(f"<p><a href='index.html'>Return to upload form</a></p>")
            except Exception as e:
                print(f"<p style='color: red;'>Error saving file: {e}</p>")

print("</body></html>")