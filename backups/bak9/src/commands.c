#include "../includes/commands.h"
#include "../includes/helpers.h"
#include "../includes/filters.h"
#include "../includes/histogram.h"
#include "../includes/image.h"

// trebuie sa incarc si imaginea si selectia, deci trimit functiei unde sa le puna in memorie
void Commands_CMD_LOAD(Image **p_image, Image **p_selection, const char *cmdString)
{
    // incarca in memoria programului img PPM sau PGM
    // fisier existent - inlocuit

    // am primit si numele fisierului ca parametru ?
    // nu -> ies din functie nu specifica in documentatie sa afisez ceva
    char fileName[STR_SIZE];
    if(sscanf(cmdString, "LOAD %s", fileName) != 1)
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

// selectez o portiune din imagine si aloc spatiu pentru acea portiune in selectie
void Commands_CMD_SELECT(Image *image, Image **p_selection, const char *cmdString)
{
    int x1, y1;
    int x2, y2;
    // No image loaded dacă nu exista un fisier incarcat.
    if(image == NULL)
    {
        printf("No image loaded\n");
        return;
    }
    // citire coordonate din linia de comanda
    if(sscanf(cmdString, "SELECT %d %d %d %d\n", &x1, &y1, &x2, &y2) != 4)
    {
        // caz eroare - nu s-au putut citi toate variabilele
        return;
    }

    // Invalid coordinates daca vreunul din punctele de delimitare se afla in afara imaginii
    if(x1 < 0 || (uint32_t)x1 > image->width  || x2 < 0 || (uint32_t)x2 > image->width || 
       y1 < 0 || (uint32_t)y1 > image->height || y2 < 0 || (uint32_t)y2 > image->height)
    {
        printf("Invalid coordinates\n");
        return;
    }

    // aici se ajunge doar daca este valida operatia
    Image_Copy(image, p_selection, x1, y1, x2, y2);

    // Selected <x1> <y1> <x2> <y2> daca operatia s-a executat cu succes.
    printf("Selected %u %u %u %u\n", x1, y1, x2, y2);
}

// selectez intreaga imagine si aloc spatiu pentru acea portiune in selectie
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

// afisez histograma imaginii, deci nu afectez selectia si nici nu moific imaginea
void Commands_CMD_HISTOGRAM(Image *image, const char *cmdString)
{
    // citesc x si y din linia de comanda 
    // nu verific ca y este putere a lui 2 ca nu se cere explicit
    int x, y;
    if(sscanf(cmdString, "HISTOGRAM %d %d\n", &x, &y) != 2){
        // caz in care nu am primit 2 parametri
        return;
    }
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
// comanda se va realiza intotdeauna asupra intregii imagini
// selectia curenta ramane va nemodificata indiferent, nu o trimit ca parametru
void Commands_CMD_EQUALIZE(Image *image)
{
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
    int angle;
    // citesc unghiul din linia de comanda
    if(sscanf(cmdString, "ROTATE %d\n", &angle) != 1)
    {
        // caz eroare - nu am trimis unghiul ca parametru
        return;
    }
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
}

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
    "LOAD <fisier>\n"
    "SELECT <x1> <y1> <x2> <y2>\n"
    "SELECT ALL\n"
    "HISTOGRAM <x> <y>\n"
    "EQUALIZE\n"
    "ROTATE <unghi>\n"
    "CROP\n"
    "APPLY <PARAMETRU>\n"
    "SAVE <nume_fisier> [ascii]\n"
    "EXIT\n"
    );
}