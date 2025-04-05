#!/usr/bin/env python3
import cgi
import cgitb
import os
import pathlib

cgitb.enable()

print("Content-Type: text/html\n\n")
print("""<html>
<head>
    <title>File Manager</title>
    <style>
        .file-list { margin: 20px; }
        .file-item { 
            padding: 10px;
            margin: 15px 0;
            background: #f8f9fa;
            border: 1px solid #dee2e6;
            border-radius: 4px;
        }
        .preview-img {
            max-width: 300px;
            max-height: 200px;
            margin: 10px 0;
        }
        .delete-btn {
            background: #dc3545;
            color: white;
            padding: 5px 10px;
            border-radius: 3px;
            text-decoration: none;
            display: inline-block;
            margin-top: 5px;
        }
        .file-name {
            font-weight: bold;
            margin-bottom: 5px;
        }
    </style>
</head>
<body>
    <h2>Uploaded Files</h2>
    <div class="file-list">""")

DATA_DIR = "data/"
try:
    files = sorted(os.listdir(DATA_DIR), key=lambda x: os.path.getmtime(os.path.join(DATA_DIR, x)), reverse=True)
    
    if not files:
        print("<p>No files found in storage</p>")
    else:
        for filename in files:
            file_path = os.path.join(DATA_DIR, filename)
            if not os.path.isfile(file_path):
                continue
                
            file_ext = pathlib.Path(filename).suffix.lower()
            print(f"<div class='file-item'>")
            print(f"<div class='file-name'>{filename}</div>")
            
            if file_ext in ['.jpg', '.jpeg', '.png', '.gif']:
                print(f"<img class='preview-img' src='{DATA_DIR}{filename}' alt='{filename}'>")
            
            print(f"""
            <div class='file-actions'>
                <a href='{DATA_DIR}{filename}' target='_blank' class='delete-btn' 
                   style='background: #007bff; margin-right: 5px;'>Open</a>
                <form action='deleteFile.py' method='post' style='display: inline;'>
                    <input type='hidden' name='filename' value='{filename}'>
                    <button type='submit' class='delete-btn' 
                            onclick="return confirm('Delete {filename} permanently?')">
                        Delete
                    </button>
                </form>
            </div>
            """)
            print("</div>")

except Exception as e:
    print(f"<p style='color: red;'>Error reading directory: {e}</p>")

print("""</div>
    <p><a href="index.html">Back to main page</a></p>
</body>
</html>""")
