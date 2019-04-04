$(document).ready(function() {
    console.log("READY  BOIS");
    setTimeout(update, 1000);
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
            setTimeout(update, 1000);
        }
});
}