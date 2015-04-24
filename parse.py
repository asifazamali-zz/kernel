#/usr/bin/python
import re
f=open('/var/log/syslog','r')
for line in f:
	print line
