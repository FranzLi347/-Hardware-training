#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "show_bmp.h"
#include "desktop.h"

int main(){
    show_start_img();
    render_desktop_application();
    while (1)
    {
        listen_touch();
    }
    return 0;
}