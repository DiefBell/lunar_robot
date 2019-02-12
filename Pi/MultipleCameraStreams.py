# Web streaming example
# Source code from the official PiCamera package
# http://picamera.readthedocs.io/en/latest/recipes2.html#web-streaming

import io
import picamera
import logging
import socketserver
import threading
from http import server
import time
import _thread

PAGE = """\
<html>
<head>
<title>Lunar Rover - Live Feed</title>
</head>
<body>
<center><h1>Lunar Rover - Live Feed</h1></center>
<center><img src="stream.mjpg" width="1280" height="960"></center>
<center><h2>Test text: </h2><h3 id="time">TIME</h3></center>
</body>
</html>
"""


class StreamingOutput(object):
	def __init__(self):
		self.frame = []
		self.cam_num = 0
		self.buffer = io.BytesIO()
		self.condition = threading.Condition()

	def write(self, buf):
		if buf.startswith(b'\xff\xd8'):
			# New frame, copy the existing buffer's content and notify all
			# clients it's available
			self.buffer.truncate()
			with self.condition:
				self.frame[cam_num] = self.buffer.getvalue()
				self.condition.notify_all()
			self.buffer.seek(0)
		return self.buffer.write(buf)

	def nextCam(numCams):
		self.cam_num += 1
		if self.cam_num >= numCams:
			self.cam_num = 0
		print(self.cam_num)


class StreamingHandler(server.BaseHTTPRequestHandler):
	def do_GET(self):
		splitPath = self.path.split('.')
		if self.path == '/':
			self.send_response(301)
			self.send_header('Location', '/index.html')
			self.end_headers()
		elif self.path == '/index.html':
			content = PAGE.encode('utf-8')
			self.send_response(200)
			self.send_header('Content-Type', 'text/html')
			self.send_header('Content-Length', len(content))
			self.end_headers()
			self.wfile.write(content)
		elif self.splitPath[0] == '/stream' and self.splitPath[2] == 'mjpg' :
			print("Getting pic")
			self.send_response(200)
			self.send_header('Age', 0)
			self.send_header('Cache-Control', 'no-cache, private')
			self.send_header('Pragma', 'no-cache')
			self.send_header('Content-Type', 'multipart/x-mixed-replace; boundary=FRAME')
			self.end_headers()
			try:
				while True:
					with output.condition:
						output.condition.wait()
						frame = output.frame[int(self.splitPath[1])]
					self.wfile.write(b'--FRAME\r\n')
					self.send_header('Content-Type', 'image/jpeg')
					self.send_header('Content-Length', len(frame))
					self.end_headers()
					self.wfile.write(frame)
					self.wfile.write(b'\r\n')
			except Exception as e:
				logging.warning(
					'Removed streaming client %s: %s',
					self.client_address, str(e))
		else:
			self.send_error(404)
			self.end_headers()


class StreamingServer(socketserver.ThreadingMixIn, server.HTTPServer):
	allow_reuse_address = True
	daemon_threads = True


def AlternateCams(numCams):
	while True:
		output.nextCam(numCams)
		time.sleep(.04)


with picamera.PiCamera(resolution='1280x720', framerate=25) as camera:
	output = StreamingOutput()
	#Uncomment the next line to change your Pi's Camera rotation (in degrees)
	#camera.rotation = 90
	camera.start_recording(output, format='mjpeg')
	_thread.start_new_thread(AlternateCams, (4,) )
	try:
		address = ('', 8000)
		server = StreamingServer(address, StreamingHandler)
		server.serve_forever()
	finally:
		camera.stop_recording()
