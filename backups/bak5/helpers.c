#include "helpers.h"

void skip_comments(FILE *fp)
{
    char line[STR_SIZE];
    while(true)
    {
        fgets(line, STR_SIZE-1, fp);
        if(line[0] == '#')
            continue;
        break;
    }
    // unread last line
    fseek(fp, -strlen(line), SEEK_CUR);
}

void skip_magic_bytes(FILE *fp, char *line)
{
    union MagicBytes magicBytes;
    fgets(line, STR_SIZE-1, fp);
    sscanf(line, "%s\n", magicBytes.str);
}

void skip_whitespace(FILE *fp)
{
    char ch;
    while(fread(&ch, sizeof(char), 1, fp) > 0)
    {
        if(!isspace(ch))
            break;
    }
    // unread last char
    fseek(fp, -sizeof(char), SEEK_CUR);
}

void citire_ascii_decimal(FILE *fp, uint32_t *val)
{
    fscanf(fp, "%u", val);
}


// binary functions
void skip_comments_binary(FILE *fp)
{
    char ch = '\0';
    while(true)
    {
        fread(&ch, sizeof(char), 1, fp);
        if(ch != '#')
            break;
    }
    // unread last char
    fseek(fp, -sizeof(char), SEEK_CUR);
}

void skip_magic_bytes_binary(FILE *fp)
{
    uint16_t magicBytes;
    fread(&magicBytes, sizeof(uint16_t), 1, fp);
}


void skip_whitespace_binary(FILE *fp)
{
    char ch;
    while(fread(&ch, sizeof(char), 1, fp) > 0)
    {
        if(!isspace(ch))
            break;
    }
    // unread last char
    fseek(fp, -sizeof(char), SEEK_CUR);
}

void citire_ascii_decimal_binary(FILE *fp, uint32_t *val)
{
    char str[STR_SIZE];
    int i = 0;
    while(fread(&str[i], sizeof(char), 1, fp) > 0)
    {
        if(!isdigit(str[i]))
            break;
        i++;
    }
    str[i] = '\0';
    *val = atoi(str);
}
void citire_ascii_decimal_binary8bit(FILE *fp, PixelGrayScale_t *pix)
{
    fread(&pix, sizeof(PixelGrayScale_t), 1, fp);
}


union MagicBytes readMagicBytes(const char *fileName)
{
    union MagicBytes magicBytes;
    FILE *fp = fopen(fileName, "r");
    if(fp == NULL)
    {
        perror(fileName);
        exit(EXIT_FAILURE);
    }
    char line[STR_SIZE];
    fgets(line, STR_SIZE-1, fp);
    sscanf(line, "%2s\n", magicBytes.str);
    fclose(fp);
    return magicBytes;
}