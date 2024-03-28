#include "commands.h"
#include "helpers.h"
#include "filters.h"
void Commands_CMD_LOAD(Image **p_image, Image **p_selection, const char *cmdString)
{
    // printf("CMD_LOAD\n");
    // incarca in memoria programului img PPM sau PGM
    // fisier existent - inlocuit - atentie la memory leak-uri aici
    Image *image = *p_image;
    Image *selection = *p_selection;
    
    char fileName[STR_SIZE];
    // vedem daca am primit si numele fisierului ca parametru
    // daca nu, ne intoarcem, nu specifica in documentatie sa afisam ceva
    if(sscanf(cmdString, "LOAD %s", fileName) <= 0)
    {   
        return;
    }

    // incarcare imagine din fisier - functie generica - ghiceste tipul fisierului
    Image *temp = Image_Load(fileName);
    // format gresit al imaginii sau alte probleme
    if(temp == NULL){
        printf("Failed to load %s\n", fileName);
        return;
    }

    // totul a mers ok - putem sa ne incepem treaba    
    // eliberam memoria pentru imaginea anterioara
    if(image != NULL)
    {
        Image_Free(image);
        image = NULL;
    }
    // eliberam memoria pentru selectia anterioar
    if(selection != NULL)
    {
        Image_Free(selection);
        selection = NULL;
    }
    // incarcam imaginea curenta
    image = temp;
    // selectam intreaga imagine la inceput
    Image_Copy(image, &selection, 0, 0, image->width, image->height);
    // salvam inapoi in variabilele trimise prin referinta imaginea si selectia
    *p_image = image;
    *p_selection = selection;
    // am facut cu bine treaba - afisam mesaj corespunzator
    printf("Loaded %s\n", fileName);
}

void Commands_CMD_SELECT(Image *image, Image **p_selection, const char *cmdString)
{
    uint32_t x1, y1;
    uint32_t x2, y2;
    // No image loaded dacă nu exista un fisier incarcat.
    if(image == NULL)
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
    if(x1 > image->width || x2 > image->width || y1 > image->height || y2 > image->height)
    {
        printf("Invalid coordinates %d %d %d %d\n", x1, y1, x2, y2);
        return;
    }

    // aici se ajunge doar daca este valida operatia
    Image_Copy(image, p_selection, x1, y1, x2, y2);

    // Selected <x1> <y1> <x2> <y2> daca operatia s-a executat cu succes.
    printf("Selected %u %u %u %u\n", x1, y1, x2, y2);
}
void Commands_CMD_SELECT_ALL(Image *image, Image **p_selection)
{
    if(image == NULL){
        printf("No image loaded\n");
        return;
    }
    Image_Copy(image, p_selection, 0, 0, image->width, image->height);
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

void Commands_CMD_CROP(Image **p_image, Image *selection)
{
    if(*p_image == NULL){
        printf("No image loaded\n");
        return;
    }
    Image_Copy(selection, p_image, 0, 0, selection->width, selection->height);
    printf("Image cropped\n");
}

void Commands_CMD_APPLY(Image *selection, const char *cmdString)
{
    // citire parametru - se putea si cu strtok dupa " "
    char parametru[STR_SIZE];
    sscanf(cmdString, "APPLY %s", parametru);
    // filtrele posibile ca input
    char *filtre[NR_FILTRE] = {
        "EDGE",
        "SHARPEN",
        "BLUR",
        "GAUSSIAN_BLUR"
    };
    // verificare parametru sa fie unul din filtrele de mai sus
    FilterEnum filterType = FILTER_UNKNOWN;
    for(int i = 0; i < NR_FILTRE; i++)
    {
        if(strcmp(parametru, filtre[i]) == 0)
        {
            filterType = (FilterEnum) i;
            break;
        }
    }
    // in cazul in care nu se introduce param corespunzator
    if(filterType == FILTER_UNKNOWN)
    {
        printf("APPLY parameter invalid\n");
        return;
    }
    // daca inaintea de executia comenzii nu se incarca nicio imagine
    if(selection == NULL)
    {
        printf("No image loaded\n");
        return;
    }
    // in cazul in care imaginea este grayscale
    if(selection->type == IMG_PGM || selection->type == IMG_PGM_BIN)
    {
        printf("Easy, Charlie Chaplin\n");
        return;
    }
    // aplic filtrul - acum totul a mers ok
    Filters_Apply(selection, filterType);
    // output-ul comenzii
    printf("APPLY %s done\n", parametru);
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
    //Commands_PrintMenu();
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