# Import modules for CGI handling
import os

import os 
dir_path = os.path.dirname(os.path.realpath(__file__)) + "/"

# Create instance of FieldStorage
# form = cgi.FieldStorage()

# Get data from fields
first_name = os.environ.get('first_name')
last_name = os.environ.get('last_name')
content_type = os.environ.get('CONTENT_TYPE')
content_len = os.environ.get('CONTENT_LENGTH')
# data = os.environ.get('DATA')

if (first_name == "" and last_name == ""):
    content = """
<!DOCTYPE html>
<html>
<head>
    <title>Hm</title>
</head>
<body>
    <p>you arent supposed to be here</p>
</body>
</html>
"""
    print("Content-Type: %s" % (content_type))
    print("Content-Length: %s" % len(content))
    print("\r\n")

    print(content)
else:
    f = open(dir_path + "../html/file.html", "r")
    counter = open(dir_path + "../html/file.html", "r")

    print("Content-Type: %s" % (content_type))
    print("Content-Length: %s" % (sum(len(x) for x in counter.readlines())))
    print("\r\n")

    print(f.read())
    # print (data)
    # print ("<html>")
    # print ('<head>')
    # print ("<title>Hello - Second CGI Program</title>")
    # print ('</head>')
    # print ('<body>')
    # print ("<h2>Hello %s %s</h2>" % (first_name, last_name))
    # print ('</body>')
    # print ('</html>')

    # /usr/bin/python3 // for linux
    # /usr/local/bin/python3 // for mac
