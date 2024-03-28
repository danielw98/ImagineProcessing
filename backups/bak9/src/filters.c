#include "../includes/filters.h"
#include "../includes/helpers.h"
typedef struct Filters{
    int8_t edge_detection[KERNEL_SIZE][KERNEL_SIZE];
    int8_t sharpen[KERNEL_SIZE][KERNEL_SIZE];
    int8_t box_blur[KERNEL_SIZE][KERNEL_SIZE];
    int8_t gaussian_blur[KERNEL_SIZE][KERNEL_SIZE];
    double gaussian_blur_normalization_coefficient;
    double box_blur_normalization_coefficient;
}Filters;
typedef struct PixelDouble {
    double r;
    double g;
    double b;
}PixelDouble;
static void Filters_Copy(int8_t dst[KERNEL_SIZE][KERNEL_SIZE], int8_t src[KERNEL_SIZE][KERNEL_SIZE])
{
    for(int i = 0; i < KERNEL_SIZE; i++)
        for(int j = 0; j < KERNEL_SIZE; j++)
            dst[i][j] = src[i][j];
}
static Filters Filters_Init(void)
{
    Filters filters;
    int8_t edge_detection[KERNEL_SIZE][KERNEL_SIZE] = {
        {-1, -1, -1},
        {-1,  8, -1},
        {-1, -1, -1}
    };
    int8_t sharpen[KERNEL_SIZE][KERNEL_SIZE] = {
        { 0, -1,  0},
        {-1,  5, -1},
        { 0, -1,  0}
    };
    int8_t box_blur[KERNEL_SIZE][KERNEL_SIZE] = {
        { 1,  1,  1},
        { 1,  1,  1},
        { 1,  1,  1}
    };
    int8_t gaussian_blur[KERNEL_SIZE][KERNEL_SIZE] = {
        { 1,  2,  1},
        { 2,  4,  2},
        { 1,  2,  1}
    };
    Filters_Copy(filters.edge_detection, edge_detection);
    Filters_Copy(filters.sharpen, sharpen);
    Filters_Copy(filters.box_blur, box_blur);
    Filters_Copy(filters.gaussian_blur, gaussian_blur);
    filters.box_blur_normalization_coefficient = 9;
    filters.gaussian_blur_normalization_coefficient = 16;
    return filters;
}

static PixelDouble Filters_Convolution(Image *image, double kernel[KERNEL_SIZE][KERNEL_SIZE], uint32_t row, uint32_t col) 
{   
    // returnam rezultatele pentru toti cei 3 pixeli
    PixelDouble pixelDouble;
	double sum_red = 0;
    double sum_green = 0;
    double sum_blue = 0;
    // imaginea este RGB
    PixelRGB_t **pixels = (PixelRGB_t**) image->pixels;
	for (uint32_t i = 0; i < KERNEL_SIZE; i++) 
		for (uint32_t j = 0; j < KERNEL_SIZE; j++)
        {
            // nu facem nimic pentru pixelii care nu sunt complet in zona
            if(i+row >= image->width || j+col >= image->height)
                continue;
            // transform pixelii curenti in double pentru calcule
            double red   = (double) pixels[i+row][j+col].r;
            double green = (double) pixels[i+row][j+col].g;
            double blue  = (double) pixels[i+row][j+col].b;
            // adaug influenta pixelulului curent la pixelul (row,col)
			sum_red     += red   * kernel[i][j];
            sum_green   += green * kernel[i][j];
            sum_blue    += blue  * kernel[i][j];
        }
    pixelDouble.r = sum_red;
    pixelDouble.g = sum_green;
    pixelDouble.b = sum_blue;
	return pixelDouble;
}

// transform din kernel-ul de uint8_t[3][3] (din motive de spatiu) in kernel de double[3][3] pt precizie maxima 
static void Filters_InitKernel(double kernel[KERNEL_SIZE][KERNEL_SIZE], Filters *filter, FilterEnum filterType)
{
    switch(filterType)
    {
        case FILTER_EDGE_DETECTION: {
            for(int i = 0; i < KERNEL_SIZE; i++)
                for(int j = 0; j < KERNEL_SIZE; j++)
                    kernel[i][j] = filter->edge_detection[i][j];
            break;
        }
        case FILTER_SHARPEN: {
             for(int i = 0; i < KERNEL_SIZE; i++)
                for(int j = 0; j < KERNEL_SIZE; j++)
                    kernel[i][j] = filter->sharpen[i][j];
            break;
        }
        case FILTER_BOX_BLUR:{
             for(int i = 0; i < KERNEL_SIZE; i++)
                for(int j = 0; j < KERNEL_SIZE; j++)
                    kernel[i][j] = filter->box_blur[i][j]/filter->box_blur_normalization_coefficient;
            break;
        }
        case FILTER_GAUSSIAN_BLUR: {
             for(int i = 0; i < KERNEL_SIZE; i++)
                for(int j = 0; j < KERNEL_SIZE; j++)
                    kernel[i][j] = filter->gaussian_blur[i][j]/filter->gaussian_blur_normalization_coefficient;
            break;
        }
        // filtrul e garantat unul din cele 4, altfel nu se apela functia
        default: {
            printf("should never occur\n");
            break;
        }
    }
}
static void Filters_ApplyKernel(Image *image, Filters *filter, FilterEnum filterType)
{
    // double pentru ca sa avem cat mai mare precizie
    double kernel[KERNEL_SIZE][KERNEL_SIZE];
    Filters_InitKernel(kernel, filter, filterType);
    // sigur sunt pixeli RGB
    PixelRGB_t **pixelsRGB  = (PixelRGB_t**) image->pixels;
    // aplica kernelul curent pe portiunea de imagine admisa
    for(uint32_t i = 0; i < image->height; i++)
    {
        for(uint32_t j = 0; j < image->width; j++)
        {
            PixelDouble result = Filters_Convolution(image, kernel, i, j);
            pixelsRGB[i][j].r = (uint8_t) result.r;
            pixelsRGB[i][j].g = (uint8_t) result.g;
            pixelsRGB[i][j].b = (uint8_t) result.b;
            if(filterType == FILTER_EDGE_DETECTION)
            {
                pixelsRGB[i][j].r = clamp(result.r);
                pixelsRGB[i][j].g = clamp(result.g);
                pixelsRGB[i][j].b = clamp(result.b);
            }
        }
    }
}
static void Filters_EdgeDetection(Filters *filters, Image *image)
{
    // sigur sunt pixeli RGB, deci functia nu verifica tipul imaginii
    Filters_ApplyKernel(image, filters, FILTER_EDGE_DETECTION);
}
static void Filters_Sharpen(Filters *filters, Image *image)
{
    // sigur sunt pixeli RGB, deci functia nu verifica tipul imaginii
    Filters_ApplyKernel(image, filters, FILTER_SHARPEN);
}
static void Filters_BoxBlur(Filters *filters, Image *image)
{
    // sigur sunt pixeli RGB, deci functia nu verifica tipul imaginii
    Filters_ApplyKernel(image, filters, FILTER_BOX_BLUR);
}
static void Filters_GaussianBlur(Filters *filters, Image *image)
{
    // sigur sunt pixeli RGB, deci functia nu verifica tipul imaginii
    Filters_ApplyKernel(image, filters, FILTER_GAUSSIAN_BLUR);
}

// aici intram daca stim sigur ca avem unul din cele 4 filtre de aplicat
// si imaginea este nenula si apelam functia corespunzatoare fiecareui filtru
void Filters_Apply(Image *image, FilterEnum type)
{
    // initializare structura statica filtre cu toate filtrele necesare si coeficientii de normalizare
    // doar la primul apel al functiei
    static Filters filters;
    static int numCalls = 0;
    if(numCalls == 0)
    {
        filters = Filters_Init();
        numCalls++;
    }
    // aplicarea filtrului corespunzator
    switch(type){
        case FILTER_EDGE_DETECTION: {
            Filters_EdgeDetection(&filters, image);
            break;
        }
        case FILTER_SHARPEN: {
            Filters_Sharpen(&filters, image);
            break;
        }
        case FILTER_BOX_BLUR:{
            Filters_BoxBlur(&filters, image);
            break;
        }
        case FILTER_GAUSSIAN_BLUR: {
            Filters_GaussianBlur(&filters, image);
            break;
        }
        // filtrul e garantat unul din cele 4, altfel nu se apela functia
        default: {
            printf("should never occur\n");
            break;
        }
    }
    
}