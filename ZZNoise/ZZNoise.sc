// without mul and add.
ZZNoise : UGen {
    *ar { arg freq = 440.0, iphase = 0.0;
        ^this.multiNew('audio', freq, iphase)
    }
}
