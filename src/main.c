#include "../includes/mylib.h"

bool mainLoop(Image **p_image, Image **p_selection)
{
	bool running = true;
	char cmdString[STR_SIZE];
	Commands command = Commands_ReadCommand(cmdString);
	switch(command)
	{
		case CMD_LOAD: {
			Commands_CMD_LOAD(p_image, p_selection, cmdString);
			break;
		}
		case CMD_SELECT: {
			Commands_CMD_SELECT(*p_image, p_selection, cmdString);
			break;
		}
		case CMD_SELECT_ALL: {
			Commands_CMD_SELECT_ALL(*p_image, p_selection);
			break;
		}
		case CMD_HISTOGRAM: {
			Commands_CMD_HISTOGRAM(*p_image, cmdString);               
			break;
		}
		case CMD_EQUALIZE: {
			Commands_CMD_EQUALIZE(*p_selection);
			break;
		}
		case CMD_ROTATE: {
			Commands_CMD_ROTATE(p_selection, p_image, cmdString);
			break;
		}
		case CMD_CROP: {
			Commands_CMD_CROP(p_image, *p_selection);
			break;
		}
		case CMD_APPLY: {
			Commands_CMD_APPLY(*p_image, *p_selection, cmdString);
			break;
		}
		case CMD_SAVE: {
			Commands_CMD_SAVE(*p_image, cmdString);
			break;
		}
		case CMD_EXIT: {
			running = Commands_CMD_EXIT(*p_image, *p_selection);
			break;
		}
		case CMD_WRONG_COMMAND: {
			Commands_CMD_WRONG_COMMAND();
			break;
		}
		default: {
			printf("Nu ar trebui niciodata sa intre pe ramura asta");
		}
	}
	return running;
}

int main()
{
	Image *image = NULL;
	Image *selection = NULL;
	bool running = true;
	while(running)
	{
		running = mainLoop(&image, &selection);
	}
	return 0;
}
