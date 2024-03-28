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
		printf("Invalid command\n");
		Image_Free(*p_image);
		*p_image = NULL;
		Image_Free(*p_selection);
		*p_selection = NULL;
		return;
	}

	// totul OK pana aici
	// format gresit al imaginii sau alte probleme
	if(Image_IsImage(fileName) == false){
		Image_Free(*p_image);
		*p_image = NULL;
		Image_Free(*p_selection);
		*p_selection = NULL;
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
	// incarcare imagine din fisier - functie generica - ghiceste tipul fisierului
	// incarcarea imagini curente
	*p_image = Image_Load(fileName);
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
		printf("Invalid command\n");
		return;
	}
	if(x1 > x2)
	{
		int tmp = x1;
		x1 = x2;
		x2 = tmp;
	}
	if(y1 > y2)
	{
		int tmp = y1;
		y1 = y2;
		y2 = tmp;
	}
	// Invalid coordinates daca vreunul din punctele de delimitare se afla in afara imaginii
	if(x1 < 0 || (uint32_t)x1 > image->width  || x2 < 0 || (uint32_t)x2 > image->width || 
	   y1 < 0 || (uint32_t)y1 > image->height || y2 < 0 || (uint32_t)y2 > image->height)
	{
		printf("Invalid set of coordinates\n");
		return;
	}
	// nu pot face o selectie de 0 pixeli pe rand sau coloana
	if(x1 == x2 || y1 == y2)
	{
		printf("Invalid set of coordinates\n");
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
	// se va afisa no image loaded daca nu exista fisier incarcat si ies
	if(image == NULL)
	{
		printf("No image loaded\n");
		return;
	}
	// citesc x si y din linia de comanda 
	// nu verific ca y este putere a lui 2 ca nu se cere explicit
	int x, y;
	char end_of_line[STR_SIZE] = {0};
	if(sscanf(cmdString, "HISTOGRAM %d %d %s\n", &x, &y, end_of_line) != 2){
		printf("Invalid command\n");
		return;
	}

	if(y < 2 || y > 256 || ((y & (y-1)) != 0)){
		printf("Invalid set of parameters\n");
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
	// printf("Afisare histograma\n");
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
		printf("No image loaded\n");
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

void Commands_CMD_ROTATE(Image **selection, Image **image, const char *cmdString)
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
	// verificam ca unghiul de rotatie sa fie plauzibil
	if(abs(angle) != 90 && abs(angle) != 180 && abs(angle) != 270 && abs(angle) != 360 && angle != 0)
	{
		printf("Unsupported rotation angle\n");
		return;
	}
	// daca nu rotim intreaga imagine
	// the selection must be square daca width difera de height
	if(((selection[0]->width != image[0]->width) || (selection[0]->height != image[0]->height)))
	{
		if(selection[0]->width != selection[0]->height)
		{
			printf("The selection must be square\n");
			return;
		}
		else
		{
			;
			// Rotate(selection);
		}
	}
	else
	{
		;
		// Rotate(image);
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

void Commands_CMD_APPLY(Image *image, Image *selection, const char *cmdString)
{
	// va aplica peste selectie unul din nucleele de imagine
	// daca inaintea de executia comenzii nu se incarca nicio imagine ma intorc
	if(selection == NULL)
	{
		printf("No image loaded\n");
		return;
	}
	// citire parametru
	char parametru[STR_SIZE];
	if(sscanf(cmdString, "APPLY %s", parametru) != 1)
	{
		printf("Invalid command\n");
		return;
	}
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
	// totul OK - verific si daca imaginea este grayscale
	// in cazul in care imaginea este grayscale
	if(selection->type == IMG_PGM || selection->type == IMG_PGM_BIN)
	{
		printf("Easy, Charlie Chaplin\n");
		return;
	}
	// totul OK - aplic filtrul asupra selectiei
	Filters_Apply(image, selection, filterType);
	// operatia s-a executat cu succes - afisez mesaj corespunzator
	printf("APPLY %s done\n", parametru);
}

void Commands_CMD_SAVE(Image *image, char *cmd)
{
	char ascii[32];
	char fileName[STR_SIZE];
	bool isASCII = true;
	// nu s-a incarcat inca o imagine - afisez mesajul no image loaded
	if(image == NULL)
	{
		printf("No image loaded\n");
		return;
	}
	if(sscanf(cmd, "SAVE %s %s\n", fileName, ascii) == 1)
	{
		isASCII = false;
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
		// programul iese oricum dar anunta eroarea
		stillRunning = false;
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
	Commands command = CMD_EXIT;
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
	
	if(cmdString[strlen(cmdString-1)] == '\n')
		cmdString[strlen(cmdString)-1] = '\0';
	int i;
	bool correctCommand = false;
	for(i = 0; i < CMD_NUM_COMMANDS; i++)
	{
		if(strncmp(cmdString, commandsArray[i], strlen(commandsArray[i])) == 0)
		{
			command = (Commands) i;
			correctCommand = true;
			break;
		}
	}
	if(correctCommand == false)
		command = CMD_WRONG_COMMAND;
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