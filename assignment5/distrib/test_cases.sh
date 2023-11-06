#!/bin/sh
rm *.tga
rm *.bmp

./a5 -input scene06_bunny_1k.txt  -size 300 300 -output 6.bmp\
   -shadows -bounces 4 -jitter -filter -depth 5 20 6_depth.bmp -normal 6_normal.bmp
./a5 -input scene06_bunny_1k.txt  -size 300 300 -output 6_no_compression.bmp\
   -shadows -bounces 4 -jitter
./a5 -input scene10_sphere.txt  -size 300 300 -output 10.bmp\
   -shadows -bounces 4 -jitter -filter
./a5 -input scene11_cube.txt  -size 300 300 -output 11.bmp\
 -shadows -bounces 4 -jitter -filter
./a5 -input scene12_vase.txt  -size 300 300 -output 12.bmp\
 -shadows -bounces 4 -jitter -filter
./a5 -input scene11_fisheye.txt -size 200 200 -output 11_fisheye.bmp\
 -shadows -bounces 4 -jitter

