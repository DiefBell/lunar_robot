import time
import threading
import os

def startprgm(i):
    print("Running thread %d" % i)
    if (i == 0):
        time.sleep(1)
        print('Running: VideoStream.py')
        os.system("sudo python3 /home/pi/Desktop/Lunar_Robot/Pi/VideoStream.py")
    elif (i == 1):
        print('Running: Main.py')
        time.sleep(1)
        os.system("sudo python3 /home/pi/Desktop/Lunar_Robot/Pi/Main.py")
    else:
        pass

for i in range(2):
    t = threading.Thread(target=startprgm, args=(i,))
    t.start()
    time.sleep(2)

while True:
    time.sleep(1)
