from flask import Flask, render_template, request, jsonify
from datetime import datetime
#import socket
#from smbus2 import SMBusWrapper


addrArduino = 0x60

app = Flask(__name__)
#host = socket.gethostbyname(socket.gethostname())
host = '192.168.0.68'  # need to get this automagically

sensors = []
drill = 0

UserInput =\
{
    "FACE_1" : 0,
    "FACE_2" : 0,
    "FACE_3" : 0,
    "FACE_4" : 0,
    "LEFT_TOP_SHOULDER" : 0,
    "RIGHT_TOP_SHOULDER" : 0,
    "LEFT_BOTTOM_SHOULDER" : 0,
    "RIGHT_BOTTOM_SHOULDER" : 0,
    "SELECT_BACK" : 0,
    "START_FORWARD" : 0,
    "LEFT_STICK" : 0,
    "RIGHT_STICK" : 0,
    "DPAD_UP" : 0,
    "DPAD_DOWN" : 0,
    "DPAD_LEFT" : 0,
    "DPAD_RIGHT" : 0,
    "HOME" : 0,
    "LEFT_STICK_X" : 0,
    "LEFT_STICK_Y" : 0,
    "RIGHT_STICK_X" : 0,
    "RIGHT_STICK_Y" : 0,
    "RPM" : 1
}

@app.route("/")
def index():
    t = datetime.now()
    return render_template('index.html', time=t, host=host)


@app.route("/update", methods=['POST'])
def update():
    global UserInput
    global sensors
    global drill

    userInput = {}
    for k,v in request.form.to_dict().items():
        if k == "RPM": userInput[k] = v
        elif float(v) > 0: userInput[k] = 1
        elif float(v) < 0: userInput[k] = -1
        else: userInput[k] = 0
    if userInput != UserInput:  # i.e. something has changed
        if userInput["START_FORWARD"] == 1 and UserInput["START_FORWARD"] == 0:  # when the start button is pressed, but ignores holds
            if drill == 0: drill = 1
            else: drill = 0

        speed = userInput["RPM"]
        cmd = [0, 0, 0, 0]
        if userInput["DPAD_UP"] == 1 or userInput["LEFT_STICK_Y"] == -1: cmd = [speed, speed, 0, 0]
        elif userInput["DPAD_DOWN"] == 1 or userInput["LEFT_STICK_Y"] == 1: cmd = [-speed, -speed, 0, 0]
        elif userInput["DPAD_RIGHT"] == 1 or userInput["LEFT_STICK_X"] == 1: cmd = [speed, -speed, 0, 0]
        elif userInput["DPAD_LEFT"] == 1 or userInput["LEFT_STICK_X"] == -1: cmd = [-speed, speed, 0, 0]
    '''
        with SMBusWrapper(1) as bus:
            bus.write_i2c_block_data(addrArduino, drill, cmd)

        UserInput = userInput
    '''

    t = datetime.now()
    return jsonify({ 'result' : 'success', 'time' : t })


#if __name__ == "__main__":
print("Host: %s" % host)
app.run(host="192.168.0.50", port=5000, debug=True)
