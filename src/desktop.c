#include "desktop.h"


typedef struct application{
    char *name;
    char *path;
    void (*func)(void);
    int x, y;
    int width;
} Application;


void application_test_pop()
{
    printf("application_test_pop\n");
}

Application applications[] = {
    {"锁屏", "../static/application_icon/lock.bmp", &lock},
    {"视频", "../static/application_icon/2.bmp", &application_test_pop},
    {"图片", "../static/application_icon/tup.bmp", &application_test_pop},
    {"游戏", "../static/application_icon/youxiji.bmp", &application_test_pop},
    {"应用5", "../static/application_icon/5.bmp", &application_test_pop},
    {"应用5", "../static/application_icon/lock.bmp", &lock},
    {"应用5", "../static/application_icon/lock.bmp", &lock},
};


int lock_flag = 0;

void lock()
{
    printf("lock\n");
    /* 打开设备文件 /dev/ubuntu_lcd */
    int lcd_fd = open(DEVICE_NAME, O_RDWR); // 模拟器的设备文件

    // int lcd_fd = open("/dev/fb0", O_RDWR);      //实物的设备文件
    if (0 > lcd_fd)
    {
        perror("lcd open error!");
        return -1;
    }
    /* 内存映射 */
    unsigned int *ptr = (unsigned int *)mmap(NULL, 800 * 480 * 4,
                                             PROT_READ | PROT_WRITE, MAP_SHARED, lcd_fd, 0);
    if (NULL == ptr)
    {
        perror("mmap error!");
        return -1;
    }
    lock_flag = 1;
    bzero(ptr, 800 * 480 * 4);
}




void show_start_img()
{

    show_gif("../static/loading.gif");
}

void show_text(char *text, int x, int y, int size, int color)
{
    /* 打开设备文件 */
    int lcd_fd = open(DEVICE_NAME, O_RDWR);
    if (0 > lcd_fd)
    {
        perror("ubuntu_lcd open error!");
        return -1;
    }

    /* 内存映射 */
    unsigned int *ptr = (unsigned int *)mmap(NULL, 800 * 480 * 4,
                                             PROT_READ | PROT_WRITE, MAP_SHARED, lcd_fd, 0);
    if (NULL == ptr)
    {
        perror("mmap error!");
        return -1;
    }

    /* 将需要显示的内容写入到屏幕中 */
    display_font_to_screen(text, size, color,
                           x, y, ptr);

    close(lcd_fd);

    return 0;
}

void clear(){
    /* 打开设备文件 */
    int lcd_fd = open(DEVICE_NAME, O_RDWR);
    if (0 > lcd_fd)
    {
        perror("ubuntu_lcd open error!");
        return -1;
    }

    /* 内存映射 */
    unsigned int *ptr = (unsigned int *)mmap(NULL, 800 * 480 * 4,
                                             PROT_READ | PROT_WRITE, MAP_SHARED, lcd_fd, 0);
    if (NULL == ptr)
    {
        perror("mmap error!");
        return -1;
    }

    bzero(ptr, 800 * 480 * 4);
}

void render_desktop_application()
{   
    clear();
    show_bmp("../static/desktop_img.bmp", 0, 0, 1);
    show_text("欢迎使用", 260, 130, 60, 0xF5F5F5);

    for (int i = 0; i < 6; i++)
    {
        applications[i].x = 10 + 130 * i;
        applications[i].y = 200;
        show_bmp(applications[i].path, 10 + 130 * i, 200, 0);
        show_text(applications[i].name, 40 + 130 * i, 320, 20, 0xF5F5F5);
    }
}

void listen_touch()
{

    int touch_fd = open(DEVICE_NAME, O_RDWR); // 模拟屏

    // int touch_fd = open("/dev/input/event0", O_RDWR);  //实物的
    if (0 > touch_fd)
    {
        perror("event open error");
        return -1;
    }
    struct input_event ts;

    int ts_x = -1, ts_y = -1;

    while (1)
    {
        /* 读取数据 */
        read(touch_fd, &ts, sizeof(ts));

        /* 分析数据 */
        if (EV_ABS == ts.type) // 判断类型是否为触摸屏类型
        {
            if (ABS_X == ts.code) // 判断X轴
            {
                ts_x = ts.value;
            }
            else if (ABS_Y == ts.code) // 判断Y轴
            {
                ts_y = ts.value;
            }
        }

        if (ts_x != -1 && ts_y != -1)
        {
            break;
        }
    }

    printf("x = %d, y = %d\n", ts_x, ts_y);

    if (lock_flag == 0)
    {
        for (int i = 0; i < 6; i++)
        {
            if (ts_x > applications[i].x && ts_x < applications[i].x + 100 && ts_y > applications[i].y && ts_y < applications[i].y + 100)
            {
                printf("x = %d, y = %d\n", ts_x, ts_y);
                applications[i].func();
            }
        }
    }
    else
    {
        lock_flag = 0;
        // show_desktop();
        render_desktop_application();
    }
    /* 收尾操作 */
    close(touch_fd);
}