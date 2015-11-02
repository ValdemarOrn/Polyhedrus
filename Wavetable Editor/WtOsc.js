
window.WtOsc = (function () {

    var wtOsc = {};

    var audioCtx = new AudioContext();
    var scriptNode = audioCtx.createScriptProcessor(512, 1, 1);

    var sinewave = [];
    for (var i = 0; i < 128; i++) {
        sinewave[i] = Math.sin(i / 128 * 2 * Math.PI);
    }

    wtOsc.Fs = audioCtx.sampleRate;
    wtOsc.wave = sinewave;
    wtOsc.iterator = 0;
    wtOsc.activeNote = null;
    wtOsc.increment = 0.01;
    wtOsc.scriptNode = scriptNode; // Need to keep a reference to this, otherwise it seems that Chrome Garbage collects the items and stops running the process loop!

    scriptNode.onaudioprocess = function (e) {
        try
        {
            var iterator = wtOsc.iterator;
            var wave = wtOsc.wave;
            var increment = wtOsc.increment;
            var len = wave.length;
            var output = e.outputBuffer.getChannelData(0);
            var volume = wtOsc.activeNote === null ? 0.0 : 0.2;

            for (var n = 0; n < output.length; n++) {

                if (iterator >= 1.0)
                    iterator -= 1.0;

                output[n] = wave[Math.floor(iterator * len)] * volume;
                iterator += increment;
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
