#!/bin/bash

# Send plain text
echo "Simple text" | nc localhost 9034

# Send binary file
cat /bin/ls | nc localhost 9034

# Send random binary data
head -c 1024 /dev/urandom | nc localhost 9034

# Send large data
dd if=/dev/zero bs=1M count=10 | nc localhost 9034

# Test with special characters
echo -e "Special chars: \x00\x01\x02\xff" | nc localhost 9034

# Test with multi-line input
echo -e "Line 1\nLine 2\nLine 3" | nc localhost 9034
