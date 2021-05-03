# **huffc**

Minimal compression/decompression software that's pretty much a better version of my previous project - [**huff**][huff].

It compresses files into a basic file format storing only the information needed to decompress
the file, the compressed data itself and nothing more. It means that, if you really want,
you can try do 'decompress' every file imaginable just for kicks.

## Building

To build/debug use these commands:

```bash
make                # build for debug
make release        # build with max optimizations
make run            # build and run
make clean          # clean the output directory
```

## Usage

Basic syntax:

```text
huffc [FLAGS] [SRC_FILE] [DST_FILE]
```

`DST_FILE` can be ommited when:

* Compressing any file.
  * `DST_FILE` is set to ./`SRC_FILE`'s name + .hfc
* Decompressing an hfc file.
  * `DST_FILE` is set to ./`SRC_FILE`'s name without
  the .hfc extension.
* Using the `--stdout` flag.

The list of all available flags can be seen by
using the `--help` flag or in the [config.h][config] file.

## The hfc format

As I mentioned, it's as basic as it can be:

1. First byte stores the padding information
for the header and data.
2. Next is an encoded Huffman Tree used to decompress the file.
3. The rest is just the compressed data itself.

It's almost the same as the hff format in [**huff**][huff],
but it stores less garbage; as a result both programs are currently
incompatible *(and they're going to stay that way until I muster up
the willpower to rewrite a most of huff)*.

## Return codes

For no particular reason i decided to create my own,
arbitrary set of return codes signifying different errors,
all of which can be seen in the [errdef.h][errdef] file.

## Contributing

I appreciate any bug reports/criticism coming my way,
but I doubt anyone cares anyway.

[huff]:https://github.com/kxlsx/huffman-coding-rs/tree/master/huff
[errdef]:https://github.com/kxlsx/huffc/blob/main/include/errdef.h
[config]:https://github.com/kxlsx/huffc/blob/main/include/config.h
