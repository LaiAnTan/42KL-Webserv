# Import modules for CGI handling
import os
import urllib.parse

dir_path = os.path.dirname(os.path.realpath(__file__)) + "/"
root_path = os.path.split(os.path.split(os.path.split(dir_path)[0])[0])[0] + "/root"

# Create instance of FieldStorage
# form = cgi.FieldStorage()

# Get data from fields
content_type = os.environ.get('CONTENT_TYPE')
f = open(dir_path + "/display_file.html", "r")

content = f.read()

custom_delete_buttons = []

if (len(os.listdir(root_path)) == 0):
    custom_delete_buttons.append("<tr><td>No File Uploaded!</td></tr>")
else:
    for filename in os.listdir(root_path):
        file_path = "/root/" + filename

        delete_button = f"""
        <tr>
        <td><a href="{urllib.parse.quote(file_path)}">{filename}</a></td>
        <td>
            <button class="delete_btn" onclick="makeDELETErequest('{file_path}')">
                Delete File
            </button>
        </td>
        </tr>
    """
        custom_delete_buttons.append(delete_button)

content = content.replace("TABLE_CONTENT", "".join(custom_delete_buttons))

print("Content-Type: %s" % (content_type), end="\r\n")
print("Content-Length: %s\r\n" % len(content), end="\r\n")
print(content)

# /usr/bin/python3 // for linux
# /usr/local/bin/python3 // for mac
