# Tire

## Tiny Image Recoverer

Stupidly check for JPG images on a raw file.
It will search for the `0xFFD8` marker to start an image
and the `0xFFD9` marker to end a image.

You can use it to find deleted images on a disk.
Just pass the disk file (something like `/dev/sda`)
as argument, and it will save all jpg images it found.

Normal user can't read  `/dev/sdx` directely, you must
run it as super user (run it with `sudo`).

It will probably work on Windows using the device on
`\\.\PhysicalDrive0`, but I didn't test.

Compile it using `-O2`, it will get realy fast.

``` C
// Usage:
// ./Tire FILE [start] [base]

// FILE - the file containing the raw data - use /dev/sdx on linux to search on disk
// start - position to start looking for jpg images
// base - the positional base that start is written - default is base 10
```
