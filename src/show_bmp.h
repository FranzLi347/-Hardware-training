#ifndef __SHOW_BMP_H
#define __SHOW_BMP_H

#include <stdio.h>
#include <sys/types.h>	
#include <sys/stat.h>	
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/mman.h>
#include <linux/input.h>

extern char *bmp_pathname[50];
extern char *source_bmp[10];

/* 函数的声明 */
void big_to_small(unsigned int*ptr, unsigned int * lcd_buffer, 
                    int bmp_height, int bmp_width, int start_x, int start_y);

void small_to_big(unsigned int*ptr, unsigned int * lcd_buffer, 
                    int bmp_height, int bmp_width, int start_x, int start_y);

void show_cur_size(unsigned int*ptr, unsigned int * lcd_buffer, 
                    int bmp_height, int bmp_width, int start_x, int start_y);

int show_bmp(char *pathname, int start_x, int start_y, int flag);



#endif
