#include "image.h"

// load p2.pgm - pgm ASCII grayscale
static Image* Image_LoadP2(const char *fileName);
// load p3.ppm - ppm ASCII RGB
static Image* Image_LoadP3(const char *fileName);
// load p5.pgm - pgm binar grayscale
static Image* Image_LoadP5(const char *fileName);
// load p6.ppm - binar RGB
static Image* Image_LoadP6(const char *fileName);

// functie de load generica - vede tipul imaginii in functie de magic bytes - singura expusa in header
Image* Image_Load(const char *fileName)
{
    union MagicBytes magicBytes = readMagicBytes(fileName);
    Image *image = NULL;
    // endianess - verificare - poate fi mai riguroasa
    // verific daca este P2 sau 2P - little endian vs big endian
    if(magicBytes.sh == 0x5032 || magicBytes.sh == 0x3250)
    {
        // este P2
        image = Image_LoadP2(fileName);
    }
    if(magicBytes.sh  == 0x5033 || magicBytes.sh  == 0x3350)
    {
        // este P3
        image = Image_LoadP3(fileName);
    }
    if(magicBytes.sh  == 0x5035 || magicBytes.sh  == 0x3550)
    {
        // este P5
        image = Image_LoadP5(fileName);
    }
    if(magicBytes.sh  == 0x5036 || magicBytes.sh  == 0x3650)
    {
        // este P6
        image = Image_LoadP6(fileName);
    }
    return image;
}

// load p2.pgm - pgm ASCII grayscale
static Image* Image_LoadP2(const char *fileName)
{
    // deschiderea fisierului
    FILE *fp = fopen(fileName, "r"); // vedem daca trebuie rb in functie de magic bytes
    if(fp == NULL)
        return NULL;
    // il vom citi linie cu linie
    char line[STR_SIZE];

    // alocarea imaginii
    Image *image = (Image*) malloc(sizeof(Image));
    image->width = image->height = 0;
    image->type = IMG_PGM;
    image->maxVal = 255;

    // 1. stim magic number-ul ca functia a fost aplelata pe baza lui
    skip_magic_bytes(fp, line);

    // 2. skip whitespace & commentarii
    skip_comments(fp);
    skip_whitespace(fp);

    // 3. citire width - ASCII decimal - oricum fscanf sare whitespace-ul
    citire_ascii_decimal(fp, &image->width);

    // 4. Whitespace
    skip_whitespace(fp);
    skip_comments(fp);

    // 5. citire height - ASCII decimal
    citire_ascii_decimal(fp, &image->height);

    // 6. Whitespace
    skip_whitespace(fp);

    // 7. The maximum color value (Maxval), again in ASCII decimal. Must be less than 65536 and more than zero.
    uint32_t maxVal;
    citire_ascii_decimal(fp, &maxVal);
    
    if(maxVal < 0 || maxVal > UINT16_MAX)
    {
        printf("avem o buba\n");
        free(image);
        return NULL;
    }
    else
        image->maxVal = maxVal;

    // verificare
    printf("width = %d\theight = %d\tmaxVal = %d\n", image->width, image->height, image->maxVal);
    
    // 8. A single whitespace character (usually a newline).
    skip_whitespace(fp);

    // alocare dinamica matrice de pixeli grayscale
    PixelGrayScale_t **pixels;
    pixels = (PixelGrayScale_t**) malloc(image->height * sizeof(PixelGrayScale_t*));
    for(int i = 0; i < image->height; i++)
        pixels[i] = (PixelGrayScale_t*) malloc(image->width * sizeof(PixelGrayScale_t));
    
    // 9. citire pixeli grayscale din fisier text
    // A raster of Height rows, in order from top to bottom. Each row consists of Width pixels, in order from left to right.
    for(int i = 0; i < image->height; i++)
    {
        for(int j = 0; j < image->width; j++)
        {
            fscanf(fp, "%hhu", &pixels[i][j]);
            printf("%3hhu ", pixels[i][j]);
        }
        printf("\n");
    }
    // salvarea pixelilor in structura        
    image->pixels = pixels;

    // eliberarea de resurse
    fclose(fp);

    return image;
}

// load p3.ppm - ppm ASCII RGB
static Image* Image_LoadP3(const char *fileName)
{
    // deschiderea fisierului
    FILE *fp = fopen(fileName, "r"); // vedem daca trebuie rb in functie de magic bytes
  
    // il vom citi linie cu linie
    char line[STR_SIZE];

    // alocarea imaginii
    Image *image = (Image*) malloc(sizeof(Image));
    image->width = image->height = 0;
    image->type = IMG_PPM;
    image->maxVal = 255;

    // 1. stim magic number-ul ca functia a fost aplelata pe baza lui
    skip_magic_bytes(fp, line);

    // 2. skip whitespace & commentarii
    skip_comments(fp);
    skip_whitespace(fp);

    // 3. citire width - ASCII decimal - oricum fscanf sare whitespace-ul
    citire_ascii_decimal(fp, &image->width);

    // 4. Whitespace
    skip_whitespace(fp);
    skip_comments(fp);

    // 5. citire height - ASCII decimal
    citire_ascii_decimal(fp, &image->height);

    // 6. Whitespace
    skip_whitespace(fp);

    // 7. The maximum color value (Maxval), again in ASCII decimal. Must be less than 65536 and more than zero.
    uint32_t maxVal;
    citire_ascii_decimal(fp, &maxVal);
    if(maxVal < 0 || maxVal > UINT16_MAX)
    {
        printf("avem o buba\n");
        free(image);
        return NULL;
    }
    else
        image->maxVal = maxVal;

    // verificare
    printf("width = %d\theight = %d\tmaxVal = %d\n", image->width, image->height, image->maxVal);

    // 8. A single whitespace character (usually a newline).
    skip_whitespace(fp);

    // alocare dinamica matrice de pixeli RGB
    PixelRGB_t **pixels;
    pixels = (PixelRGB_t**) malloc(image->height * sizeof(PixelRGB_t*));
    for(int i = 0; i < image->height; i++)
        pixels[i] = (PixelRGB_t*) malloc(image->width * sizeof(PixelRGB_t));
    
    // citire pixeli RGB din fisier text
    for(int i = 0; i < image->height; i++)
    {
        for(int j = 0; j < image->width; j++)
        {
            fscanf(fp, "%hhu %hhu %hhu", &pixels[i][j].r, &pixels[i][j].g, &pixels[i][j].b);
            printf("%3hhu %3hhu %3hhu ", pixels[i][j].r, pixels[i][j].g, pixels[i][j].b);
        }
        printf("\n");
    }
    // salvarea pixelilor in structura
    image->pixels = pixels;
    
    // eliberarea de resurse
    fclose(fp);

    return image;
}

// load p5.pgm - pgm binar grayscale
static Image* Image_LoadP5(const char *fileName)
{
    // deschiderea fisierului
    FILE *fp = fopen(fileName, "rb"); // vedem daca trebuie rb in functie de magic bytes

    // il vom citi linie cu linie
    char line[STR_SIZE];
    
    // 1. stim magic number-ul ca functia a fost aplelata pe baza lui
    // skip magic bytes
    skip_magic_bytes_binary(fp, line);

    // 2. skip whitespace
    skip_whitespace_binary(fp);
    // alocare imagine
    Image *image = (Image*) malloc(sizeof(Image));
    image->width = image->height = 0;
    image->type = IMG_PGM_BIN;
    image->maxVal = 255;
    
    // 3. citire width - ASCII decimal
    citire_ascii_decimal_binary(fp, &image->width);

    // 4. Whitespace
    skip_whitespace_binary(fp);

    // 5. citire height - ASCII decimal
    citire_ascii_decimal_binary(fp, &image->height);

    printf("width = %u\theight = %u\n", image->width, image->height);

    // 6. Whitespace
    skip_whitespace_binary(fp);

    // 7. citire valoare maxima - ASCII decimal - {0, 1, ..., 65535} 
    uint32_t maxVal;
    citire_ascii_decimal_binary(fp, &maxVal);
    if(maxVal < 0 || maxVal > UINT16_MAX)
        printf("avem o buba\n");
    else
        image->maxVal = maxVal;
    
    // 8. Whitespace
    skip_whitespace_binary(fp);

    // alocare dinamica matrice de pixeli grayscale
    PixelGrayScale_t **pixels;
    pixels = (PixelGrayScale_t**) malloc(image->height * sizeof(PixelGrayScale_t*));
    for(int i = 0; i < image->height; i++)
        pixels[i] = (PixelGrayScale_t*) malloc(image->width * sizeof(PixelGrayScale_t));

    // citire pixeli grayscale din fisier binar
    for(int i = 0; i < image->height; i++)
    {
        for(int j = 0; j < image->width; j++)
        {
            fread(&pixels[i][j], sizeof(PixelGrayScale_t), 1, fp);
            skip_whitespace_binary(fp);
            printf("%2hhu ", pixels[i][j]);
        }
        printf("\n");
    }
       
    // salvarea pixelilor in structura        
    image->pixels = pixels;
    // printf("width = %d\theight = %d\n", image->width, image->height);

    // eliberarea de resurse
    fclose(fp);

    return image;
}

// load p6.ppm - binar RGB
static Image* Image_LoadP6(const char *fileName)
{
    // deschiderea fisierului
    FILE *fp = fopen(fileName, "rb"); // vedem daca trebuie rb in functie de magic bytes

    // il vom citi linie cu linie
    char line[STR_SIZE];
    
    // 1. stim magic number-ul ca functia a fost aplelata pe baza lui
    // skip magic bytes
    skip_magic_bytes_binary(fp, line);

    // 2. skip whitespace
    skip_whitespace_binary(fp);
    // alocare imagine
    Image *image = (Image*) malloc(sizeof(Image));
    image->width = image->height = 0;
    image->type = IMG_PPM_BIN;
    image->maxVal = 255;
    
    // 3. citire width - ASCII decimal
    citire_ascii_decimal_binary(fp, &image->width);

    // 4. Whitespace
    skip_whitespace_binary(fp);

    // 5. citire height - ASCII decimal
    citire_ascii_decimal_binary(fp, &image->height);

    printf("width = %u\theight = %u\n", image->width, image->height);

    // 6. Whitespace
    skip_whitespace_binary(fp);

    // 7. citire valoare maxima - ASCII decimal - {0, 1, ..., 65535} 
    uint32_t maxVal;
    citire_ascii_decimal_binary(fp, &maxVal);
    if(maxVal < 0 || maxVal > UINT16_MAX)
        printf("avem o buba\n");
    else
        image->maxVal = maxVal;
    
    // 8. Whitespace
    skip_whitespace_binary(fp);

    // alocare dinamica matrice de pixeli RGB
    PixelRGB_t **pixels;
    pixels = (PixelRGB_t**) malloc(image->height * sizeof(PixelRGB_t*));
    for(int i = 0; i < image->height; i++)
        pixels[i] = (PixelRGB_t*) malloc(image->width * sizeof(PixelRGB_t));
    
    // citire pixeli RGB din fisier binar
    for(int i = 0; i < image->height; i++)
    {
        for(int j = 0; j < image->width; j++)
        {
            fread(&pixels[i][j].r, sizeof(PixelGrayScale_t), 1, fp);
            skip_whitespace_binary(fp);
            fread(&pixels[i][j].g, sizeof(PixelGrayScale_t), 1, fp);
            skip_whitespace_binary(fp);
            fread(&pixels[i][j].b, sizeof(PixelGrayScale_t), 1, fp);
            skip_whitespace_binary(fp);
            printf("%3hhu %3hhu %3hhu ", pixels[i][j].r, pixels[i][j].g, pixels[i][j].b);
        }
        printf("\n");
    }
    // salvarea pixelilor in structura
    image->pixels = pixels;
    // printf("width = %d\theight = %d\n", image->width, image->height);
    
    // eliberarea de resurse
    fclose(fp);

    return image;
}



// functie care sterge pixelii si dupa elibereaza imaginea - nu are memory leaks
void Image_Free(Image *image)
{
    PixelGrayScale_t **ptrPixGray;
    PixelRGB_t **ptrPixRGB;
    for(int i = 0; i < image->height; i++)
    {
        if(image->type == IMG_PGM || image->type == IMG_PGM_BIN)
        {
            ptrPixGray = image->pixels;
            free(ptrPixGray[i]);
        }
        if(image->type == IMG_PPM || image->type == IMG_PPM_BIN)
        {
            ptrPixRGB = image->pixels;
            free(ptrPixRGB[i]);
        }
    }
    
    free(image);
}