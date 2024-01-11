#!/bin/zsh

g++ -std=c++20 -Wall -Wextra -g \
    -lGL -lGLEW -lGLU -lglfw -lassimp \
    -Ilib \
    *.cpp \
    -o scene && ./scene && rm scene
