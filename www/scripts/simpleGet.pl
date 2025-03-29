#!/usr/bin/env perl

use strict;
use warnings;
use CGI;

# Create CGI object
my $cgi = CGI->new;

# Set content type
print $cgi->header("text/html");

# Get the current time
my ($sec, $min, $hour, $mday, $mon, $year) = localtime();
$year += 1900;
$mon += 1;
my $current_time = sprintf("%04d-%02d-%02d %02d:%02d:%02d", $year, $mon, $mday, $hour, $min, $sec);

# Print the HTML content
print <<END_HTML;
<html>
<head><title>CGI Example</title></head>
<body>
<h1>Perl CGI!</h1>
<p>Current Time: $current_time</p>
END_HTML

# Check if "name" parameter exists
my @names = $cgi->param('name');
if (@names) {
    foreach my $user_name (@names) {
        $user_name = $cgi->escapeHTML($user_name);  # Escape HTML characters
        print "<p>Hello, $user_name!</p>\n";
    }
} else {
    print "<p>No name parameter provided in the query string.</p>\n";
}

print "</body></html>\n";
