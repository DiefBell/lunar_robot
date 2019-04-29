#!/usr/bin/python3

from subprocess import Popen

wkdir = "/home/lunar_robot/Pi/"
gitdir="/home/lunar_robot/"

Popen(["sudo", "pip3", "install", "smbus2"], cwd=wkdir)
Popen(["sudo", "git", "reset", "--HARD"],  cwd=gitdir)
Popen(["sudo", "git", "pull"], cwd=gitdir)
