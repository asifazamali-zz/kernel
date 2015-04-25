#!/usr/bin/python
import re
import sys
import time
import os


timer=int((sys.argv)[1])
time.sleep(timer)
f=open(r"/var/log/syslog")
if(sum(1 for l in open(r"/var/log/syslog")  )<2):
	print"No trace:syslog file empty\n"
	sys.exit(1)
cpu1=[]
cpu2=[]
complete=[]
R=0
W=0
CR=0
CW=0
rs=0
ws=0
cs=0
C=0
Cs=0
cr=0
cw=0
lnum=0
timer=10
for line in f:
	lnum=lnum+1
	if(r'.*[0-9],[0-9]',line):
		l=re.split('\]',line,1)
		if l[0]!='\n':
			print l[0]
			m=re.split(' ',l[1])	
			if m[2]=='0':
				if m:
		 			cpu1.append(m)
			elif m[2]=='1':
				if m:
					cpu2.append(m)
			else:
				complete.append(m)
			
#		print "cpu1"+str(cpu1)
#		print "cpu2"+str(cpu2)
for a in cpu1:
	if a[7]=='R':
		R=R+1
		k=re.split('\+',str(a[8]))
		if (k[0]!='\n'and k):
			rs+=int(k[1])
	else:
	 	W=W+1
		k=re.split('\+',str(a[8]))
		if k[0]!='\n'and k:
			ws+=int(k[1])
	
for a in cpu2:
        if a[7]=='R':
                CR=CR+1
		k=re.split('\+',str(a[8]))
		if k[0]!='\n' and k:
			cr+=int(k[1])
	
        else:
                CW=CW+1
        	k=re.split('\+',str(a[8]))
		if k[0]!='\n' and k:
        		cw+=int(k[1])


for a in complete:
	C=C+1

print "CPU0 ("+str(cpu1[1][1])+")\n"
print " Reads Queued : " +str(R)+"     Writes Queued : " +str(W)+"      "+str(int((rs+ws)*0.5))+"Kb\n\n"

print "CPU1 ("+str(cpu1[1][1])+")\n"
print " Reads Queued : " +str(CR)+"     Writes Queued : " +str(CW)+"      "+str(int((cr+cw)*0.5))+"Kb\n\n\n"

print "Total ("+str(cpu1[1][1])+")\n"
print " Reads Queued : " +str(CR+R)+"     Writes Queued : " +str(CW+W)+"      "+str(int((cr+cw+rs+ws)*0.5))+"Kb\n"
print " Request Completed "+str(C)+"\n\n\n"


print "Throuput (R/W) "+str(int(rs+cr)/timer)+" Kb/s "+str(int((ws+cw)/timer))+" Kb/s\n"
print "Events ("+str(cpu1[0][1])+") : "+str(lnum)


