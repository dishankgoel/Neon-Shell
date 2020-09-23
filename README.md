# Neon Shell

[![MIT license](https://img.shields.io/badge/License-MIT-blue.svg)](https://lbesson.mit-license.org/) 
[![ForTheBadge built-with-love](http://ForTheBadge.com/images/badges/built-with-love.svg)](https://GitHub.com/Naereen/)

Neon Shell is an implementation of a UNIX shell like bash in C language. 
Neon shells comes from the stable shells of the noble gas Neon, also the colors too ;)

This implementation is part of the course Operating Systems Fall 2020 at IIT Gandhinagar

## Features

There are 3 main requirements that this shell satisfies:

1. Run inbuilt binaries (like ps, pmap, wget, etc.) with arguments
   
2. Implementation of commands:
   
    * ls
    * grep
    * cat
    * Mv
    * Cp (along with -r option)
    * Cd
    * Pwd
    * rm (along with -r option)
    * Chmod
    * Mkdir

3. Can run programs in background using & at the end

### ls

Long listing format is not yet implemented, so no options as of now. But multiple directories can be given as arguments

### grep

grep supports multiple files as arguments and taking input from stdin if no argument is given

The output is colored too :)

## Limitations

As of now, piping is not implemented, but you can run processes in background

Many flags for self implemented binaries are not supported (like -a, -l for ls) are not supported

Auto tab completion or cycling through previous commands using up arrow key are not implemented as of now


## Installation

Clone the repository and inside the project folder run,

```
make
```

This will compile all the required binaries, and now you can simply call the shell

```
./shell
```

Voila, you should drop to the Neon Shell!

If you want to clean the installation, simply run

```
make clean
```