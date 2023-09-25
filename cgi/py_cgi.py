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
content_length = os.environ.get('CONTENT_LENGTH')
data = os.environ.get('DATA')
data_len = os.environ.get('DATA_LEN')

print ("Content-type: %s" % (content_type))
print ("Content-length: %s" % (content_length))
print ("\r\n")

# print (data_len)
i = 0
while i < int(data_len):
	print (data[i])
	i += 1
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