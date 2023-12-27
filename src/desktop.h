#ifndef __DESKTOP_H
#define __DESKTOP_H

#define DEVICE_NAME "/dev/ubuntu_lcd"

#include <sys/mman.h>
#include <linux/input.h>
#include "show_bmp.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "freetype.h"


void show_start_img();
void render_desktop_application();
void clear();

#endif