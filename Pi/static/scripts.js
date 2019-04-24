var updateInterval = 100;

var userInput = {
    FACE_1: 0,
    FACE_2: 0,
    FACE_3: 0,
    FACE_4: 0,
    LEFT_TOP_SHOULDER: 0,
    RIGHT_TOP_SHOULDER: 0,
    LEFT_BOTTOM_SHOULDER: 0,
    RIGHT_BOTTOM_SHOULDER: 0,
    SELECT_BACK: 0,
    START_FORWARD: 0,
    LEFT_STICK: 0,
    RIGHT_STICK: 0,
    DPAD_UP: 0,
    DPAD_DOWN: 0,
    DPAD_LEFT: 0,
    DPAD_RIGHT: 0,
    HOME: 0,
    LEFT_STICK_X: 0,
    LEFT_STICK_Y: 0,
    RIGHT_STICK_X: 0,
    RIGHT_STICK_Y: 0,
    RPM: 1
}

$(document).ready(function(){
    window.gamepad = new Gamepad();
    window.gamepad.bind(Gamepad.Event.CONNECTED, function(device){
        console.log('Connected', device);
    });
    window.gamepad.bind(Gamepad.Event.DISCONNECTED, function(device){
        console.log('Disconnected', device);
    });
    window.gamepad.bind(Gamepad.Event.TICK, function(gamepads){
        var gamepad,
			wrap,
			control,
			value,
			i,
			j;

        for(i = 0; i < gamepads.length; i++)
        {
            gamepad = gamepads[i];
            if(gamepad)
            {
                for (control in gamepad.state)
                {
                    userInput[control] = gamepad.state[control];
                    //console.log(gamepad.state[control]);
                }
            }
        }
    });
    if(!window.gamepad.init() )
    {
        console.log("Gamepads not supported by current browser...");
    }

    setTimeout(update, updateInterval)
});

function update(){
    userInput["RPM"] = $('#rpm').val();
    $.ajax({
        url: "/update",
        type: "post",
        data: userInput,
        success: function(data){
            $("#time").html(data.time);
        },
        complete: function() {
            setTimeout(update, updateInterval);
        }
    });
}

/*
$(document).ready(function() {
    console.log("READY  BOIS");
    setTimeout(update, 200);
});

var keys = {};

$(document).keydown(function (e) {
    keys[e.which] = true;
    printKeys();
});

$(document).keyup(function (e) {
    delete keys[e.which];
    printKeys();
});

function printKeys() {
    var html = "";
    for (var i in keys) {
        if (!keys.hasOwnProperty(i)) continue;
        html += "<p>" + i + "</p>";
    }
    $("#keylog").html(html);
}

function update() {
    $.ajax({
        url : "/update",
        type : "post",
        data : keys,
        success: function(data) {
            $("#time").html(data.time);
        },
        complete: function() {
            setTimeout(update, 200);
        }
});
}*/