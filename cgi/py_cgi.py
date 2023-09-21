# Import modules for CGI handling
import cgi
import cgitb
import os

# Create instance of FieldStorage
# form = cgi.FieldStorage()

# Get data from fields
first_name = os.environ.get('first_name')
last_name = os.environ.get('last_name')
content_type = os.environ.get('CONTENT_TYPE')
data = os.environ.get('DATA')

print ("Content-type: %s" % (content_type))
print("\r\n")

print (data)
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