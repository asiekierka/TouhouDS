#!/bin/sh
cd "$1"
for i in *.mp3 *.MP3 *.aac *.AAC; do begin
        fn=${i%%.*}
        ffmpeg -i "$i" "$fn".wav
        wavpack -b128 -f "$fn".wav -o "$fn".wv
        rm "$i" "$fn".wav
done
