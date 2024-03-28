#ifndef _COMMANDS_H_
#define _COMMANDS_H_
#include "commons.h"
#include "image.h"
Image* Commands_CMD_LOAD(Image *image, const char *cmdString);
void Commands_CMD_SELECT(void);
void Commands_CMD_SELECT_ALL(void);
void Commands_CMD_HISTOGRAM(void);
void Commands_CMD_EQUALIZE(void);
void Commands_CMD_ROTATE(void);
void Commands_CMD_CROP(void);
void Commands_CMD_APPLY(void);
void Commands_CMD_SAVE(Image *image, char *cmd);
void Commands_CMD_EXIT(Image *image);
void Commands_CMD_WRONG_COMMAND(void);
void Commands_PrintMenu(void);
Commands Commands_ReadCommand(char *cmdString);

#endif /* _COMMANDS_H_ */