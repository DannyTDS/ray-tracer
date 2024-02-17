#!/bin/zsh

for file in *.ppm; do
    if [ -f "$file" ] && [ ! -f "${file%.ppm}.png" ]; then
        # Convert .ppm to .png with the same file name
        magick "$file" "${file%.ppm}.png"
    fi
done

