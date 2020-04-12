# Tire

## Tiny Image Recoverer

Stupidly check for a JPG image on a raw input.
It will search for the `0xFFD8` marker to start an image
and the `0xFFD9` marker to end a image.

You can use it to find deleted images on a disk.
Just pass the disk file (something like `/dev/sdb`)
to the `stdin`, and it will save all jpg images it found.

It will probably work on Windows using the device on
`\\?\Device\`, but I didn't test.

Compile it using `-O2`, it will get realy fast.

``` C
// Usage:
// ./Tire [start] [base] < FILE

// start - position to start looking for jpg images
// base - the positional base that start is written - default is base 10
// FILE - the file containing the raw data - use /dev/sdx on linux to search on disk
```
