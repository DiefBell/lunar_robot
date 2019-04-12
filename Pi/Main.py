from flask import Flask, render_template, request, jsonify
from datetime import datetime
#import socket
from smbus2 import SMBusWrapper 


addrArduino = 0x60

app = Flask(__name__)
#host = socket.gethostbyname(socket.gethostname())
host = '192.168.0.68'  # need to get this automagically

data = []

InputMap =\
{
    "forward" : "38",
    "back" : "40",
    "left" : "37",
    "right" : "39"
}
UserInput =\
{
}

@app.route("/")
def index():
    t = datetime.now()
    return render_template('index.html', time=t, host=host)


@app.route("/update", methods=['POST'])
def update():
    global UserInput
    global data
    UserInput = request.form.to_dict()
    print(UserInput)

    userInput = UserInput.keys()
    
    if InputMap["forward"] in userInput:
        data = [15, 1, 2, 3]
    elif InputMap["back"] in userInput:
        data = [-15, 1, 2, 3]
    else:
        data = [0, 1, 2, 3]
    
    with SMBusWrapper(1) as bus:
        bus.write_i2c_block_data(addrArduino, 0, data)
    
    t = datetime.now()
    return jsonify({ 'result' : 'success', 'time' : t })


#if __name__ == "__main__":
print("Host: %s" % host)
app.run(host=host, port=5000, debug=False)
