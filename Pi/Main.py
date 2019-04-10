from flask import Flask, render_template, request, jsonify
from datetime import datetime
#import socket
import smbus

bus = smbus.SMBus(1)
addrArduino = 0x60

app = Flask(__name__)
#host = socket.gethostbyname(socket.gethostname())
host = '192.168.0.68'  # need to get this automagically

InputMap =\
{
    "forward" : "38",
    "back" : "40",
    "left" : "37",
    "right" : "39"
}
UserInput =\
{
    "37" : False,  # left
    "38" : False,  # fwd
    "39" : False,  # right
    "40" : False  # back
}

@app.route("/")
def index():
    t = datetime.now()
    return render_template('index.html', time=t, host=host)


@app.route("/update", methods=['POST'])
def update():
    global UserInput
    UserInput = request.form
    print(UserInput)

    if UserInput["38"] == 'true':
        bus.write_byte_data(addrArduino, 0, 5)
    elif UserInput["40"] == 'true':
        bus.write_byte_data(addrArduino, 0, -5)
    else:
        bus.write_byte_data(addrArduino, 0, 0)

    t = datetime.now()
    return jsonify({ 'result' : 'success', 'time' : t })


if __name__ == "__main__":
    print("Host: %s" % host)
    app.run(host=host, port=5000, debug=False)
