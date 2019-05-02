#!/usr/bin/python3

from subprocess import getoutput
import os

os.chdir("/home/pi/lunar_robot/Pi/")
pipDL = getoutput("sudo pip3 install smbus2")
print(pipDL)

os.chdir("/home/pi/lunar_robot/")
gitReset = getoutput("sudo git reset --HARD")
print(gitReset)
gitPull = getoutput("sudo git pull")
print(gitPull)

hostname = getoutput("hostname -I")
print(hostname)
ip = hostname.split(' ')[0].split('.')
print(ip)

addrArduino = 0x60
from smbus2 import SMBusWrapper
with SMBusWrapper(1) as bus:
	cmd = [ int(ip[0]), int(ip[1]), int(ip[2]), int(ip[3]) ]
	bus.write_i2c_block_data(addrArduino, 3, cmd)