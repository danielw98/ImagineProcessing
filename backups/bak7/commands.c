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
    // citesc x si y din linia de comanda 
    // nu verific ca y este putere a lui 2 ca nu se cere explicit
    int x, y;
    sscanf(cmdString, "HISTOGRAM %d %d\n", &x, &y);
    // se va afisa no image loaded daca nu exista fisier incarcat
    if(*p_selection == NULL)
    {
        printf("No image loaded\n");
        return;
    }
    // black and white needed daca imaginea este color
    if(p_selection[0]->type == IMG_PPM || p_selection[0]->type == IMG_PPM_BIN)
    {
        printf("Black and white image needed\n");
        return;
    }
    // TODO: calculare histograma
    // TODO: afisare histograma
    printf("Afisare histograma\n");
}

void Commands_CMD_EQUALIZE(Image **selection)
{
    // TODO: operatia se va realiza pe toata imaginea, indiferent de selectia curenta - schimba param
    // no image loaded daca nu exista fisier incarcat
    if(*selection == NULL)
    {
        printf("NO image loaded\n");
        return;
    }
    // black and white image needed daca imaginea este color
    if(selection[0]->type == IMG_PPM || selection[0]->type == IMG_PPM_BIN)
    {
        printf("Black and white image needed\n");
        return;
    }
    // TODO: egalizarea propriu zisa a imaginii pe baza histogramei
    printf("Equalize done\n");
}

void Commands_CMD_ROTATE(Image **selection, const char *cmdString)
{
    // printf("TODO: Commands_CMD_ROTATE()\n");
    int angle;
    // citesc unghiul din linia de comanda
    sscanf(cmdString, "ROTATE %d\n", &angle);
    // no image loaded daca nu avem imagine
    if(*selection == NULL)
    {
        printf("No image loaded\n");
        return;
    }
    // the selection must be square daca width difera de height
    if(selection[0]->width != selection[0]->height)
    {
        printf("The selection must be square\n");
        return;
    }
    // verificam ca unghiul de rotatie sa fie plauzibil
    if(abs(angle) != 90 && abs(angle) != 180 && abs(angle) != 270 && abs(angle) != 360)
    {
        printf("Unsupported rotation angle\n");
        return;
    }
    // rotirea propriu zisa
    // TODO
    printf("Rotated %d\n", angle); 
}
// Reduce imaginea la cea cuprinsa in cadrul selectiei curente
void Commands_CMD_CROP(Image **p_image, Image *selection)
{
    // daca inaintea de executia comenzii nu se incarca nicio imagine
    if(*p_image == NULL){
        printf("No image loaded\n");
        return;
    }
    // incarc portiunea selectata - codul propriu zis
    Image_Copy(selection, p_image, 0, 0, selection->width, selection->height);
    // operatia s-a executat cu succes
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
// salveaza imaginea curenta in fisierul dat ca parametru
void Commands_CMD_SAVE(Image *image, char *cmd)
{
    // verific daca fisierul este ASCII sau nu
    bool isASCII = true;
    if(strstr(cmd, "ascii") == NULL)
        isASCII = false;
    
    // aflu numele fisierului, stiu ca se afla dupa comanda 
    // si e eventual delimitat la dreapta de un spatiu daca e ASCII
    strtok(cmd, " ");
    char *fileName = strtok(NULL, " ");;
    // nu s-a incarcat inca o imagine - afisez mesajul no image loaded
    if(image == NULL)
    {
        printf("No image loaded\n");
        return;
    }
    // am ce imagine sa salvez - execut codul corespunzator
    // isASCII spune daca o salvez ASCII sau binar
    Image_Save(image, fileName, isASCII);
    // comanda afiseaza dupa salvarea fisierului un mesaj de succes
    printf("Saved %s\n", fileName);
}

bool Commands_CMD_EXIT(Image *image, Image *selection)
{
    // daca selectia si imaginea sunt ambele nule, nu avem nimic incarcat, ne intoarcem in bucla
    bool stillRunning;
    if(selection == NULL && image == NULL)
    {
        printf("No image loaded\n");
        stillRunning = true;
        return stillRunning;
    }
    // selectia poate sa existe sau sa nu existe
    if(selection != NULL)
        Image_Free(selection);
    // imaginea sigur exista daca s-a ajuns pana aici - nu exista caz in care img = NULL si selectie != NULL
    if(image != NULL) {
        Image_Free(image);
        stillRunning = false;
        return stillRunning;
    }
    // teoretic nu se intra niciodata aici, dar trebuie return pe toate ramurile
    stillRunning = true;
    return stillRunning;
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