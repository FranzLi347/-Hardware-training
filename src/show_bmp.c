#include "show_bmp.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <gif_lib.h>

#define FRAMEBUFFER "/dev/ubuntu_lcd"

/* bmp图片的路径 */
char *bmp_pathname[50] = {
    "./pic/show/0.bmp",
    "./pic/show/1.bmp",
    "./pic/show/2.bmp",
    "./pic/show/3.bmp",
    "./pic/show/4.bmp",
    "./pic/show/5.bmp",
    "./pic/show/6.bmp",
};

/* 资源文件的路径 */
char *source_bmp[10] = {"./pic/src/up.bmp",
                        "./pic/src/dowm.bmp",
                        "./pic/src/big.bmp",
                        "./pic/src/small.bmp",
                        "./pic/src/return.bmp",
                        "./pic/src/exit.bmp"};

// 大图缩小的显示方式
void big_to_small(unsigned int *ptr, unsigned int *lcd_buffer,
                  int bmp_height, int bmp_width, int start_x, int start_y)
{
    /* 显示颜色数据 */
    int l_col, l_row;
    int b_row, b_col;
    int num = ((bmp_width / 801 + 1) > (bmp_height / 481 + 1)) ? (bmp_width / 801 + 1) : (bmp_height / 481 + 1);
    for (l_row = 0, b_row = 0; b_row < bmp_height && l_row + start_y < 480; b_row++)
    {
        // 大图缩小的本质图片数据每次+n，屏幕显示数据才会+1
        if (b_row % num == 0)
        {
            l_row++;
        }
        for (l_col = 0, b_col = 0; b_col < bmp_width && l_col + start_x < 800; b_col++)
        {
            if (b_col % num == 0)
            {
                l_col++;
            }
            // 屏幕显示的内容      //处理之后的数据
            /*
                当col row 都为0
                *(ptr+(400+100)*800+0+100)
            */
            *(ptr + (l_row + start_y) * 800 + l_col + start_x) = lcd_buffer[b_row * bmp_width + b_col];
        }
    }
}

// 小图放大的显示方式
void small_to_big(unsigned int *ptr, unsigned int *lcd_buffer,
                  int bmp_height, int bmp_width, int start_x, int start_y)
{
    /* 显示颜色数据 */
    int l_col, l_row;
    int b_row, b_col;
    int num = ((800 / bmp_width) > (480 / bmp_height)) ? (800 / bmp_width) : (480 / bmp_height);
    for (l_row = 0, b_row = 0; b_row < bmp_height && l_row + start_y < 480; l_row++)
    {
        // 小图放大的本质屏幕显示数据每次+4，图片数据才会+1
        if (l_row % num == 0)
        {
            b_row++;
        }
        for (l_col = 0, b_col = 0; b_col < bmp_width && l_col + start_x < 800; l_col++)
        {

            if (l_col % num == 0)
            {
                b_col++;
            }
            // 屏幕显示的内容      //处理之后的数据
            /*
                当col row 都为0
                *(ptr+(400+100)*800+0+100)
            */
            *(ptr + (l_row + start_y) * 800 + l_col + start_x) = lcd_buffer[b_row * bmp_width + b_col];
        }
    }
}

// 正常显示的方式
void show_cur_size(unsigned int *ptr, unsigned int *lcd_buffer,
                   int bmp_height, int bmp_width, int start_x, int start_y)
{
    int row, col;
    for (row = 0; row < bmp_height && row + start_y < 480; row++)
    {
        for (col = 0; col < bmp_width && col + start_x < 800; col++)
        {
            // 屏幕显示的内容      //处理之后的数据
            /*
                当col row 都为0
                *(ptr+(400+100)*800+0+100)
            */
            *(ptr + (row + start_y) * 800 + col + start_x) = lcd_buffer[row * bmp_width + col];
        }
    }
}

/*
    pathname：需要显示的图片路径
*/
int show_bmp(char *pathname, int start_x, int start_y, int flag)
{
    /* 打开设备文件 /dev/ubuntu_lcd */
    int lcd_fd = open("/dev/ubuntu_lcd", O_RDWR); // 模拟器的设备文件

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
    // bzero(ptr, 800 * 480 * 4);

    /* 从图片中获取颜色数据 */
    int bmp_fd = open(pathname, O_RDWR);
    if (0 > bmp_fd)
    {
        perror("bmp open error!");
        return -1;
    }

    // 读取图片的信息：宽度+高度
    int bmp_height, bmp_width;
    lseek(bmp_fd, 18, SEEK_SET);
    read(bmp_fd, &bmp_width, 4);
    read(bmp_fd, &bmp_height, 4);
    // printf("bmp_width=%d, bmp_height=%d\n", bmp_width, bmp_height);

    unsigned char *bmp_buffer = (unsigned char *)malloc(bmp_width * bmp_width * 3);
    lseek(bmp_fd, 54, SEEK_SET);
    read(bmp_fd, bmp_buffer, bmp_width * bmp_width * 3);

    /* 对数据进行处理 */
    unsigned int *lcd_buffer = (unsigned int *)malloc(bmp_width * bmp_width * 4);
    int row, col;
    int lcd_num, bmp_num;
    for (row = 0; row < bmp_height; row++)
    {
        for (col = 0; col < bmp_width; col++)
        {
            // 处理之后的结果        待处理的数据
            lcd_num = (bmp_height - row - 1) * bmp_width + col;
            bmp_num = row * bmp_width + col;
            lcd_buffer[lcd_num] = bmp_buffer[3 * bmp_num + 0] << 0 | bmp_buffer[3 * bmp_num + 1] << 8 | bmp_buffer[3 * bmp_num + 2] << 16;
        }
    }

    // 清屏
    // bzero(ptr, 800*480*4);

    // 正常显示的效果
    if (0 == flag)
    {
        show_cur_size(ptr, lcd_buffer, bmp_height, bmp_width, start_x, start_y);
    }
    // 缩放显示的效果
    else if (1 == flag)
    {
        if (bmp_width > 800 || bmp_height > 480)
        {
            big_to_small(ptr, lcd_buffer, bmp_height, bmp_width, start_x, start_y);
        }
        else
        {
            small_to_big(ptr, lcd_buffer, bmp_height, bmp_width, start_x, start_y);
        }
    }

    /* 释放资源 */
    free(bmp_buffer);
    free(lcd_buffer);
    munmap(ptr, 800 * 480 * 4);
    close(lcd_fd);
    close(bmp_fd);
}

// 展示gif图片

void show_gif(const char *gif_path)
{
    int error;

    // 打开GIF文件
    GifFileType *gif = DGifOpenFileName(gif_path, &error);
    if (gif == NULL)
    {
        printf("Cannot open GIF file, error code: %d\n", error);
        return;
    }

    // 读取GIF文件
    if (DGifSlurp(gif) != GIF_OK)
    {
        printf("Cannot read GIF file\n");
        return;
    }

    // 打开Framebuffer设备
    int lcd_fd = open("/dev/ubuntu_lcd", O_RDWR);
    if (lcd_fd == -1)
    {
        printf("Cannot open framebuffer device\n");
        return;
    }

    // 映射Framebuffer到内存
    unsigned char *lcd_ptr = (unsigned char *)mmap(NULL, 800 * 480 * 4, PROT_READ | PROT_WRITE, MAP_SHARED, lcd_fd, 0);
    if (lcd_ptr == MAP_FAILED)
    {
        printf("Cannot map framebuffer device to memory\n");
        return;
    }

    // 遍历GIF的每一帧
    for (int i = 0; i < gif->ImageCount; i++)
    {
        SavedImage *frame = &gif->SavedImages[i];
        ColorMapObject *colorMap = frame->ImageDesc.ColorMap ? frame->ImageDesc.ColorMap : gif->SColorMap;

        // 检查是否有图形控制扩展
        int transparentColorIndex = -1;
        for (int j = 0; j < frame->ExtensionBlockCount; j++)
        {
            if (frame->ExtensionBlocks[j].Function == GRAPHICS_EXT_FUNC_CODE)
            {
                if (frame->ExtensionBlocks[j].Bytes[0] & 1)
                {
                    transparentColorIndex = (unsigned char)frame->ExtensionBlocks[j].Bytes[3];
                }
                break;
            }
        }

        // 将GIF帧写入到Framebuffer
        for (int y = 0; y < frame->ImageDesc.Height; y++)
        {
            for (int x = 0; x < frame->ImageDesc.Width; x++)
            {
                GifPixelType pixel = frame->RasterBits[y * frame->ImageDesc.Width + x];

                // 如果这个像素是透明的，跳过它
                if (pixel == transparentColorIndex)
                {
                    continue;
                }

                GifColorType color = colorMap->Colors[pixel];
                // 转换颜色空间
                unsigned int rgba = (0xFF << 24) | (color.Red << 16) | (color.Green << 8) | color.Blue;

                // 计算位置
                long location = (x + frame->ImageDesc.Left) * 4 + (y + frame->ImageDesc.Top) * 800 * 4;

                // 写入颜色值
                memcpy(lcd_ptr + location, &rgba, sizeof(rgba));
            }
        }

        // 等待下一帧
        usleep(100000);
    }

    // 清理
    bzero(lcd_ptr, 800 * 480 * 4);
    munmap(lcd_ptr, 800 * 480 * 4);
    close(lcd_fd);
    DGifCloseFile(gif, NULL);
}