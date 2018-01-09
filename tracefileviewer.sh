#!/bin/sh
hexdump -n 4 -s 0 -e ' 1/4 "Header length: %6d"  "\n"' $1
hexdump -n 4 -s 4 -e ' 1/4 "Restarts:  %10d"  "\n"' $1
hexdump -s 10 -e '"%07.7_ad  " 1/1 "%_p "  1/4 "%10d"  "\n"' $1
