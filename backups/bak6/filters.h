#ifndef _FILTERS_H_
#define _FILTERS_H_
#include "commons.h"
#define KERNEL_SIZE 3
typedef enum FilterEnum {
    FILTER_EDGE_DETECTION,
    FILTER_SHARPEN,
    FILTER_BOX_BLUR,
    FILTER_GAUSSIAN_BLUR,
    FILTER_UNKNOWN
}FilterEnum;


void Filters_Apply(Image *image, FilterEnum type);
#endif /* _FILTERS_H_ */