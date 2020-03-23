#-*- conding:utf-8 -*-
import urllib
import sys
import os
import time
import datetime

if len(sys.argv) != 3:
    print "bad param"
    sys.exit()
qps = int(sys.argv[1])
filename = sys.argv[2]

duration = 1.0 / qps
start = time.time()
count = 0

while True:
    f = open(filename)
    for line in f:
        line = line.strip()
        count += 1
        command = "wget -q \"%s\" -O /dev/null &" % (line)
        print str(datetime.datetime.now()) + ' ' + command
        os.system(command)

        expect = start + duration * count
        cur = time.time()
        if cur > expect:
            sleep = 0
        else:
            sleep = expect - cur
        print sleep
        time.sleep(sleep)