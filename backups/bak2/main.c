#include "mylib.h"

bool mainLoop(Image **p_image, PixelGrayScale_t ***p_grayCrtPix, PixelRGB_t ***p_rgbCrtPix)
{
    Image *image = *p_image;
    PixelGrayScale_t **grayCrtPix = *p_grayCrtPix;
    PixelRGB_t **rgbCrtPix = *p_rgbCrtPix;
    bool running = true;
    char cmdString[STR_SIZE];
    Commands command = Commands_ReadCommand(cmdString);
    switch(command){
        case CMD_LOAD: {
            image = Commands_CMD_LOAD(image, cmdString);
            break;
        }
        case CMD_SELECT: {
            Commands_CMD_SELECT();
            break;
        }
        case CMD_SELECT_ALL: {
            Commands_CMD_SELECT_ALL();
            break;
        }
        case CMD_HISTOGRAM: {
            Commands_CMD_HISTOGRAM();               
            break;
        }
        case CMD_EQUALIZE: {
            Commands_CMD_EQUALIZE();
            break;
        }
        case CMD_ROTATE: {
            Commands_CMD_ROTATE();
            break;
        }
        case CMD_CROP: {
            Commands_CMD_CROP();
            break;
        }
        case CMD_APPLY: {
            Commands_CMD_APPLY();
            break;
        }
        case CMD_SAVE: {
            Commands_CMD_SAVE(image, cmdString);
            break;
        }
        case CMD_EXIT: {
            if(image == NULL)
                printf("\n\nDAMMMMNNN\n");
            Commands_CMD_EXIT(image);
            running = false;
            break;
        }
        case CMD_WRONG_COMMAND: {
            Commands_CMD_WRONG_COMMAND();
            break;
        }
        default: {
            printf("Nu ar trebui niciodata sa intre pe ramura asta");
        }
    }
    *p_image = image;
    *p_grayCrtPix = grayCrtPix;
    *p_rgbCrtPix = rgbCrtPix;
    return running;
}
int main()
{
    bool running = true;
    Image *image = NULL;
    PixelGrayScale_t **grayCrtPix = NULL;
    PixelRGB_t **rgbCrtPix = NULL;
    while(running)
    {
        running = mainLoop(&image, &grayCrtPix, &rgbCrtPix);
    }
    return 0;
}