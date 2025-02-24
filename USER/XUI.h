#ifndef __GRAM_H
#define __GRAM_H
#include "sys.h"

#define int16 short int

void XUI_dma_init(void);
void XUI_clear(u8 c);
void XUI_point(int16 x, int16 y, u8 color);
void XUI_point2(int16 x, int16 y, u8 color);
void XUI_line_l(int16 x, int16 sy, int16 ey, u8 color);
void XUI_line(int16 sx, int16 sy, int16 ex, int16 ey, u8 color);
void XUI_fill(u8 sx, u8 sy, u8 ex, u8 ey, u8 color);
void XUI_circle(u16 x0, u16 y0, u8 r, u8 color);
void XUI_fill_circle(int16 x0, u16 y0, u16 r, u16 color);
void XUI_rectangle(u16 x1, u16 y1, u16 x2, u16 y2, u16 color);
void XUI_bar(u8 x0, u8 y0, u8 dx, u8 dy, u8 r, u8 color);
void XUI_bar0(u16 x0, u16 y0, u16 dx, u16 dy, u8 r, u16 color);
void XUI_bmp1(u16 x, u16 y, u16 dx, u16 dy, const u8 *p, u8 mode, u16 color1, u16 color2);
void XUI_char(int16 x, int16 y, u8 num, u8 size, u8 mode, u8 color1, u8 color2);
void XUI_num(u16 x, u16 y, int num, u8 len, u8 size, u8 mode, u8 color1, u8 color2);
void XUI_num0(u16 x, u16 y, int num, u8 len, u8 size, u8 mode, u8 color1, u8 color2);
void XUI_font(u8 x, u8 y, u8 *font, u8 size, u8 mode, u8 color1, u8 color2);
void XUI_str(int16 x, int16 y, u8 width, u8 height, u8 *str, u8 size, u8 mode, u8 color1, u8 color2);
void XUI_reflash(void);
void XUI_reflash_part(u8 x, u8 y, u8 dx, u8 dy);
void XUI_reflash_suoyin(void);

void XUI_char_center(int16 x, int16 y, u8 num, u8 sizey, u8 mode, u8 color1, u8 color2);

void XUI_point_ss(u8 x, u8 y, u8 color);
void XUI_line_ss(u8 sx, u8 sy, u8 ex, u8 ey, u8 color);
void XUI_3d(float rotx, float roty, float rotz, u8 big, u8 c); //?????,???

void XUI_char8(int16 x, int16 y, u8 p, u8 mode, u8 color1, u8 color2);
void XUI_num8(int16 x, int16 y, u32 num, u8 len, u8 mode, u16 color1, u16 color2);
void XUI_string8(int16 x, int16 y, u8 *p, u8 mode, u16 color1, u16 color2);
void XUI_num16_8(u16 x, u16 y, u32 num, u8 len, u8 size, u16 color1, u16 color2);
void XUI_line_horizontal(int16 x1, int16 x2, int16 y, u8 color);

void XUI_bmp16(int16 sx, int16 sy, int16 width, int16 height, const u8 *pic, u8 mode);
void XUI_bmp8(int16 sx, int16 sy, int16 width, int16 height, const u8 *pic, u8 mode);

void LCD_Fill(u16 xsta, u16 ysta, u16 xend, u16 yend, u16 color); // 指定区域填充颜色

void XUI_bmp8_160_80(const u8 *pic);
void XUI_bmp16_160_80(const u8 *pic);

// 画笔颜色
#define WHITE 0xFFFF
#define BLACK 0x0000
#define BLUE 0x001F
#define BRED 0XF81F
#define GRED 0XFFE0
#define GBLUE 0X07FF
#define RED 0xF800
#define MAGENTA 0xF81F
#define GREEN 0x07E0
#define CYAN 0x7FFF
#define YELLOW 0xFFE0
#define BROWN 0XBC40      // 棕色
#define BRRED 0XFC07      // 棕红色
#define GRAY 0X8430       // 灰色
#define DARKBLUE 0X01CF   // 深蓝色
#define LIGHTBLUE 0X7D7C  // 浅蓝色
#define GRAYBLUE 0X5458   // 灰蓝色
#define LIGHTGREEN 0X841F // 浅绿色
#define LGRAY 0XC618      // 浅灰色(PANNEL),窗体背景色
#define LGRAYBLUE 0XA651  // 浅灰蓝色(中间层颜色)
#define LBBLUE 0X2B12     // 浅棕蓝色(选择条目的反色)

// 8位画笔颜色

#define WHITE8 0
#define BLACK8 1
#define BLUE8 2
#define BRED8 3
#define GRED8 4
#define GBLUE8 5
#define RED8 6
#define MAGENTA8 7
#define GREEN8 8
#define CYAN8 9
#define YELLOW8 10
#define BROWN8 11 // 棕色
#define BRRED8 12 // 棕红色
#define GRAY8 13  // 灰色
// GUI颜色
#define DARKBLUE8 14  // 深蓝色
#define LIGHTBLUE8 15 // 浅蓝色
#define GRAYBLUE8 16  // 灰蓝色
// 以上三色为PANEL的颜色
#define LIGHTGREEN8 17 // 浅绿色
#define LGRAY8 18      // 浅灰色(PANNEL),窗体背景色
#define LGRAYBLUE8 19  // 浅灰蓝色(中间层颜色)
#define LBBLUE8 20     // 浅棕蓝色(选择条目的反色)
#define LGREEN8 21
#define DARKGREEN8 22

#define CYAN1 23
#define CYAN2 24
#define CYAN3 25
#define CYAN4 26
#define CYAN5 27

#define WHITE1 28
#define WHITE2 29
#define WHITE3 30
#define WHITE4 31
#define WHITE5 32

#define BLACK1 33
#define BLACK2 34
#define BLACK3 35
#define BLACK4 36
#define BLACK5 37

#define YELLOW1 38
#define YELLOW2 39
#define YELLOW3 40
#define YELLOW4 41
#define YELLOW5 42

#define ORANGE1 43
#define ORANGE2 44
#define ORANGE3 45
#define ORANGE4 46
#define ORANGE5 47

#define RED1 48
#define RED2 49
#define RED3 50
#define RED4 51
#define RED5 52

#define PURPLE1 53
#define PURPLE2 54
#define PURPLE3 55
#define PURPLE4 56
#define PURPLE5 57

#define BLUE1 58
#define BLUE2 59
#define BLUE3 60
#define BLUE4 61
#define BLUE5 62

#define GREEN1 63
#define GREEN2 64
#define GREEN3 65
#define GREEN4 66
#define GREEN5 67

#endif
