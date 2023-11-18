# Import modules for CGI handling
import os

dir_path = os.path.dirname(os.path.realpath(__file__)) + "/"

# Create instance of FieldStorage
# form = cgi.FieldStorage()

# Get data from fields
first_name = os.environ.get('username')
last_name = os.environ.get('password')
content_type = os.environ.get('CONTENT_TYPE')
content_len = os.environ.get('CONTENT_LENGTH')
# data = os.environ.get('DATA')

if (first_name is None and last_name is None):
    content = """
<!DOCTYPE html>
<html>
<head>
    <style>
        body {
        font-family: Arial, Helvetica, sans-serif;
        background-color: #000000;
        }

        * {
        box-sizing: border-box;
        }

        p {
        color: white;
        }

        .center {
        margin: auto;
        width: 50%;
        padding: 10px;
        text-align: center;
        }

        img {
        display: block;
        margin-left: auto;
        margin-right: auto;
        width: 40%;
        border: 3px solid white
        }
    </style>
    <title>Hm</title>
</head>
<body>
    <div class="center">
    <p text-align="center">you arent supposed to be here</p>
    <img src="/image/door.webp" alt="Image">
    </div>
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
