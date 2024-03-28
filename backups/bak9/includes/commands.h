#ifndef _COMMANDS_H_
#define _COMMANDS_H_
#include "commons.h"
// LOAD <fisier>
// Incarca in memoria programului imaginea (PPM sau PGM) din fisierul dat
void Commands_CMD_LOAD(Image **image, Image **selection, const char *cmdString);
// SELECT ALL
// Readuce zona de selectie la intreaga imagine
void Commands_CMD_SELECT_ALL(Image *image, Image **selection);
// SELECT <x1> <y1> <x2> <y2>
// Restrange efectul comenzilor urmatoare la pixelii inclusi in intervalul [x1, x2) pe axa X (latimea imaginii), 
// respectiv in intervalul [y1, y2) pe axa Y (inaltimea imaginii).
void Commands_CMD_SELECT(Image *image, Image **selection, const char *cmdString);
// HISTOGRAM <x> <y>
// Afiseaza histograma imaginii folosind maxim x stelute si y bin-uri
// Valorile permise pentru y sunt puterile lui 2 din intervalul [2, 256]
void Commands_CMD_HISTOGRAM(Image *image, const char *cmdString);
// EQUALIZE
// Face egalizarea imaginii alb-negru conform histogramei
void Commands_CMD_EQUALIZE(Image *image);
// ROTATE <unghi> 
// Rotirea selectiei curente cu un anumit numar (exprimat in grade)
// Valorile permise pentru unghi sunt +/-90, +/-180, +/-270, +/-360.
void Commands_CMD_ROTATE(Image **selection, const char *cmdString);
// CROP 
// Reduce imaginea la cea cuprinsa în cadrul selectiei curente
void Commands_CMD_CROP(Image **p_image, Image *selection);
// APPLY <PARAMETRU> 
// Aplica peste selectie nucleul PARAMETRU
// Valorile permise pentru PARAMETRU sunt: 
// * <EDGE> 
// * <SHARPEN>
// * <BLUR>
// * <GAUSSIAN_BLUR>
void Commands_CMD_APPLY(Image *selection, const char *cmdString);
// SAVE <nume_fisier> [ascii] -> salveaza imaginea in fisierul nume_fisier, binar sau ASCII, daca ultimul parametru este prezent
void Commands_CMD_SAVE(Image *image, char *cmd);
// EXIT -> eliberaza memoria si iese din program
bool Commands_CMD_EXIT(Image *image, Image *selection);
// O linie care nu poate fi interpretata precum comenzile de mai sus
void Commands_CMD_WRONG_COMMAND(void);
// Afiseaza Meniul cu lista de comenzi disponibile
void Commands_PrintMenu(void);
// Citeste comanda primita in cmdString si o returneaza, sau returnaza comanda inavalida
Commands Commands_ReadCommand(char *cmdString);


#endif /* _COMMANDS_H_ */