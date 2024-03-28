#ifndef _COMMANDS_H_
#define _COMMANDS_H_
#include "commons.h"
#include "image.h"
void Commands_CMD_LOAD(Image **image, Image **selection, const char *cmdString);
void Commands_CMD_SELECT_ALL(Image **image, Image **selection);
void Commands_CMD_SELECT(Image **image, Image **selection, const char *cmdString);
void Commands_CMD_HISTOGRAM(Image **selection, const char *cmdString);
void Commands_CMD_EQUALIZE(Image **selection);
void Commands_CMD_ROTATE(Image **selection, const char *cmdString);
void Commands_CMD_CROP(Image **selection, const char *cmdString);
void Commands_CMD_APPLY(Image **image, Image **selection, const char *cmdString);
void Commands_CMD_SAVE(Image *image, char *cmd);
void Commands_CMD_EXIT(Image *image, Image *selection);
void Commands_CMD_WRONG_COMMAND(void);
void Commands_PrintMenu(void);
Commands Commands_ReadCommand(char *cmdString);


#endif /* _COMMANDS_H_ */