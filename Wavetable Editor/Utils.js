Utils = {};

Utils.normalize = function(data) {

    var max = 0.0;

    for (var i = 0; i < data.length; i++) {
        if (Math.abs(data[i]) > max)
            max = Math.abs(data[i]);
    }

    for (var i = 0; i < data.length; i++) {
        data[i] = data[i] / max;
    }
}

Utils.gain = function (data, gain) {

    for (var i = 0; i < data.length; i++) {
        data[i] = data[i] * gain;
    }
}

Utils.tanh = function (data) {

    for (var i = 0; i < data.length; i++) {
        data[i] = Math.tanh(data[i]);
    }
}