#!/bin/zsh

g++ -std=c++20 -Wall -Wextra -g \
    *.cpp \ 
    -lGL -lGLEW -lGLU -lglfw -lassimp \
    -Ilib \
    -o scene && ./scene && rm scene
