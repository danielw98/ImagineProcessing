# BMP Image Processing Library

## Introduction
This library provides a suite of functions for basic image processing on BMP files. It is developed from scratch, inspired by the "cips2ed" book on C image processing, offering transformations, filters, cropping, and more, compiled into a shared `.so` library.

## Features
- Load and save BMP images in binary and ASCII formats.
- Apply various filters like edge detection, sharpen, blur, and Gaussian blur.
- Perform histogram calculations and equalization on grayscale images.
- Crop and rotate images to specified coordinates or angles.

## Dive Into the Code
Key files in this library include:
- `filters.c/h`: Apply predefined filters to images.
- `histogram.c/h`: Calculate and equalize image histograms.
- `image.c/h`: Load and save images, with utility functions for copying and freeing image structures.
- `helpers.c/h`: Contain helper functions for image processing such as clamping values and skipping file comments.

## Getting Started
To use this library, you must compile it using the provided `Makefile` and link the resulting shared library with your application.

## Installation
1. Clone the repository to your local machine.
2. Navigate to the cloned directory.
3. Run `make` to build the shared library.

## Usage
Link the shared library to your application and include the header `mylib.h` to access the library functions. 

## License