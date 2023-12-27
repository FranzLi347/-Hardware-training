#ifndef	__FREE_TYPE
#define	__FREE_TYPE

#include <wchar.h>
#include <ft2build.h>
//#include "ftheader.h"
#include FT_FREETYPE_H


extern int display_font_to_screen(char *string, int size, unsigned int color, unsigned int start_x, unsigned int start_y, unsigned int *lcd_fb_addr);

#endif
