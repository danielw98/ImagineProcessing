#include "commands.h"


Image* Commands_CMD_LOAD(Image *image, const char *cmdString)
{
    // printf("CMD_LOAD\n");
    // incarca in memoria programului img PPM sau PGM
    // fisier existent - inlocuit - atentie la memory leak-uri aici
    if(image != NULL)
    {
        Image_Free(image);
        image = NULL;
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
        return image;
        
    }
    // incarcare - automat selectam complet
    image = Image_Load(fileName);

    // Commands_CMD_SELECT_ALL(); // TODO!!!
    
    // format gresit al imaginii sau alte probleme
    if(image == NULL)
        printf("Failed to load %s\n", fileName);
    else
        printf("Loaded %s\n", fileName);
    return image;
}
void Commands_CMD_SELECT(void)
{
    printf("CMD_SELECT\n");
}
void Commands_CMD_SELECT_ALL(void)
{
    printf("CMD_SELECT_ALL\n");
}
void Commands_CMD_HISTOGRAM(void)
{
    printf("CMD_HISTOGRAM\n");
}
void Commands_CMD_EQUALIZE(void)
{
    printf("CMD_EQUALIZE\n");
}
void Commands_CMD_ROTATE(void)
{
    printf("CMD_ROTATE\n");
}
void Commands_CMD_CROP(void)
{
    printf("CMD_CROP\n");
}
void Commands_CMD_APPLY(void)
{
    printf("CMD_APPLY\n");
}
void Commands_CMD_SAVE(Image *image, char *cmd)
{
    char *fileName = "inexistent";
    char *ascii = NULL;
    if(image == NULL)
    {
        printf("No image loaded\n");
        return;
    }
    printf("Saved %s\n", fileName);
}
void Commands_CMD_EXIT(Image *image)
{
    if(image != NULL) {
        Image_Free(image);
        return;
    }
    printf("No image loaded\n");
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
    strcpy(commandsArray[1], "SELECT");
    strcpy(commandsArray[2], "SELECT ALL");
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
        // else {
        //     printf("%s vs %s\n", cmdString, commandsArray[i]);
        // }
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