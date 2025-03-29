// <?php
// // Set the Content-Type header
// header("Content-Type: text/html\n\n");
//
// // Get the current time
// $current_time = date("Y-m-d H:i:s");
//
// echo "<html>";
// echo "<head><title>CGI Example</title></head>";
// echo "<body>";
// echo "<h1>PHP CGI!</h1>";
//
// // Display current time
// echo "<p>Current Time: $current_time</p>";
//
// // Check if "name" parameter is set in GET or POST
// if (!empty($_REQUEST['name'])) {
//     // Handle multiple "name" parameters
//     $names = (array) $_REQUEST['name'];
//
//     foreach ($names as $user_name) {
//         $safe_name = htmlspecialchars($user_name, ENT_QUOTES, 'UTF-8');
//         echo "<p>Hello, $safe_name!</p>";
//     }
// } else {
//     echo "<p>No name parameter provided in the query string.</p>";
// }
//
// echo "</body>";
// echo "</html>";
// ?>

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