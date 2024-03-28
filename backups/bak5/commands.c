#include "commands.h"
#include "helpers.h"

void Commands_CMD_LOAD(Image **p_image, Image **p_selection, const char *cmdString)
{
    // printf("CMD_LOAD\n");
    // incarca in memoria programului img PPM sau PGM
    // fisier existent - inlocuit - atentie la memory leak-uri aici
    Image *image = *p_image;
    Image *selection = *p_selection;
    if(image != NULL)
    {
        Image_Free(image);
        image = NULL;
    }
    if(selection != NULL)
    {
        Image_Free(selection);
        selection = NULL;
    }
    bool everythingWentWell = true;
    char *ptr = (*((char**) &cmdString)) + 4;
    // trim spaces
    while(*ptr == ' ')
        ptr++;
    
    char *fileName = ptr;
    if(*fileName == '\0')
        everythingWentWell = false;
    if(everythingWentWell == false){
        return;
    }
    // incarcare - automat selectam complet
    image = Image_Load(fileName);
    Commands_CMD_SELECT_ALL(&image, &selection); // asta are si output - vedem la teste
    //Image_Copy(*p_image, p_selection, 0, 0, p_image[0]->width, p_image[0]->height);
    // format gresit al imaginii sau alte probleme
    if(image == NULL)
        printf("Failed to load %s\n", fileName);
    else
        printf("Loaded %s\n", fileName);
    *p_image = image;
    *p_selection = selection;
}

void Commands_CMD_SELECT(Image **p_image, Image **p_selection, const char *cmdString)
{
    uint32_t x1, y1;
    uint32_t x2, y2;
    // No image loaded dacă nu exista un fisier incarcat.
    if(*p_image == NULL)
    {
        printf("No image loaded\n");
        return;
    }

    // citire coordonate din linia de comanda
    char unused[STR_SIZE];
    // printf("citite  = %s\n", cmdString); // SELECT 0 0 20 20 
    sscanf(cmdString, "%s %u %u %u %u\n", unused, &x1, &y1, &x2, &y2);
    // printf("primite = %s %u %u %u %u\n", unused, x1, y1, x2, y2);

    //Invalid coordinates daca vreunul din punctele de delimitare se afla in afara imaginii
    if(x1 > p_image[0]->width || x2 > p_image[0]->width || y1 > p_image[0]->height || y2 > p_image[0]->height)
    {
        printf("Invalid coordinates %d %d %d %d\n", x1, y1, x2, y2);
        return;
    }

    // aici se ajunge doar daca este valida operatia
    Image_Copy(*p_image, p_selection, x1, y1, x2, y2);

    // Selected <x1> <y1> <x2> <y2> daca operatia s-a executat cu succes.
    printf("Selected %u %u %u %u\n", x1, y1, x2, y2);
}
void Commands_CMD_SELECT_ALL(Image **p_image, Image **p_selection)
{
    if(*p_image == NULL){
        printf("No image loaded\n");
        return;
    }
    Image_Copy(*p_image, p_selection, 0, 0, p_image[0]->width, p_image[0]->height);
    printf("Selected ALL\n");
}

void Commands_CMD_HISTOGRAM(Image **p_selection, const char *cmdString)
{
    printf("CMD_HISTOGRAM\n");
}

void Commands_CMD_EQUALIZE(Image **selection)
{
    printf("CMD_EQUALIZE\n");
}

void Commands_CMD_ROTATE(Image **selection, const char *cmdString)
{
    printf("CMD_ROTATE\n");
}

void Commands_CMD_CROP(Image **image, Image **selection)
{
    if(*image == NULL){
        printf("No image loaded\n");
        return;
    }
    Image_Copy(*selection, image, 0, 0, selection[0]->width, selection[0]->height);
    printf("Image cropped\n");
}

void Commands_CMD_APPLY(Image **image, Image **selection, const char *cmdString)
{
    printf("CMD_APPLY\n");
}

void Commands_CMD_SAVE(Image *image, char *cmd)
{
    bool isASCII = true;
    if(strstr(cmd, "ascii") == NULL)
        isASCII = false;
    
    // aflu numele fisierului, stiu ca se afla dupa comanda 
    // si e eventual delimitat la dreapta de un spatiu daca e ASCII
    strtok(cmd, " ");
    char *fileName = strtok(NULL, " ");;

    if(image == NULL)
    {
        printf("No image loaded\n");
        return;
    }

    Image_Save(image, fileName, isASCII);
    printf("Saved %s\n", fileName);
}

bool Commands_CMD_EXIT(Image *image, Image *selection)
{
    if(selection != NULL)
        Image_Free(selection);
    if(image != NULL) {
        Image_Free(image);
        return false;
    }
    printf("No image loaded\n");
    return true;
}

void Commands_CMD_WRONG_COMMAND(void)
{
    printf("Invalid command\n");
}


Commands Commands_ReadCommand(char *cmdString)
{
    Commands command = CMD_WRONG_COMMAND;
    Commands_PrintMenu();
    int commandsLen[] = {4, 6, 10, 9, 8, 6, 4, 5, 4, 4};
    char **commandsArray = (char**) malloc(CMD_NUM_COMMANDS * sizeof(char*));
    for(int i = 0; i < CMD_NUM_COMMANDS; i++)
        commandsArray[i] = (char*) malloc((commandsLen[i]+1)* sizeof(char));
    
    strcpy(commandsArray[0], "LOAD");
    strcpy(commandsArray[1], "SELECT ALL");
    strcpy(commandsArray[2], "SELECT");
    strcpy(commandsArray[3], "HISTOGRAM");
    strcpy(commandsArray[4], "EQUALIZE");
    strcpy(commandsArray[5], "ROTATE");
    strcpy(commandsArray[6], "CROP");
    strcpy(commandsArray[7], "APPLY");
    strcpy(commandsArray[8], "SAVE");
    strcpy(commandsArray[9], "EXIT");

    fgets(cmdString, STR_SIZE-1, stdin);
    cmdString[strlen(cmdString)-1] = '\0';
    int i;
    for(i = 0; i < CMD_NUM_COMMANDS; i++)
    {
        if(strncmp(cmdString, commandsArray[i], strlen(commandsArray[i])) == 0)
        {
            command = (Commands) i;
            break;
        }
    }
    for(int i = 0; i < CMD_NUM_COMMANDS; i++)
        free(commandsArray[i]);
    free(commandsArray);
    return command;
}

void Commands_PrintMenu(void)
{
    printf(
    "Optiunile sunt:\n"
    " 1. LOAD\n"
    " 2. SELECT\n"
    " 3. SELECT_ALL\n"
    " 4. HISTOGRAM\n"
    " 5. EQUALIZE\n"
    " 6. ROTATE\n"
    " 7. CROP\n"
    " 8. APPLY\n"
    " 9. SAVE\n"
    "10. EXIT\n");
}