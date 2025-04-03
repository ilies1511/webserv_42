<?php
// Print content type header with required newlines
echo "Content-Type: text/html\n\n";

// Get current time
$current_time = date("Y-m-d H:i:s");

// Start HTML output
echo <<<HTML
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>PHP CGI Example</title>
</head>
<body>
    <h1>PHP CGI</h1>
    <p>Current Date and Time: $current_time</p>
HTML;

// End HTML
echo <<<HTML
</body>
</html>
HTML;
?>