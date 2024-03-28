#ifndef _HELPERS_H_
#define _HELPERS_H_
#include "commons.h"

uint8_t clamp(double val);
void skip_comments(FILE *fp);
void skip_magic_bytes(FILE *fp, char *line);
void skip_whitespace(FILE *fp);
void citire_ascii_decimal(FILE *fp, uint32_t *val);
void skip_comments_binary(FILE *fp);
void skip_magic_bytes_binary(FILE *fp);
void skip_whitespace_binary(FILE *fp);
void citire_ascii_decimal_binary(FILE *fp, uint32_t *val);
void citire_ascii_decimal_binary8bit(FILE *fp, PixelGrayScale_t *pix);
union MagicBytes readMagicBytes(const char *fileName);
#endif /* _HELPERS_H_ */