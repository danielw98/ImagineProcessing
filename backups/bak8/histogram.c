#include "histogram.h"
#include "image.h"
#include <math.h>

void init_histogram(Histogram *histogram, int num_astericks, int num_bins)
{
    // nr de stelute este x-ul
    histogram->num_astericks = num_astericks;
    // nr de bin-uri este y - putere a lui 2, nu cere verificarea
    histogram->num_bins = num_bins;
    // dimensiunea unui bin, daca sunt 4 bin-uri, fiecare are 256/4 = 64 valori posibile
    // bin[0] [0, 63] bin[1] [64, 127] bin[2] [128, 191] bin[3] [192, 255]
    // pixeul 138 apartine bin-ului 2 deoarece [138/64] = 2 
    histogram->bin_size = 256 / histogram->num_bins;
    // initializez cu 0 frecventele din toate bin-urile
    histogram->bins = (int*) malloc(histogram->num_bins*sizeof(int));
    memset(histogram->bins, 0, histogram->num_bins * sizeof(int));
    // initializez cu 0 nr de stelute din toate bin-urile
    histogram->astericks = (int*) malloc(histogram->num_bins * sizeof(int));
    memset(histogram->astericks, 0, histogram->num_bins * sizeof(int));
}

void calculate_histogram(Image *image, Histogram *histogram)
{
    // stiu sigur ca am pixeli grayscale
    PixelGrayScale_t **pixels = (PixelGrayScale_t**) image->pixels;
    // calculez cati pixeli se afla in fiecare bin
    for(uint32_t i = 0; i < image->height; i++)
    {
        for(uint32_t j = 0; j < image->width; j++)
        {
            // presupunem 4 bin-uri => bin_size = 256/4 = 64
            // pixeul 112 apartine bin-ului 1 deoarece [112/64] = 1 
            int poz = pixels[i][j] / histogram->bin_size;
            histogram->bins[poz]++;
        }
    }
    // aflam frecventa maxima
    int max_freq = -1;
    for(int i = 0; i < histogram->num_bins; i++)
    {
        if(histogram->bins[i] > max_freq)
            max_freq = histogram->bins[i];
    }
    // pentru numar de stelute 0 nu are sens sa calculez nr de stelute pt fiecaere bin
    if(histogram->num_astericks == 0)
        return;
    // calculez nr de stelute pt fiecare bin
    for(int i = 0; i < histogram->num_bins; i++)
    {
        // numarul de stelute pentru fiecare bin conform formulei freq*nr_max_stelute/max_freq
        // bin-ul cu freq=max_freq va avea nr_max_stelute stelute
        histogram->astericks[i] = (histogram->bins[i] * histogram->num_astericks) / max_freq;
    }
}
void display_histogram(Histogram *histogram)
{
    // afisez histograma conform cerintei
    for(int i = 0; i < histogram->num_bins; i++)
    {
        printf("%d\t|\t", histogram->astericks[i]);
        for(int j = 0; j < histogram->astericks[i]; j++)
            printf("*");
        printf("\n");
    }
}

void free_histogram(Histogram *histogram)
{
    // eliberez campurile alocate dinamic din histograma
    free(histogram->astericks);
    free(histogram->bins);
}

void equalization_histogram(Image *image, Histogram *histogram)
{
    // egalizarea se aplica asupra intregii imagini
    // calculez sumele partiale ale histogramei si le salvez
    int sum = 0;
    int *sum_of_histogram = (int*) malloc(histogram->num_bins*sizeof(int));
    for(int i = 0; i < histogram->num_bins; i++)
    {
        sum += histogram->bins[i];
        sum_of_histogram[i] = sum;
    }
    // aria imaginii
    double area = image->height * image->width;
    // coeficientul de scalare a fiecarui pixel
    double scaling_coefficent = 255 / area;
    // salvez pixelii sa pot itera prin ei, stiu sigur ca sunt alb-negru
    PixelGrayScale_t **pixels = (PixelGrayScale_t**) image->pixels;
    for(uint32_t i = 0; i < image->height; i++)
    {
        for(uint32_t j = 0; j < image->width; j++)
        {
            // valoarea vechiului pixel
            PixelGrayScale_t current_pixel = pixels[i][j];
            // valoarea noului pixel este rotunjita la cel mai apropiat nr intreg
            int new_pixel = (int) round(scaling_coefficent * sum_of_histogram[current_pixel]);
            // fac un clamp dar nu este cazul, deoarece functia este cdf (cumulative distribution function)
            // deci retuneaza intre 0 si 1 (impartita la aria imaginii), de inmultit cu 255, maxim 255
            pixels[i][j] = (new_pixel > 255) ? 255 : (PixelGrayScale_t) new_pixel;
        }
    }
    // am terminat treaba
    // eliberez memoria alocata dinamic in cadrul functiei
    free(sum_of_histogram);
}
    

  