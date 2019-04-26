var updateInterval = 100;

var keys = {
    37 : false,
    38 : false,
    39 : false,
    40: false
};

var userInput = {
    MODE : 0,
    FWD: 0,
    TURN: 0,

    FL: 0,
    FR: 0,
    BL: 0,
    BR: 0,

    RPM: 0
};

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
                if(gamepad.state["LEFT_STICK_Y"] < 0 || keys[38]) userInput["FWD"] = 1;
                else if(gamepad.state["LEFT_STICK_Y"] > 0 || keys[40]) userInput["FWD"] = -1;
                else userInput["FWD"] = 0;

                if(gamepad.state["LEFT_STICK_X"] > 0 || keys[39]) userInput["TURN"] = 1;
                else if(gamepad.state["LEFT_STICK_X"] < 0 || keys[37]) userInput["TURN"] = 1;
                else userInput["FWD"] = 0;

                if(gamepad.state["START_FORWARD"]) userInput["MODE"] = 1;
                else userInput["mode"] = 0;
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
    userInput["FL"] = $('#fl_target').val();
    userInput["FR"] = $('#fr_target').val();
    userInput["BL"] = $('#bl_target').val();
    userInput["BR"] = $('#br_target').val();
    $.ajax({
        url: "/update",
        type: "post",
        data: userInput,
        success: function(data){
            $('#time').html(data.time);
            $('#fl_val').html(data.fl);
            $('#fr_val').html(data.fr);
            $('#bl_val').html(data.bl);
            $('#br_val').html(data.br);
        },
        complete: function() {
            setTimeout(update, updateInterval);
        }
    });
}

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