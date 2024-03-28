#ifndef _COMMONS_H_
#define _COMMONS_H_
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#define STR_SIZE 256
#define NR_FILTRE 4
typedef enum Commands {
    CMD_LOAD,
    CMD_SELECT_ALL,
    CMD_SELECT,
    CMD_HISTOGRAM,
    CMD_EQUALIZE,
    CMD_ROTATE,
    CMD_CROP,
    CMD_APPLY,
    CMD_SAVE,
    CMD_EXIT,
    CMD_NUM_COMMANDS,
    CMD_WRONG_COMMAND
}Commands;

typedef enum ImageType{
    IMG_PGM_BIN = 5,
    IMG_PGM = 2,
    IMG_PPM = 3,
    IMG_PPM_BIN = 6
}ImageType;

typedef struct PixelRGB {
    uint8_t r;
    uint8_t g;
    uint8_t b;
}PixelRGB_t;

typedef uint8_t PixelGrayScale_t;

typedef struct Image {
    uint16_t magicBytes;
    uint32_t width;
    uint32_t height;
    uint16_t maxVal;
    ImageType type;
    void *pixels;
}Image;

union MagicBytes{
    char str[3];
    short sh;
};

#endif /* _COMMONS_H_ */