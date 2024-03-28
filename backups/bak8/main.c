#include "mylib.h"

bool mainLoop(Image **p_image, Image **p_selection)
{
    bool running = true;
    char cmdString[STR_SIZE];
    Commands command = Commands_ReadCommand(cmdString);
    switch(command){
        case CMD_LOAD: {
            // trebuie sa incarc si imaginea si selectia, deci trimit
            // functiei unde sa le puna in memorie
            Commands_CMD_LOAD(p_image, p_selection, cmdString);
            break;
        }
        case CMD_SELECT: {
            // selectez o portiune din imagine si aloc spatiu pentru
            // acea portiune in selectie
            Commands_CMD_SELECT(*p_image, p_selection, cmdString);
            break;
        }
        case CMD_SELECT_ALL: {
            // selectez intreaga imagine si aloc spatiu pentru
            // acea portiune in selectie
            Commands_CMD_SELECT_ALL(*p_image, p_selection);
            break;
        }
        case CMD_HISTOGRAM: {
            // afisez histograma imaginii, deci nu afectez selectia
            // si nici nu moific imaginea
            Commands_CMD_HISTOGRAM(*p_image, cmdString);               
            break;
        }
        case CMD_EQUALIZE: {
            // comanda se va realiza intotdeauna asupra intregii imagini
            // selectia curenta ramane va nemodificata indiferent, nu o trimitem ca parametru
            Commands_CMD_EQUALIZE(*p_image);
            break;
        }
        case CMD_ROTATE: {
            Commands_CMD_ROTATE(p_selection, cmdString);
            break;
        }
        case CMD_CROP: {
            Commands_CMD_CROP(p_image, *p_selection);
            break;
        }
        case CMD_APPLY: {
            Commands_CMD_APPLY(*p_selection, cmdString);
            break;
        }
        case CMD_SAVE: {
            Commands_CMD_SAVE(*p_image, cmdString);
            break;
        }
        case CMD_EXIT: {
            running = Commands_CMD_EXIT(*p_image, *p_selection);
            break;
        }
        case CMD_WRONG_COMMAND: {
            printf("Command %s\n", cmdString);
            // Commands_CMD_WRONG_COMMAND();
            break;
        }
        default: {
            printf("Nu ar trebui niciodata sa intre pe ramura asta");
        }
    }
   
    return running;
}
int main()
{
    bool running = true;
    Image *image = NULL;
    Image *selection = NULL;
    while(running)
    {
        running = mainLoop(&image, &selection);
    }
    return 0;
}