![alt_text](./seeshell.jpg "seeshell")

# SeeShell 0.2

## Synopsis

SeeShell is a Bash-like shell replacement for Unix and Linux.

## Compilation

$ make install

## Built-in commands

cd - change directory (cd ~ (home), cd ~user, cd ~/dir, cd ~user/dir, etc ...)

cl, clear - clear screen.

help - show help.

history [-c] - show history or clear history.

pwd - print working directory.

## Features

- Support for pipes added in ver 0.2. ie: cmd1 | cmd2

- More to come.

## Files

Two global vars in main.c may be changed:

char* HISTFILE = "/shell/.shellhistory" - shell history file

const char bindir[1048] = "/shell/bin/" - if using provided coreutils-clones

## License

Copyright 2016, Matthew Wilson. 

License GPLv3+: GNU GPL version 3 or later http://gnu.org/licenses/gpl.html. 
No warranty. Software provided as is.
