#ifndef _IMAGE_H_
#define _IMAGE_H_
#include "commons.h"


Image* Image_Load(const char *fileName);
void Image_Save(Image *image, const char *fileName, bool isASCII);
// functii statice implementate doar in .c care nu sunt exportate deoarece nu este nevoie de ele
// // load p2.pgm - pgm ASCII grayscale
// Image* Image_LoadP2(const char *fileName);
// // load p3.ppm - ppm ASCII RGB
// Image* Image_LoadP3(const char *fileName);
// // load p5.pgm - pgm binar grayscale
// Image* Image_LoadP5(const char *fileName);
// // load p6.ppm - binar RGB
// Image* Image_LoadP6(const char *fileName);
// void Image_SaveP2(void);
// void Image_SaveP3(void);
// void Image_SaveP5(void);
// void Image_SaveP6(void);

// copiaza portiunea delimitata de dreptunghiul [x1,y1) [x2,y2) intr-o imagine noua (selectie)
bool Image_Copy(Image *image, Image **selection, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2);
// copiaza intreaga imagine intr-o imagine noua(selectie)
bool Image_CopyAll(Image *image, Image **selection);
// functie care sterge pixelii si dupa elibereaza imaginea - nu are memory leaks
void Image_Free(Image *image);
// mai mult pentru teste
void Image_Print(Image *image);
// verifica daca este plauzibil in fileName sa se afle un PGM/PPM cerut
bool Image_IsImage(const char *fileName);
#endif /* _IMAGE_H_ */