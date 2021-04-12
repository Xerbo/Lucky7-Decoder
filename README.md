# Lucky 7 Image Decoder

A small C++ program for decoding images from the Lucky-7 cubesat, included are GNURadio flowgraphs for both IQ and FM/AF recordings as well as some sample data from [Derek (@dereksgc)](https://twitter.com/dereksgc) and [FelixTRG (@OK9UWU)](https://twitter.com/ok9uwu).

## Usage

Compiled binaries for both Windows and Linux are available in the [releases](https://github.com/Xerbo/Lucky7-Decoder/releases) page.

After downloading a release you can run it with:

```
./Lucky7-Decoder input.bin output.jpg
```

Or (if you are on Windows)

```
Lucky7-Decoder.exe input.bin output.jpg
```

## Demodulation

You will need the awesome [gr-satellites](https://github.com/daniestevez/gr-satellites) installed in order to use the flowgraphs (example data available [here](https://cloud.xerbo.net/s/jGFk7WNygFoG54T), both IQ and AF).

If you're using the IQ demod:

1. Figure out the sample rate of your recording, say 93750hz
2. Change the sample rate in the flowgraph to match that
3. Change the path of the "File Source" to that of your SDR# IQ recording
4. Select a sane output path and click run

If you're using the FM/AF demod:

1. Figure out the sample rate of your recording, say 48000hz
2. Change the sample rate in the flowgraph to match that
3. Change the path of the "WAV File Source" to that of your recording
4. Select a sane output path and click run

## Example images

Some example images received by amateurs with a SDR setup and decoded with this decoder (converted to PNG to show consistently across devices)

![An image received by Derek (@dereksgc) on 13/11/2020](Sample_Data/13.11.2020-Derek.png)
![The same image but received by FelixTRG (@OK9UWU)](Sample_Data/13.11.2020-FelixTRG.png)

## Protocol

GMSK demodulation, deframing, CRC and descrambling are all handled by gr-satellites, and therefore won't be covered in this section, if you are interested in this check out [Daniel Estévez's blog post](https://destevez.net/2019/07/lucky-7-decoded/).

Each packet is laid out as following  

 - 1 byte for the OBC ID
 - 2 bytes for the OBC's counter
 - 2 bytes for the MCU's counter
 - 2 bytes for the length of the data being transmitted (in number of packets)
 - 28 bytes of actual data

Whenever an image is being transmitted the OBC's counter will start at `0xC000` and the first bytes of the JPEG will always be `0xFFD8FFDB`, however due to the nature of the downlink and how missing packets can be retransmitted it is required that the decoder can deal with repeated packets. After this you can just concatenate the data from each packet into a normal JPEG file.

## License

See `LICENSE`
