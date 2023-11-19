# Import modules for CGI handling
import os

dir_path = os.path.dirname(os.path.realpath(__file__)) + "/"
root_path = os.path.split(os.path.split(os.path.split(dir_path)[0])[0])[0] + "/root"

# Create instance of FieldStorage
# form = cgi.FieldStorage()

# Get data from fields
first_name = os.environ.get('username')
last_name = os.environ.get('password')
content_type = os.environ.get('CONTENT_TYPE')
content_len = os.environ.get('CONTENT_LENGTH')

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

    content = f.read()

    custom_delete_buttons = []
    custom_delete_buttons.append("<table>")

    for filename in os.listdir(root_path):
        file_path =  "/root/" + filename

        delete_button = f"""\
        <tr>
        <td>{filename}</td>
        <td>
            <button class="delete_btn" onclick="makeDELETErequest('{file_path}')">
                Delete File
            </button>
        </td>
        </tr>
    """
        custom_delete_buttons.append(delete_button)
    custom_delete_buttons.append("</table>")

    content = content.replace("TABLE_CONTENT", "".join(custom_delete_buttons))

    print("Content-Type: %s" % (content_type), end="\r\n")
    print("Content-Length: %s\r\n" % len(content), end="\r\n")
    print(content)

# /usr/bin/python3 // for linux
# /usr/local/bin/python3 // for mac
