#include "commands.h"
#include "helpers.h"
#include "filters.h"
#include "histogram.h"
#include "image.h"
void Commands_CMD_LOAD(Image **p_image, Image **p_selection, const char *cmdString)
{
    // incarca in memoria programului img PPM sau PGM
    // fisier existent - inlocuit

    // am primit si numele fisierului ca parametru ?
    // nu -> ies din functie nu specifica in documentatie sa afisez ceva
    char fileName[STR_SIZE];
    if(sscanf(cmdString, "LOAD %s", fileName) <= 0)
    {   
        return;
    }

    // totul OK pana aici

    // incarcare imagine din fisier - functie generica - ghiceste tipul fisierului
    Image *temp = Image_Load(fileName);
    // format gresit al imaginii sau alte probleme
    if(temp == NULL){
        printf("Failed to load %s\n", fileName);
        return;
    }

    // totul a mers OK - incarcarea propriu zisa
    // eliberare memorie pentru imaginea anterioara
    Image_Free(*p_image);
    *p_image = NULL;
    // eliberare memorie pentru selectia anterioara
    Image_Free(*p_selection);
    *p_selection = NULL;
    // incarcarea imagini curente
    *p_image = temp;
    // intreaga imagine trebuie selectata la inceput
    Image_CopyAll(*p_image, p_selection);
    // functia s-a executat cu succes - afisare mesaj corespunzator
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
    // no image loaded daca nu a fost incarcata nicio imagine si ies
    if(image == NULL){
        printf("No image loaded\n");
        return;
    }
    // totul OK pana aici
    // copiez intreaga imagine in selectie
    Image_CopyAll(image, p_selection);
    // afisez output-ul corespunzator mesajului de succes
    printf("Selected ALL\n");
}

void Commands_CMD_HISTOGRAM(Image *image, const char *cmdString)
{
    int x, y;
    // citesc x si y din linia de comanda 
    // nu verific ca y este putere a lui 2 ca nu se cere explicit
    sscanf(cmdString, "HISTOGRAM %d %d\n", &x, &y);
    // se va afisa no image loaded daca nu exista fisier incarcat si ies
    if(image == NULL)
    {
        printf("No image loaded\n");
        return;
    }
    // black and white needed daca imaginea este color si ies
    if(image->type == IMG_PPM || image->type == IMG_PPM_BIN)
    {
        printf("Black and white image needed\n");
        return;
    }
    // totul OK - pot sa calculez si sa afisez histograma
    Histogram histogram;
    // initializare histograma
    init_histogram(&histogram, x, y);
    // calculare histograma
    calculate_histogram(image, &histogram);
    // afisare histograma
    printf("Afisare histograma\n");
    display_histogram(&histogram);
    // eliberare de memorie pentru histograma
    free_histogram(&histogram);
}

void Commands_CMD_EQUALIZE(Image *image)
{
    // comanda opereaza pe intreaga imagine
    // no image loaded daca nu exista fisier incarcat si ies
    if(image == NULL)
    {
        printf("NO image loaded\n");
        return;
    }
    // black and white image needed daca imaginea este color si ies
    if(image->type == IMG_PPM || image->type == IMG_PPM_BIN)
    {
        printf("Black and white image needed\n");
        return;
    }
    // totul OK pana aici, pot sa fac egalizarea egalizarea propriu zisa a imaginii pe baza histogramei
    // calculez histograma
    Histogram histogram;
    // initializez histograma - nr de bin-uri este maxim, nu ma intereseaza nr de stelute ca nu o afisez
    init_histogram(&histogram, 0, 256);
    // calculez histograma 
    calculate_histogram(image, &histogram);
    // aplic algoritmul de egalizare a imaginii pe baza histogramei
    equalization_histogram(image, &histogram);
    // eliberez memoria
    free_histogram(&histogram);
    // totul s-a terminat cu bine, afisez mesajul cerut
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


void Commands_CMD_CROP(Image **p_image, Image *selection)
{
    // Reduce imaginea la cea cuprinsa in cadrul selectiei curente
    // daca inaintea de executia comenzii nu se incarca nicio imagine afisez mesaj corespunzator si ies
    if(*p_image == NULL){
        printf("No image loaded\n");
        return;
    }
    // totul OK - incarc portiunea selectata - codul propriu zis
    Image_Copy(selection, p_image, 0, 0, selection->width, selection->height);
    // operatia s-a executat cu succes - afisez mesaj corespunzator
    printf("Image cropped\n");
}

void Commands_CMD_APPLY(Image *selection, const char *cmdString)
{
    // va aplica peste selectie unul din nucleele de imagine
    // daca inaintea de executia comenzii nu se incarca nicio imagine ma intorc
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
    // totul OK - aplic filtrul asupra selectiei
    Filters_Apply(selection, filterType);
    // operatia s-a executat cu succes - afisez mesaj corespunzator
    printf("APPLY %s done\n", parametru);
    return;
    // se aplica doar asupra selectiei, am vazut mai tarziu, nu asupra intergii imagini
    // // selectez intreaga imagine
    // Image_CopyAll(p_image[0], p_selection);
    // // aplic filtrul - acum totul a mers ok
    // Filters_Apply(p_selection[0], filterType);
    // // copiez inapoi din selectie imaginea
    // Image_CopyAll(p_selection[0], p_image);
}

void Commands_CMD_SAVE(Image *image, char *cmd)
{
    // nu s-a incarcat inca o imagine - afisez mesajul no image loaded si ies
    if(image == NULL)
    {
        printf("No image loaded\n");
        return;
    }
    // totul OK - verific daca fisierul este ASCII sau nu
    bool isASCII = true;
    if(strstr(cmd, "ascii") == NULL)
        isASCII = false;
    // aflu numele fisierului, stiu ca se afla dupa comanda delimitat de spatiu
    strtok(cmd, " ");
    // si e eventual delimitat la dreapta de un spatiu daca e ASCII
    char *fileName = strtok(NULL, " ");
    // salvarea imaginii in fisier ASCII/binar
    Image_Save(image, fileName, isASCII);
    // comanda afiseaza dupa salvarea fisierului un mesaj de succes
    printf("Saved %s\n", fileName);
}

bool Commands_CMD_EXIT(Image *image, Image *selection)
{
    bool stillRunning = true;
    // daca imaginea este nula, nu avem nimic incarcat
    if(image == NULL)
    {
        printf("No image loaded\n");
        // programul se intoarce in bucla
        stillRunning = true;
        return stillRunning;
    }
    // selectia poate sa existe sau sa nu existe - nu sunt sigur - incerc sa o eliberez
    Image_Free(selection);
    // imaginea sigur exista daca s-a ajuns pana aici - o eliberez
    Image_Free(image);
    // programul isi anunta incheierea executiei
    stillRunning = false;
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
    int commandsLen[] = {4, 10, 6, 9, 8, 6, 4, 5, 4, 4};
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
    " 1. LOAD <fisier>\n"
    " 2. SELECT <x1> <y1> <x2> <y2>\n"
    " 3. SELECT ALL\n"
    " 4. HISTOGRAM <x> <y>\n"
    " 5. EQUALIZE\n"
    " 6. ROTATE <unghi>\n"
    " 7. CROP\n"
    " 8. APPLY <PARAMETRU>\n"
    " 9. SAVE <nume_fisier> [ascii]\n"
    "10. EXIT\n");
}