#!/usr/bin/env python3
import cgi
import cgitb
import os
from os import path

cgitb.enable()  # Enable error reporting

DATA_DIR = "data/"
DELETE_SCRIPT = "delete_file.cgi"

print("Content-Type: text/html\n\n")
print("""<html>
<head>
    <title>File Manager</title>
    <style>
        .file-list { margin: 20px; }
        .file-item { 
            padding: 10px;
            margin: 5px 0;
            background: #f5f5f5;
            border-radius: 4px;
            display: flex;
            align-items: center;
            justify-content: space-between;
        }
        .delete-btn {
            background: #ff6666;
            color: white;
            border: none;
            padding: 5px 10px;
            border-radius: 3px;
            cursor: pointer;
        }
        .delete-btn:hover { background: #ff4444; }
    </style>
</head>
<body>
    <h2>Stored Files</h2>
    <div class="file-list">""")

# Get list of files
try:
    files = [f for f in os.listdir(DATA_DIR) if path.isfile(path.join(DATA_DIR, f))]

    if not files:
        print("<p>No files found in storage</p>")
    else:
        for filename in files:
            print(f"""<div class="file-item">
                <span>{filename}</span>
                <form action="{DELETE_SCRIPT}" method="post" style="display: inline;">
                    <input type="hidden" name="filename" value="{filename}">
                    <button type="submit" class="delete-btn" 
                            onclick="return confirm('Delete {filename}?')">
                        Delete
                    </button>
                </form>
            </div>""")

except Exception as e:
    print(f"<p style='color: red;'>Error reading directory: {e}</p>")

print("""</div>
    <p><a href="index.html">Back to main page</a></p>
</body>
</html>""")