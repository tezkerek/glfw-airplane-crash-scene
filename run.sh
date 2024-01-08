#!/bin/zsh

g++ -std=c++20 -Wall -Wextra -g \
    -lGL -lGLEW -lGLU -lglfw \
    *.cpp \
    -o scene && ./scene && rm scene
