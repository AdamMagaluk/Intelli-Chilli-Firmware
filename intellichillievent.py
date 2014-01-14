#!/usr/bin/python

import sys
from socket import *

cs = socket(AF_INET, SOCK_DGRAM)
cs.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)
cs.setsockopt(SOL_SOCKET, SO_BROADCAST, 1)
cs.sendto("{\"event\":"+sys.argv[1]+"}", ('255.255.255.255', 3000))
