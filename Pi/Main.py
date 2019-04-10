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
    '37' : 'false',  # left
    '38' : 'false',  # fwd
    '39' : 'false',  # right
    '40' : 'false'  # back
}

@app.route("/")
def index():
    t = datetime.now()
    return render_template('index.html', time=t, host=host)


@app.route("/update", methods=['POST'])
def update():
    global UserInput
    global data
    UserInput = request.form
    print(UserInput)

    for k in UserInput:
        print("%s = %s" % (k, UserInput[k]))
    
    #if UserInput['38'] == 'true':
        #data = [5, 1, 2, 3]
        #print("Moving forwards...")
    #if UserInput['40'] == 'true':
        #data = [6, 1, 2, 3]
        #print("Moving backwards...")
    #else:
    data = [7, 1, 2, 3]
    print("data = [0, 1, 2, 3]")
    
    with SMBusWrapper(1) as bus:
        bus.write_i2c_block_data(addrArduino, 0, data)
    
    t = datetime.now()
    return jsonify({ 'result' : 'success', 'time' : t })


if __name__ == "__main__":
    print("Host: %s" % host)
    app.run(host=host, port=5000, debug=False)
