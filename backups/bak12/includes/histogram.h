#ifndef _HISTOGRAM_H_
#define _HISTOGRAM_H_
#include "commons.h"

typedef struct Histogram {
	int num_bins;
	int bin_size;
	int *bins;
	int num_astericks;
	int *astericks;
}Histogram;
void init_histogram(Histogram *histogram, int num_astericks, int num_bins);
void equalization_histogram(Image *image, Histogram *histogram);
void calculate_histogram(Image *image, Histogram *histogram);
void display_histogram(Histogram *histogram);
void free_histogram(Histogram *histogram);

#endif /* _HISTOGRAM_H_ */