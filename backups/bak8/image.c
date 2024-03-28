#include "image.h"
#include "helpers.h"
// load p2.pgm - pgm ASCII grayscale
static Image* Image_LoadP2(const char *fileName);
// load p3.ppm - ppm ASCII RGB
static Image* Image_LoadP3(const char *fileName);
// load p5.pgm - pgm binar grayscale
static Image* Image_LoadP5(const char *fileName);
// load p6.ppm - binar RGB
static Image* Image_LoadP6(const char *fileName);

// save p2.pgm - pgm ASCII grayscale
static void Image_SaveP2(Image *image, const char *fileName);
// save p3.ppm - ppm ASCII RGB
static void Image_SaveP3(Image *image, const char *fileName);
// save p5.pgm - pgm binar grayscale
static void Image_SaveP5(Image *image, const char *fileName);
// save p6.ppm - binar RGB
static void Image_SaveP6(Image *image, const char *fileName);


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


void Image_Free(Image *image)
{
    // daca am pasat un pointer NULL nu am ce elibera
    if(image == NULL){
        return;
    }
    PixelGrayScale_t **ptrPixGray;
    PixelRGB_t **ptrPixRGB;
    // eliberez fiecare rand de pixeli
    for(uint32_t i = 0; i < image->height; i++)
    {
        if(image->type == IMG_PGM || image->type == IMG_PGM_BIN)
        {
            ptrPixGray = (PixelGrayScale_t**) image->pixels;
            free(ptrPixGray[i]);
        }
        if(image->type == IMG_PPM || image->type == IMG_PPM_BIN)
        {
            ptrPixRGB = (PixelRGB_t**) image->pixels;
            free(ptrPixRGB[i]);
        }
    }
    // eliberez pixelii
    free(image->pixels);
    // eliberez imaginea
    free(image);
}

// afisarea imaginii catre utilizator in terminal - mai mult pt debugging
void Image_Print(Image *image)
{
    for(uint32_t i = 0; i < image->height; i++)
    {
        for(uint32_t j = 0; j < image->width; j++)
        {
            if(image->type == IMG_PGM || image->type == IMG_PGM_BIN)
            {
                PixelGrayScale_t pixel = ((PixelGrayScale_t**)image->pixels)[i][j];
                printf("%hhu ", pixel);
            }
            if(image->type == IMG_PPM || image->type == IMG_PPM_BIN)
            {
                PixelRGB_t pixel = ((PixelRGB_t**)image->pixels)[i][j];
                printf("%hhu %hhu %hhu ", pixel.r, pixel.g, pixel.b);
            }
        }
        printf("\n");
    }
}

bool Image_Copy(Image *image, Image **selection, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2)
{
    // aici verific ca am colturile bune.
    if(x1 > x2)
    {
        uint32_t tmp;
        tmp = x1; x1 = x2; x2 = tmp;
        tmp = y1; y1 = y2; y2 = tmp;
    }
    if(x1 == x2 && y1 == y2)
    {
        printf("Wrong input (%u %u %u %u)\n", x1, y1, x2, y2);
        return false;
    }
    // eliberez imaginea anterioara daca exista
    if(*selection != NULL)
    {
        Image_Free(*selection);
        *selection = NULL;
    }
    // printf("IMAGE COPY(%u %u %u %u)\n", x1, y1, x2, y2);
    // initializez imaginea cu datele cunoscute
    *selection = (Image*) malloc(sizeof(Image));
    selection[0]->height = y2 - y1; // v[i] = *(v+i)
    selection[0]->width  = x2 - x1;
    selection[0]->maxVal = image->maxVal;
    selection[0]->magicBytes = image->magicBytes;
    selection[0]->type = image->type;

    // printf("Image_Copy: w: %u\th: %u\n", selection[0]->width, selection[0]->height);

    PixelRGB_t **pixelRGB = NULL;
    PixelGrayScale_t **pixelGray = NULL;
    if(image->type == IMG_PGM || image->type == IMG_PGM_BIN)
        pixelGray = (PixelGrayScale_t**) malloc(selection[0]->height*sizeof(PixelGrayScale_t*));
    if(image->type == IMG_PPM || image->type == IMG_PPM_BIN)
        pixelRGB = (PixelRGB_t**) malloc(selection[0]->height*sizeof(PixelRGB_t*));
    
    for(uint32_t i1 = y1, i2 = 0; i1 < y2; i1++, i2++)
    {
        if(image->type == IMG_PGM || image->type == IMG_PGM_BIN)
            pixelGray[i2] = (PixelGrayScale_t*) malloc(selection[0]->width*sizeof(PixelGrayScale_t));
        if(image->type == IMG_PPM || image->type == IMG_PPM_BIN)
            pixelRGB[i2]  = (PixelRGB_t*) malloc(selection[0]->width*sizeof(PixelRGB_t));
        
        for(uint32_t j1 = x1, j2 = 0; j1 < x2; j1++, j2++)
        {
            if(image->type == IMG_PGM || image->type == IMG_PGM_BIN)
                pixelGray[i2][j2] = ((PixelGrayScale_t**) image->pixels)[i1][j1];
            if(image->type == IMG_PPM || image->type == IMG_PPM_BIN)
            {
                pixelRGB[i2][j2].r  = ((PixelRGB_t**) image->pixels)[i1][j1].r;
                pixelRGB[i2][j2].g  = ((PixelRGB_t**) image->pixels)[i1][j1].g;
                pixelRGB[i2][j2].b  = ((PixelRGB_t**) image->pixels)[i1][j1].b;
            }
        }
    }
    // if(pixelRGB == NULL && pixelGray == NULL)
    //     printf("\n\nPROBLEMA %d\n\n", __LINE__);
    selection[0]->pixels = (pixelRGB == NULL) ? (void*)pixelGray : (void*)pixelRGB;

    // printf("la %p am selectat imaginea cu w: %u\th: %u\n", selection[0], selection[0]->width, selection[0]->height);
    // Image_Print(selection[0]);
    return true;
}

bool Image_CopyAll(Image *image, Image **selection)
{
    return Image_Copy(image, selection, 0, 0, image->width, image->height); 
}
// salveaza o imagine oarecare in fisierul dat ca parametru
void Image_Save(Image *image, const char *fileName, bool isASCII)
{
    if(isASCII == true)
    {
        if(image->type == IMG_PGM || image->type == IMG_PGM_BIN)
            Image_SaveP2(image, fileName);
        if(image->type == IMG_PPM || image->type == IMG_PPM_BIN)
            Image_SaveP3(image, fileName);
    }
    if(isASCII == false)
    {
        if(image->type == IMG_PGM || image->type == IMG_PGM_BIN)
            Image_SaveP5(image, fileName);
        if(image->type == IMG_PPM || image->type == IMG_PPM_BIN)
            Image_SaveP6(image, fileName);
    }
}


/*********************************************
***************CITIRE IMAGINI*****************
*********************************************/


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
    
    if(maxVal > UINT16_MAX)
    {
        printf("avem o buba\n");
        free(image);
        return NULL;
    }
    else
        image->maxVal = maxVal;

    // verificare
    // printf("width = %d\theight = %d\tmaxVal = %d\n", image->width, image->height, image->maxVal);
    
    // 8. A single whitespace character (usually a newline).
    skip_whitespace(fp);

    // alocare dinamica matrice de pixeli grayscale
    PixelGrayScale_t **pixels;
    pixels = (PixelGrayScale_t**) malloc(image->height * sizeof(PixelGrayScale_t*));
    for(uint32_t i = 0; i < image->height; i++)
        pixels[i] = (PixelGrayScale_t*) malloc(image->width * sizeof(PixelGrayScale_t));
    
    // 9. citire pixeli grayscale din fisier text
    // A raster of Height rows, in order from top to bottom. Each row consists of Width pixels, in order from left to right.
    for(uint32_t i = 0; i < image->height; i++)
    {
        for(uint32_t j = 0; j < image->width; j++)
        {
            fscanf(fp, "%hhu", &pixels[i][j]);
           // printf("%3hhu ", pixels[i][j]);
        }
        // printf("\n");
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
    if(maxVal > UINT16_MAX)
    {
        printf("avem o buba\n");
        free(image);
        return NULL;
    }
    else
        image->maxVal = (uint16_t) maxVal;

    // verificare
    // printf("width = %d\theight = %d\tmaxVal = %d\n", image->width, image->height, image->maxVal);

    // 8. A single whitespace character (usually a newline).
    skip_whitespace(fp);

    // alocare dinamica matrice de pixeli RGB
    PixelRGB_t **pixels;
    pixels = (PixelRGB_t**) malloc(image->height * sizeof(PixelRGB_t*));
    for(uint32_t i = 0; i < image->height; i++)
        pixels[i] = (PixelRGB_t*) malloc(image->width * sizeof(PixelRGB_t));
    
    // citire pixeli RGB din fisier text
    for(uint32_t i = 0; i < image->height; i++)
    {
        for(uint32_t j = 0; j < image->width; j++)
        {
            fscanf(fp, "%hhu %hhu %hhu", &pixels[i][j].r, &pixels[i][j].g, &pixels[i][j].b);
           // printf("%3hhu %3hhu %3hhu ", pixels[i][j].r, pixels[i][j].g, pixels[i][j].b);
        }
       // printf("\n");
    }
    // salvarea pixelilor in structura
    image->pixels = (PixelRGB_t**) pixels;
    
    // eliberarea de resurse
    fclose(fp);

    return image;
}

// load p5.pgm - pgm binar grayscale
static Image* Image_LoadP5(const char *fileName)
{
    // deschiderea fisierului
    FILE *fp = fopen(fileName, "rb"); // vedem daca trebuie rb in functie de magic bytes
    
    // 1. stim magic number-ul ca functia a fost aplelata pe baza lui
    // skip magic bytes
    skip_magic_bytes_binary(fp);

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

    // verificare
    // printf("width = %u\theight = %u\n", image->width, image->height);

    // 6. Whitespace
    skip_whitespace_binary(fp);

    // 7. citire valoare maxima - ASCII decimal - {0, 1, ..., 65535} 
    uint32_t maxVal;
    citire_ascii_decimal_binary(fp, &maxVal);
    if(maxVal > UINT16_MAX)
        printf("avem o buba\n");
    else
        image->maxVal = maxVal;
    
    // 8. Whitespace
    skip_whitespace_binary(fp);

    // alocare dinamica matrice de pixeli grayscale
    PixelGrayScale_t **pixels;
    pixels = (PixelGrayScale_t**) malloc(image->height * sizeof(PixelGrayScale_t*));
    for(uint32_t i = 0; i < image->height; i++)
        pixels[i] = (PixelGrayScale_t*) malloc(image->width * sizeof(PixelGrayScale_t));

    // citire pixeli grayscale din fisier binar
    for(uint32_t i = 0; i < image->height; i++)
    {
        for(uint32_t j = 0; j < image->width; j++)
        {
            fread(&pixels[i][j], sizeof(PixelGrayScale_t), 1, fp);
            skip_whitespace_binary(fp);
            //printf("%2hhu ", pixels[i][j]);
        }
        //printf("\n");
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

    // 1. stim magic number-ul ca functia a fost aplelata pe baza lui
    // skip magic bytes
    skip_magic_bytes_binary(fp);

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

    // verificare
    // printf("width = %u\theight = %u\n", image->width, image->height);

    // 6. Whitespace
    skip_whitespace_binary(fp);

    // 7. citire valoare maxima - ASCII decimal - {0, 1, ..., 65535} 
    uint32_t maxVal;
    citire_ascii_decimal_binary(fp, &maxVal);
    if(maxVal > UINT16_MAX)
        printf("avem o buba\n");
    else
        image->maxVal = maxVal;
    
    // 8. Whitespace
    skip_whitespace_binary(fp);

    // alocare dinamica matrice de pixeli RGB
    PixelRGB_t **pixels;
    pixels = (PixelRGB_t**) malloc(image->height * sizeof(PixelRGB_t*));
    for(uint32_t i = 0; i < image->height; i++)
        pixels[i] = (PixelRGB_t*) malloc(image->width * sizeof(PixelRGB_t));
    
    // citire pixeli RGB din fisier binar
    for(uint32_t i = 0; i < image->height; i++)
    {
        for(uint32_t j = 0; j < image->width; j++)
        {
            fread(&pixels[i][j].r, sizeof(PixelGrayScale_t), 1, fp);
            skip_whitespace_binary(fp);
            fread(&pixels[i][j].g, sizeof(PixelGrayScale_t), 1, fp);
            skip_whitespace_binary(fp);
            fread(&pixels[i][j].b, sizeof(PixelGrayScale_t), 1, fp);
            skip_whitespace_binary(fp);
            //printf("%3hhu %3hhu %3hhu ", pixels[i][j].r, pixels[i][j].g, pixels[i][j].b);
        }
        //printf("\n");
    }
    // salvarea pixelilor in structura
    image->pixels = pixels;
    // printf("width = %d\theight = %d\n", image->width, image->height);
    
    // eliberarea de resurse
    fclose(fp);

    return image;
}


/*********************************************
***************SALVARE IMAGINI****************
*********************************************/


// save p2.pgm - pgm ASCII grayscale
static void Image_SaveP2(Image *image, const char *fileName)
{
    FILE *fp = fopen(fileName, "w");
    char magicBytes[] = "P2";
    fprintf(fp, "%s\n", magicBytes);
    fprintf(fp, "%u %u\n", image->width, image->height);
    fprintf(fp, "%hu\n", image->maxVal);

    PixelGrayScale_t **pixels = image->pixels;
    for(uint32_t i = 0; i < image->height; i++)
    {
        for(uint32_t j = 0; j < image->width; j++)
        {
            fprintf(fp, "%hhu ", pixels[i][j]);
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
}
// save p3.ppm - ppm ASCII RGB
static void Image_SaveP3(Image *image, const char *fileName)
{
    FILE *fp = fopen(fileName, "w");
    char magicBytes[] = "P3";
    fprintf(fp, "%s\n", magicBytes);
    fprintf(fp, "%u %u\n", image->width, image->height);
    fprintf(fp, "%hu\n", image->maxVal);

    PixelRGB_t **pixels = image->pixels;
    for(uint32_t i = 0; i < image->height; i++)
    {
        for(uint32_t j = 0; j < image->width; j++)
        {
            fprintf(fp, "%hhu %hhu %hhu ", pixels[i][j].r, pixels[i][j].g, pixels[i][j].b);
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
}
// save p5.pgm - pgm binar grayscale
static void Image_SaveP5(Image *image, const char *fileName)
{
    FILE *fp = fopen(fileName, "wb");
    char magicBytes[] = "P5";
    fprintf(fp, "%s\n", magicBytes);
    fprintf(fp, "%u %u\n", image->width, image->height);
    fprintf(fp, "%hu\n", image->maxVal);

    PixelGrayScale_t **pixels = image->pixels;
    for(uint32_t i = 0; i < image->height; i++)
    {
        for(uint32_t j = 0; j < image->width; j++)
        {
            fwrite(&pixels[i][j], sizeof(PixelGrayScale_t), 1, fp);
        }
    }
    fclose(fp);
}
// sav p6.ppm - binar RGB
static void Image_SaveP6(Image *image, const char *fileName)
{
    FILE *fp = fopen(fileName, "wb");
    char magicBytes[] = "P6";
    fprintf(fp, "%s\n", magicBytes);
    fprintf(fp, "%u %u\n", image->width, image->height);
    fprintf(fp, "%hu\n", image->maxVal);

    PixelRGB_t **pixels = image->pixels;
    for(uint32_t i = 0; i < image->height; i++)
    {
        for(uint32_t j = 0; j < image->width; j++)
        {
            fwrite(&pixels[i][j].r, sizeof(uint8_t), 1, fp);
            fwrite(&pixels[i][j].g, sizeof(uint8_t), 1, fp);
            fwrite(&pixels[i][j].b, sizeof(uint8_t), 1, fp);
        }
    }
    fclose(fp);
}
