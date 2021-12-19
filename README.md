# **chuff**

Minimal compression/decompression software that's pretty much a better version of my previous project - [**huff**][huff].

It compresses files into a basic file format storing only the information needed to decompress
the file, the compressed data itself, and nothing more.

## Building

To build/debug use:

```bash
make                # build for debug
make release        # build with release flags
make run            # build and run
make clean          # clean the output directory
```

## Usage

### Basic usage:

```text
chuff [FLAGS]... [FILES]...
```

The list of all available flags can be seen by
using the `--help` flag.

### Behaviour:

When compressing:
  * The program will attempt to compress the files into
  the current directory with the src file's name + .hfc

When decompressing:
  * The program will attempt to compress the files into
  the current directory with the .hfc extension trimmed out
  (every file without it will be skipped).

## The hfc format

As I mentioned, it's as basic as it can be:

1. First byte stores the padding information
for the header and data.
2. Next is an encoded Huffman Tree used to decompress the file.
3. The rest is just the compressed data itself.

It's almost the same as the hff format in [**huff**][huff],
but it stores less garbage; as a result both programs are currently
incompatible *(and they're going to stay that way until I muster up
the willpower to rewrite most of huff)*.

## Contributing

I appreciate any bug reports/criticism coming my way,
but I doubt anyone cares anyway.

[huff]:https://github.com/kxlsx/huffman-coding-rs/tree/master/huff
