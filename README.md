# Lucky 7 Image Decoder

A small C++ program for decoding images from the Lucky-7 cubesat, included are GNURadio flowgraphs for both IQ and FM/AF recordings as well as some sample data from [Derek (OK9SGC)](https://twitter.com/ok9sgc) and 

## Usage

Since the project currently consists of only a single source file with no dependencies apart from the standard library, compiling is done manually:

```
g++ -Wall -O2 -o Lucky7-Decoder main.cpp
```

Then to actually run it:

```
./Lucky7-Decoder input.bin output.jpg
```

## Demodulation

You will need the awesome [gr-satellites](https://github.com/daniestevez/gr-satellites) installed in order to use the flowgraphs.

If you're using the IQ demod:

1. Figure out the sample rate of your recording, say 96000hz
2. Chance the sample rate in the flowgraph to match that
3. Change the path on the WAV file source to that of your SDR# IQ recording (input can be changed to F32 rather easily)
4. Select a sane output path and click run

If you're using the FM/AF demod:

1. Figure out the sample rate of your recording, say 48000hz
2. Chance the sample rate in the flowgraph to match that
3. Change the path on the WAV file source to that of your recording
4. Select a sane output path and click run

## Protocol

TBD

## License

See `LICENSE`
