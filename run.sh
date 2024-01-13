#!/bin/zsh

g++ -std=c++20 -Wall -Wextra -g \
    *.cpp \
    -lglfw -lGL -lGLEW -lGLU -lassimp \
    -Ilib \
    -o scene && ./scene && rm scene
