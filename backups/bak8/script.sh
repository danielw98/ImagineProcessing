#!/bin/bash
# stergere fisiere vechi - ignoram erorile
rm *.pgm *.ppm test_gray.png 2> /dev/null
# creez imagine grayscale din imaginea de test existenta
convert test.png -colorspace Gray test_gray.png
# creez P2 - pgm ASCII grayscale
convert -compress None test_gray.png p2.pgm
# creez P3 - ppm ASCII RGB
convert -compress None test.png p3.ppm
# creez P5 - pgm binar grayscale
convert test_gray.png p5.pgm
# creez P6 - ppm binar RGB
convert test.png p6.ppm