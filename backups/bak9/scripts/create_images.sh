#!/bin/bash
# creare imagine grayscale din imaginea de test existenta
convert -strip images/test.png -colorspace Gray images/test_gray.png
# creare P2 - pgm ASCII grayscale
convert -compress None images/test_gray.png images/p2.pgm
# creare P3 - ppm ASCII RGB
convert -compress None images/test.png images/p3.ppm 
# creare P5 - pgm binar grayscale
convert images/test_gray.png images/p5.pgm 
# creare P6 - ppm binar RGB
convert images/test.png images/p6.ppm 
# creare lena.pgm
convert -compress None images/lena.png images/lena.pgm 