var N = 1024;
var wavetableCount = 256;
var wavetable = new Array(wavetableCount);

var wt = {};

function compileScript() {
    //var text = document.getElementById('scriptTextarea').value;
    var text = wt.codeEditor.getValue();
    eval(text);
}

function recompute() {

    try {
        wt.errorEditor.setValue("");
        wavetableCount = parseInt(wavetableCountTextBox.value);
        wavetable = new Array(wavetableCount);
        N = parseInt(wavetableSizeTextBox.value);

        compileScript();
        if (wt.initialize !== null)
            wt.initialize(N, wavetableCount);

        for (var k = 0; k < wavetableCount; k++) {
            wavetable[k] = wt.makeWave(k, N, wavetableCount);
        }

        updateWave();
        scriptError.style.visibility = 'collapse';
        scriptError.style.height = '0';
    }
    catch (ex) {
        scriptError.style.visibility = 'visible';
        scriptError.style.height = '200px';
        wt.errorEditor.setValue(ex.message + '\n' + ex.stack);
    }
}

function updateWave() {
    var slider = document.getElementById('waveSlider').value;
    var k = Math.round(slider * (wavetableCount - 1));
    var selectedWave = wavetable[k];
    window.WtOsc.wave = selectedWave;
    paintWave(selectedWave);
}

function paintWave(wave) {
    var canvas = document.getElementById('canvas');
    var width = canvas.width;
    var height = canvas.height;
    var ctx = canvas.getContext('2d');

    ctx.clearRect(0, 0, width, height);

    ctx.beginPath();
    ctx.moveTo(0, 0.1 * height);
    ctx.lineTo(width, 0.1 * height);
    ctx.lineWidth = 1;
    ctx.strokeStyle = '#999';
    ctx.stroke();

    ctx.beginPath();
    ctx.moveTo(0, 0.9 * height);
    ctx.lineTo(width, 0.9 * height);
    ctx.lineWidth = 1;
    ctx.strokeStyle = '#999';
    ctx.stroke();

    ctx.beginPath();
    for (var i = 0; i < wave.length; i++) {
        var x = i / wave.length * width;
        var y = -wave[i] * height / 2 * 0.8 + height / 2;

        if (i == 0)
            ctx.moveTo(x, y);
        else
            ctx.lineTo(x, y);
    }

    ctx.lineWidth = 1.5;
    ctx.strokeStyle = "#555";
    ctx.stroke();

}

function downloadWave() {
    var vol = 0.8;
    var wav = new Wav();
    var buffer = [];

    for (var k = 0; k < wavetable.length; k++) {
        var wave = wavetable[k];
        for (var n = 0; n < wave.length; n++) {
            buffer.push(wave[n] * vol);
        }
    }

    wav.setBuffer(buffer);
    var b = wav.getBlob();
    var url = window.URL.createObjectURL(b);
    document.getElementById('downloadLink').href = url;
}

window.addEventListener('load', function () {

    wt.codeEditor = CodeMirror.fromTextArea(scriptTextarea, { lineNumbers: true, mode: "javascript", indentUnit: 2, smartIndent: false, tabSize: 2 });
    wt.errorEditor = CodeMirror.fromTextArea(scriptErrorTextarea, { lineNumbers: true, mode: "javascript", indentUnit: 2, smartIndent: false, tabSize: 2, readOnly: true });
    wt.codeEditor.setSize("100%", "100%");
    wt.errorEditor.setSize("100%", "100%");
    recompute();

    var noteLinks = document.getElementsByClassName('noteLink');
    for (var i = 0; i < noteLinks.length; i++) {
        var link = noteLinks[i];

        link.addEventListener('mousedown', function (ev) {

            if (ev.currentTarget.innerHTML == 'Off') {
                window.WtOsc.activeNote = null;
                return;
            }

            var octave = ev.currentTarget.innerHTML.substr(1, 1);
            var note = 60 + (octave - 3) * 12;
            var hz = 440 * Math.pow(2, (note - 69) / 12);
            var samplesPerCycle = window.WtOsc.Fs / hz;
            var increment = 1.0 / samplesPerCycle;
            window.WtOsc.activeNote = note;
            window.WtOsc.increment = increment;
        });

        /*link.addEventListener('mouseup', function () {
            if (ev.currentTarget.innerHTML == 'Off')
                window.WtOsc.activeNote = null;
        });*/
    }

});
