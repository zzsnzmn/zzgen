// without mul and add.
ZZGen : UGen {
    *ar { arg freq = 440.0, iphase = 0.0;
        ^this.multiNew('audio', freq, iphase)
    }
    *kr { arg freq = 440.0, iphase = 0.0;
        ^this.multiNew('control', freq, iphase)
    }
}
