
window.WtOsc = (function () {

    var wtOsc = {};

    var audioCtx = new AudioContext();
    var scriptNode = audioCtx.createScriptProcessor(512, 1, 1);

    var size = 128;
    var sinewave = [];
    for (var i = 0; i < 128; i++) {
        sinewave[i] = Math.sin(i / 128 * 2 * Math.PI);
    }

    wtOsc.wave = sinewave;
    wtOsc.iterator = 0;
    wtOsc.scriptNode = scriptNode; // Need to keep a reference to this, otherwise it seems that Chrome Garbage collects the items and stops running the process loop!

    scriptNode.onaudioprocess = function (e) {

        try
        {
            var iterator = wtOsc.iterator;
            var wave = wtOsc.wave;
            var len = wave.length;
            var output = e.outputBuffer.getChannelData(0);

            for (var n = 0; n < output.length; n++) {

                if (iterator >= len)
                    iterator = 0;

                output[n] = wave[iterator] * 0.8;
                iterator += 4;
            }

            wtOsc.iterator = iterator;
        }
        catch (ex)
        {
            console.log(ex.message);
        }
    }

    scriptNode.connect(audioCtx.destination);

    return wtOsc;
})();
