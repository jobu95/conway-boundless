Conway's Game of Life
=====================

Description
-----------

It's Conway's Game of Life implemented in C. It's boundless for two reasons:

1. When calculating the neighbors around a cell, the Life algorithm looks to the other side of the board if it hits an edge.

2. Much like zombo.com, the only limit is yourself.

Usage
-----
make
./conway -f stateFile

See the files in ./statefiles for samples. Living cells are designated with an 'x' character and dead cells with a '.' character.

Dependencies
------------
ncurses
