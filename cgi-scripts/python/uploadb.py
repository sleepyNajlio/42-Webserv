import sys
import io
import os

# Wrap sys.stdin with io.BufferedReader to read binary data
sys.stdin = io.BufferedReader(sys.stdin.buffer)

# Read the request body as binary data
request_body = sys.stdin.read()

# Process the binary data
# Modify this section according to your specific processing logic
if request_body:
    # Process the binary data as needed
    # For example, you can save it to a file
    with open("/Users/najlio/test/jpew.jpg", "wb") as f: # change the path to env variable
        f.write(request_body)
    message = "Binary data received and processed successfully"
else:
    message = "No binary data was received"

# Print the response
print("Content-Type: text/html\n")
print("<html><body>")
print(f"<p>{message}</p>")
print("</body></html>")
