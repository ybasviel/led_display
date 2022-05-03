#!/bin/sh

bmpfont-generator $2  ./out/char.png -c "$1 " -H 8 --no-anti-alias   --baseline 7 --margin 0 >/dev/null
convert ./out/char.png   -background white -flatten  -alpha off -compress none  ./out/char.pbm