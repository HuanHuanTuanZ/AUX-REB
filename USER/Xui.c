
#include "xuan.h"
#include "common.h"
// #include "lcd.h"

#include "ips.h"
#include "dma.h"

#define lcd_w 160
#define lcd_h 80

//				PC=WHITE8;
//				BC=BLUE1;
//				RC=BLUE3;
//				SC=BLUE2;
//				FC=BLUE5;

/////////////////////////////////////////////////定义的数组
const u16 color8_index[] =
	{
		0xFFFF,
		0x0000,
		0x001F,
		0XF81F,
		0XFFE0,
		0X07FF,
		0xF800,
		0xF81F,
		0x07E0,
		0x7FFF,
		0xFFE0,
		0XBC40,
		0XFC07,
		0X8430,
		0X01CF,
		0X7D7C,
		0X5458,
		0X841F,
		0XC618,
		0XA651,
		0X2B12,
		0x9732,
		0x0300,

		0xffff,
		0x1a13,
		0x00ea,
		0x645b,
		0x955b,

		0xffff,
		0x0000,
		0x3186,
		0x8410,
		0xdefb,
		0x0000,
		0xffff,
		0x8410,
		0xdefb,
		0x4208,
		0xffff,
		0xe0a2,
		0x7820,
		0xf30c,
		0xf4f3,
		0xffff,
		0xe3a2,
		0x79c0,
		0xf52c,
		0xf633,
		0xffff,
		0xe722,
		0x7bc0,
		0xf7ac,
		0xf7b3,
		0xffff,
		0x15c2,
		0x0300,
		0x5f2b,
		0x9732,
		0xffff,
		0x0c51,
		0x0249,
		0x56ba,
		0x8eba, //
		0xffff,
		0x68b3,
		0x302a,
		0xb2fb,
		0xc49b,
};

extern const u8 ascii_0608[][6];
extern const u8 ascii_1206[][12];
extern const u8 ascii_1608[][16];
extern const u8 ascii_2412[][48];
extern const u8 ascii_3216[][64];

u8 len(int t)
{
	if (t < 0)
		t = -t;
	if (t > 999999999)
		return 10;
	else if (t > 99999999)
		return 9;
	else if (t > 9999999)
		return 8;
	else if (t > 999999)
		return 7;
	else if (t > 99999)
		return 6;
	else if (t > 9999)
		return 5;
	else if (t > 999)
		return 4;
	else if (t > 99)
		return 3;
	else if (t > 9)
		return 2;
	else
		return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////

#if DMA == 0
u8 gram[lcd_w * lcd_h];
#else
u8 gram[2 * 12800];
#endif

void XUI_clear(u8 c)
{
#if DMA == 0
	u8 *xs = gram;
	u32 count = lcd_w * lcd_h;
	while (count--)
		*xs++ = c;
#else
	u8 *xs = gram;
	u32 count = lcd_w * lcd_h;
	while (count--)
	{
		*xs++ = color8_index[c] >> 8;
		*xs++ = color8_index[c];
	}
#endif
}

int16 x_offset = 0;
int16 y_offset = 0;

void XUI_point(int16 x, int16 y, u8 color)
{
#if USE_animation == 1
	x += x_offset;
	y += y_offset;
#endif

	if (x < 0 || x >= lcd_w || y >= lcd_h || y < 0)
		return;

#if DMA == 0
	gram[y * lcd_w + x] = color;
#else
	gram[(y * lcd_w + x) * 2] = color8_index[color] >> 8;
	gram[(y * lcd_w + x) * 2 + 1] = color8_index[color];
#endif
}

void XUI_point2(int16 x, int16 y, u8 color)
{

	if (x < 0 || x >= lcd_w || y >= lcd_h || y < 0)
		return;
#if DMA == 0
	gram[y * lcd_w + x] = color;
#else
	gram[(y * lcd_w + x) * 2] = color8_index[color] >> 8;
	gram[(y * lcd_w + x) * 2 + 1] = color8_index[color];
#endif
}

extern void delay(int t);
void LCD_Writ_Bus2(u8 dat)
{
	// while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);//检查接收标志位
	SPI_I2S_SendData(SPI2, dat);
	// delay(6);
}
void LCD_WR_DATA2(u16 dat)
{
	LCD_Writ_Bus2(dat >> 8);
	LCD_Writ_Bus2(dat);
}

void XUI_reflash()
{
#if DMA == 0
	u16 i = 0;
	int sum = lcd_w * lcd_h;
	LCD_Address_Set(0, 0, lcd_w - 1, lcd_h - 1);
	LCD_CS_Clr();
	for (i = 0; i < sum; i++)
	{
		SPI_I2S_SendData(SPI2, color8_index[gram[i]] >> 8);
		__nop();
		__nop();
		__nop();
		__nop();
		__nop();
		__nop();
		__nop();
		__nop();
		SPI_I2S_SendData(SPI2, color8_index[gram[i]]);
		__nop();
		__nop();
		__nop();
		__nop();
		__nop();
		__nop();
		__nop();
		__nop();
	}
	LCD_CS_Set();
#else
	LCD_Address_Set(0, 0, 159, 79);
	LCD_CS_Clr();
	SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE); // 使能串口1的DMA发送
	MYDMA_Enable(DMA1_Channel5);					 // 开始一次DMA传输！
	while (1)
	{
		if (DMA_GetFlagStatus(DMA1_FLAG_TC5) != RESET) // 判断通道4传输完成
		{
			DMA_ClearFlag(DMA1_FLAG_TC5); // 清除通道4传输完成标志
			break;
		}
	}
#endif
}

#if DMA
void XUI_dma_init()
{
	MYDMA_Config(DMA1_Channel5, (u32)&SPI2->DR, (u32)gram, 2 * 12800); // DMA1通道4,外设为串口1,存储器为SendBuff,长度SEND_BUF_SIZE.
}
#endif

const u16 suoyin[256] = {
	0, 8, 16, 24, 256, 264, 272, 280, 512, 520, 528, 536, 768, 776, 784, 792, 1024, 1032, 1040, 1048, 1280, 1288, 1296, 1304, 1536, 1544, 1552, 1560, 1792, 1800, 1808, 1816,
	8192, 8200, 8208, 8216, 8448, 8456, 8464, 8472, 8704, 8712, 8720, 8728, 8960, 8968, 8976, 8984, 9216, 9224, 9232, 9240, 9472, 9480, 9488, 9496, 9728, 9736, 9744, 9752, 9984, 9992, 10000, 10008,
	16384, 16392, 16400, 16408, 16640, 16648, 16656, 16664, 16896, 16904, 16912, 16920, 17152, 17160, 17168, 17176, 17408, 17416, 17424, 17432, 17664, 17672, 17680, 17688, 17920, 17928, 17936, 17944, 18176, 18184, 18192, 18200,
	24576, 24584, 24592, 24600, 24832, 24840, 24848, 24856, 25088, 25096, 25104, 25112, 25344, 25352, 25360, 25368, 25600, 25608, 25616, 25624, 25856, 25864, 25872, 25880, 26112, 26120, 26128, 26136, 26368, 26376, 26384, 26392, //
	32768, 32776, 32784, 32792, 33024, 33032, 33040, 33048, 33280, 33288, 33296, 33304, 33536, 33544, 33552, 33560, 33792, 33800, 33808, 33816, 34048, 34056, 34064, 34072, 34304, 34312, 34320, 34328, 34560, 34568, 34576, 34584, //
	40960, 40968, 40976, 40984, 41216, 41224, 41232, 41240, 41472, 41480, 41488, 41496, 41728, 41736, 41744, 41752, 41984, 41992, 42000, 42008, 42240, 42248, 42256, 42264, 42496, 42504, 42512, 42520, 42752, 42760, 42768, 42776,
	49152, 49160, 49168, 49176, 49408, 49416, 49424, 49432, 49664, 49672, 49680, 49688, 49920, 49928, 49936, 49944, 50176, 50184, 50192, 50200, 50432, 50440, 50448, 50456, 50688, 50696, 50704, 50712, 50944, 50952, 50960, 50968,
	57344, 57352, 57360, 57368, 57600, 57608, 57616, 57624, 57856, 57864, 57872, 57880, 58112, 58120, 58128, 58136, 58368, 58376, 58384, 58392, 58624, 58632, 58640, 58648, 58880, 58888, 58896, 58904, 59136, 59144, 59152, 65535};

// void XUI_reflash_suoyin()
//{
//	u16 i=0;
//	LCD_Address_Set(0,0,159,79);
//	for(i=0;i<12800;i++)
//	{
//		LCD_WR_DATA(suoyin[gram[i]]);
//	}
// }

void XUI_reflash_part(u8 x, u8 y, u8 dx, u8 dy)
{
	u16 i = 0, j = 0;
	LCD_Address_Set(x, y, x + dx - 1, y + dy - 1);
	LCD_CS_Clr();
	for (i = y; i < (y + dy); i++)
		for (j = x; j < (x + dx); j++)

#if DMA == 0
		{
			SPI_I2S_SendData(SPI2, color8_index[gram[i * lcd_w + j]] >> 8);
			__nop();
			__nop();
			__nop();
			__nop();
			__nop();
			__nop();
			__nop();
			__nop();
			SPI_I2S_SendData(SPI2, color8_index[gram[i * lcd_w + j]]);
			__nop();
			__nop();
			__nop();
			__nop();
			__nop();
			__nop();
			__nop();
			__nop();
		}
#else
		{
			SPI_I2S_SendData(SPI2, gram[(i * lcd_w + j) * 2]);
			SPI_I2S_SendData(SPI2, gram[(i * lcd_w + j) * 2 + 1]);
			delay_us(1);
		}
#endif
	LCD_CS_Set();
}

void XUI_fill(u8 sx, u8 sy, u8 ex, u8 ey, u8 color)
{
	u16 x, y;
	for (y = sy; y <= ey; y++)
	{
		for (x = sx; x <= ex; x++)
		{
			XUI_point(x, y, color);
		}
	}
}

void XUI_line_l(int16 x, int16 sy, int16 ey, u8 color)
{
	int16 dis = abs(ey - sy);
	int16 t = 0;
	if (sy > ey)
	{
		ey = sy;
		sy -= dis;
	}
	for (t = 0; t <= dis; t++)
	{
		XUI_point(x, sy + t, color); // 画点
	}
}

// void XUI_line_l(u8 x, u8 y1 ,u8 y2,u8 color)
//{
//	u8 t=0;
//	for(t=y1;t<y2;t++)
//	{
//		XUI_point(x,t,color);
//	}
// }

void XUI_line_horizontal(int16 x1, int16 x2, int16 y, u8 color)
{
	int16 t = 0;
	if (x1 > x2)
	{
		int16 temp = x1;
		x1 = x2;
		x2 = temp;
	}
	for (t = x1; t < x2; t++)
		XUI_point(t, y, color);
}

void XUI_circle(u16 x0, u16 y0, u8 r, u8 color)
{
	int a, b;
	int di;
	a = 0;
	b = r;
	di = 3 - (r << 1); // 判断下个点位置的标志
	while (a <= b)
	{
		XUI_point(x0 - b, y0 - a, color); // 3
		XUI_point(x0 + b, y0 - a, color); // 0
		XUI_point(x0 - a, y0 + b, color); // 1
		XUI_point(x0 - b, y0 - a, color); // 7
		XUI_point(x0 - a, y0 - b, color); // 2
		XUI_point(x0 + b, y0 + a, color); // 4
		XUI_point(x0 + a, y0 - b, color); // 5
		XUI_point(x0 + a, y0 + b, color); // 6
		XUI_point(x0 - b, y0 + a, color);
		a++;
		// 使用Bresenham算法画圆
		if (di < 0)
			di += 4 * a + 6;
		else
		{
			di += 10 + 4 * (a - b);
			b--;
		}
		XUI_point(x0 + a, y0 + b, color);
	}
}

void XUI_line(int16 sx, int16 sy, int16 ex, int16 ey, u8 color)
{
	u16 t;
	int xerr = 0, yerr = 0, delta_x, delta_y, distance;
	int incx, incy, uRow, uCol;
	delta_x = ex - sx; // 计算坐标增量
	delta_y = ey - sy;
	uRow = sx;
	uCol = sy;
	if (delta_x > 0)
		incx = 1; // 设置单步方向
	else if (delta_x == 0)
		incx = 0; // 垂直线
	else
	{
		incx = -1;
		delta_x = -delta_x;
	}
	if (delta_y > 0)
		incy = 1;
	else if (delta_y == 0)
		incy = 0; // 水平线
	else
	{
		incy = -1;
		delta_y = -delta_y;
	}
	if (delta_x > delta_y)
		distance = delta_x; // 选取基本增量坐标轴
	else
		distance = delta_y;
	for (t = 0; t <= distance + 1; t++) // 画线输出
	{
		XUI_point(uRow, uCol, color); // 画点
		xerr += delta_x;
		yerr += delta_y;
		if (xerr > distance)
		{
			xerr -= distance;
			uRow += incx;
		}
		if (yerr > distance)
		{
			yerr -= distance;
			uCol += incy;
		}
	}
}

void XUI_hline(short int x0, short int y0, u16 len, u16 color)
{
	// if(len==0||y0>320||y0<=80)return;
	XUI_fill(x0, y0, x0 + len - 1, y0, color);
}

void XUI_fill_circle(int16 x0, u16 y0, u16 r, u16 color)
{
	u32 i;
	u32 imax = ((u32)r * 707) / 1000 + 1;
	u32 sqmax = (u32)r * (u32)r + (u32)r / 2;
	u32 x = r;
	XUI_hline(x0 - r, y0, 2 * r, color);
	for (i = 1; i <= imax; i++)
	{
		if ((i * i + x * x) > sqmax) // draw lines from outside
		{
			if (x > imax)
			{
				XUI_hline(x0 - i + 1, y0 + x, 2 * (i - 1), color);
				XUI_hline(x0 - i + 1, y0 - x, 2 * (i - 1), color);
			}
			x--;
		}
		XUI_hline(x0 - x, y0 + i, 2 * x, color);
		XUI_hline(x0 - x, y0 - i, 2 * x, color);
	}
}

// 画矩形
void XUI_rectangle(u16 x1, u16 y1, u16 x2, u16 y2, u8 color)
{
	XUI_line(x1, y1, x2, y1, color);
	XUI_line(x1, y1, x1, y2, color);
	XUI_line(x1, y2, x2, y2, color);
	XUI_line(x2, y1, x2, y2, color);
}

void XUI_bar(u8 x0, u8 y0, u8 dx, u8 dy, u8 r, u8 color)
{
	int a, b;
	int di;
	u8 x, y;
	x = dx - r;
	y = dy - r;
	a = 0;
	b = r;
	di = 3 - (r << 1); // 茞讖袀俣郑位變謩要志
	while (a <= b)
	{
		XUI_line(x0 + a + x, y0 - b - y, x0 + a + x, y0 + b + y, color);
		XUI_line(x0 + b + x, y0 - a - y, x0 + b + x, y0 + a + y, color);
		XUI_line(x0 - a - x, y0 - b - y, x0 - a - x, y0 + b + y, color);
		XUI_line(x0 - b - x, y0 - a - y, x0 - b - x, y0 + a + y, color);
		a++;
		// 使詢Bresenham蹋专郗圆
		if (di < 0)
			di += 4 * a + 6;
		else
		{
			di += 10 + 4 * (a - b);
			b--;
		}
	}
	XUI_fill(x0 - x, y0 - dy, x0 + x, y0 + dy, color);
}

void XUI_bar0(u16 x0, u16 y0, u16 dx, u16 dy, u8 r, u16 color)
{
	int a, b;
	int di;
	u8 x, y;
	x = dx - r;
	y = dy - r;
	a = 0;
	b = r;
	di = 3 - (r << 1); // 茞讖袀俣郑位變謩要志
	while (a <= b)
	{
		XUI_point(x0 + a + x, y0 - b - y, color); // 5
		XUI_point(x0 + b + x, y0 - a - y, color); // 0
		XUI_point(x0 + b + x, y0 + a + y, color); // 4
		XUI_point(x0 + a + x, y0 + b + y, color); // 6
		XUI_point(x0 - a - x, y0 + b + y, color); // 1
		XUI_point(x0 - b - x, y0 + a + y, color);
		XUI_point(x0 - a - x, y0 - b - y, color); // 2
		XUI_point(x0 - b - x, y0 - a - y, color); // 7
		a++;
		// 使詢Bresenham蹋专郗圆
		if (di < 0)
			di += 4 * a + 6;
		else
		{
			di += 10 + 4 * (a - b);
			b--;
		}
	}
	XUI_line(x0 - x, y0 - dy, x0 + x, y0 - dy, color);
	XUI_line(x0 - x, y0 + dy, x0 + x, y0 + dy, color);
	XUI_line(x0 - dx, y0 - y, x0 - dx, y0 + y, color);
	XUI_line(x0 + dx, y0 - y, x0 + dx, y0 + y, color);
}

#if DMA == 1

void XUI_point_color(int16 x, int16 y, u16 color)
{
#if USE_animation == 1
	x += x_offset;
	y += y_offset;
#endif
	if (x < 0 || x >= lcd_w || y >= lcd_h || y < 0)
		return;

	gram[(y * lcd_w + x) * 2] = color >> 8;
	gram[(y * lcd_w + x) * 2 + 1] = color;
}

void XUI_bmp8_160_80(const u8 *pic)
{
	int i;
	for (i = 0; i < 25600;)
	{
		gram[i++] = suoyin[*pic] >> 8;
		gram[i++] = suoyin[*pic++];
	}
}

void XUI_bmp8(int16 sx, int16 sy, int16 width, int16 height, const u8 *pic, u8 mode)
{
	int16 ex = sx + width - 1;
	int16 ey = sy + height - 1;
	int16 x, y;
	if (mode == 0)
	{
		for (y = sy; y <= ey; y++)
		{
			for (x = sx; x <= ex; x++)
			{
				XUI_point_color(x, y, suoyin[*pic++]);
			}
		}
	}
	else // 屏蔽黑色模式
	{
		for (y = sy; y <= ey; y++)
		{
			for (x = sx; x <= ex; x++)
			{
				if (*pic++)
					XUI_point_color(x, y, suoyin[*pic]);
			}
		}
	}
}

void XUI_bmp16_160_80(const u8 *pic)
{
	int i = 0;
	for (i = 0; i < 25600; i += 2)
	{
		gram[i + 1] = *pic++;
		gram[i] = *pic++;
	}
}

void XUI_bmp16(int16 sx, int16 sy, int16 width, int16 height, const u8 *pic, u8 mode)
{
	int16 ex = sx + width - 1;
	int16 ey = sy + height - 1;
	int16 x, y;
	u16 color;
	u8 c1;
	u8 c2;
	if (mode == 0)
	{
		for (y = sy; y <= ey; y++)
		{
			for (x = sx; x <= ex; x++)
			{
				c1 = *pic++;
				c2 = *pic++;
				color = c2 << 8 | c1;
				XUI_point_color(x, y, color);
			}
		}
	}
	else
	{
		for (y = sy; y <= ey; y++)
		{
			for (x = sx; x <= ex; x++)
			{
				c1 = *pic++;
				c2 = *pic++;
				color = c2 << 8 | c1;
				if (color)
					XUI_point_color(x, y, color);
			}
		}
	}
}

#endif

void XUI_bmp1(int16 x, int16 y, int16 dx, int16 dy, const u8 *p, u8 mode, u16 color1, u16 color2)
{
	u16 i, j, k;
	u8 temp;
	for (j = 0; j < dy; j++)
	{
		for (i = 0; i < dx / 8; i++)
		{
			temp = *p;
			for (k = 0; k < 8; k++)
			{
				if (temp & 0x80)
					XUI_point(x + i * 8 + k, y + j, color1);
				else if (mode == 0)
					XUI_point(x + i * 8 + k, y + j, color2);
				temp <<= 1;
			}
			p++;
		}
	}
}

void XUI_char(int16 x, int16 y, u8 num, u8 sizey, u8 mode, u8 color1, u8 color2)
{
	u16 i, TypefaceNum; // 一个字符所占字节大小
	u8 x0 = x, temp, t;
	u8 sizex = sizey / 2;
	TypefaceNum = (sizex / 8 + ((sizex % 8) ? 1 : 0)) * sizey;
	num = num - ' '; // 得到偏移后的值
	for (i = 0; i < TypefaceNum; i++)
	{
		if (sizey == 12)
			temp = ascii_1206[num][i]; // 调用6x12字体
		else if (sizey == 16)
			temp = ascii_1608[num][i]; // 调用8x16字体
		else if (sizey == 24)
			temp = ascii_2412[num][i]; // 调用12x24字体
		else if (sizey == 32)
			temp = ascii_3216[num][i]; // 调用16x32字体
		else
			return;
		for (t = 0; t < 8; t++)
		{
			if (!mode) // 非叠加模式
			{
				if (temp & (0x01 << t))
					XUI_point(x, y, color1); // gram[y*160+x]=color1;
				else
					XUI_point(x, y, color2);
				x++;
				if ((x - x0) == sizex)
				{
					x = x0;
					y++;
					break;
				}
			}
			else // 叠加模式
			{
				if (temp & (0x01 << t))
					XUI_point(x, y, color1);
				x++;
				if ((x - x0) == sizex)
				{
					x = x0;
					y++;
					break;
				}
			}
		}
	}
}

void XUI_char_center(int16 x, int16 y, u8 num, u8 sizey, u8 mode, u8 color1, u8 color2)
{
	x = x - sizey / 4;
	y = y - sizey / 2;
	XUI_char(x, y, num, sizey, mode, color1, color2);
}

// m^n函数
// 返回值:m^n次方.
u32 LCD_Pow(u8 m, u8 n)
{
	u32 result = 1;
	while (n--)
		result *= m;
	return result;
}

void XUI_num0(u16 x, u16 y, int num, u8 len, u8 size, u8 mode, u8 color1, u8 color2)
{
	u8 t, temp;
	u8 enshow = 0;
	for (t = 0; t < len; t++)
	{
		temp = (num / LCD_Pow(10, len - t - 1)) % 10;
		if (enshow == 0 && t < (len - 1))
		{
			if (temp == 0)
			{
				XUI_char(x + (size / 2) * t, y, '0', size, mode, color1, color2);
				continue;
			}
			else
				enshow = 1;
		}
		XUI_char(x + (size / 2) * t, y, temp + '0', size, mode, color1, color2);
	}
}

void XUI_num(u16 x, u16 y, int num, u8 len, u8 size, u8 mode, u8 color1, u8 color2)
{
	u8 t, temp;
	u8 enshow = 0;
	u8 flag = 1;
	if (num >= 0)
	{
		for (t = 0; t < len; t++)
		{
			temp = (num / LCD_Pow(10, len - t - 1)) % 10;
			if (enshow == 0 && t < (len - 1))
			{
				if (temp == 0)
				{
					XUI_char(x + (size / 2) * t, y, ' ', size, mode, color1, color2);
					continue;
				}
				else
					enshow = 1;
			}
			XUI_char(x + (size / 2) * t, y, temp + '0', size, mode, color1, color2);
		}
	}
	else
	{
		num = -num;
		for (t = 0; t < len; t++)
		{
			temp = (num / LCD_Pow(10, len - t - 1)) % 10;
			if (enshow == 0 && t < (len - 1))
			{
				if (temp == 0)
				{
					XUI_char(x + size / 2 * (len - 1), y, ' ', size, mode, color1, color2);
					continue;
				}
				else
					enshow = 1;
			}
			if (flag)
			{
				flag = 0;
				XUI_char(x + (size / 2) * t, y, '-', size, mode, color1, color2);
				x += size / 2;
			}
			XUI_char(x + (size / 2) * t, y, temp + '0', size, mode, color1, color2);
		}
	}
}

#include "lcdfont.h"

void XUI_font12(int16 x, int16 y, u8 *font, u8 mode, u8 fc, u8 bc)
{
	u8 i, j;
	u16 k;
	u16 HZnum;		 // 汉字数目
	u16 TypefaceNum; // 一个字符所占字节大小
	int16 x0 = x;
	TypefaceNum = (12 / 8 + ((12 % 8) ? 1 : 0)) * 12;
	HZnum = sizeof(tfont12) / sizeof(typFNT_GB12); // 统计汉字数目
	for (k = 0; k < HZnum; k++)
	{
		if ((tfont12[k].Index[0] == *(font)) && (tfont12[k].Index[1] == *(font + 1)))
		{
			for (i = 0; i < TypefaceNum; i++)
			{
				for (j = 0; j < 8; j++)
				{
					if (tfont12[k].Msk[i] & (0x01 << j))
						XUI_point(x, y, fc);
					else if (!mode)
						XUI_point(x, y, bc);
					x++;
					if ((x - x0) == 12)
					{
						x = x0;
						y++;
						break;
					}
				}
			}
			break;
		}
	}
}
void XUI_font16(int16 x, int16 y, u8 *font, u8 mode, u8 fc, u8 bc)
{
	u8 i, j;
	u16 k;
	u16 HZnum;		 // 汉字数目
	u16 TypefaceNum; // 一个字符所占字节大小
	int16 x0 = x;
	TypefaceNum = (16 / 8 + ((16 % 8) ? 1 : 0)) * 16;
	HZnum = sizeof(tfont16) / sizeof(typFNT_GB16); // 统计汉字数目
	for (k = 0; k < HZnum; k++)
	{
		if ((tfont16[k].Index[0] == *(font)) && (tfont16[k].Index[1] == *(font + 1)))
		{
			for (i = 0; i < TypefaceNum; i++)
			{
				for (j = 0; j < 8; j++)
				{
					if (tfont16[k].Msk[i] & (0x01 << j))
						XUI_point(x, y, fc);
					else if (!mode)
						XUI_point(x, y, bc);
					x++;
					if ((x - x0) == 16)
					{
						x = x0;
						y++;
						break;
					}
				}
			}
			break;
		}
	}
}
void XUI_font24(int16 x, int16 y, u8 *font, u8 mode, u8 fc, u8 bc)
{
	u8 i, j;
	u16 k;
	u16 HZnum;		 // 汉字数目
	u16 TypefaceNum; // 一个字符所占字节大小
	int16 x0 = x;
	TypefaceNum = (24 / 8 + ((24 % 8) ? 1 : 0)) * 24;
	HZnum = sizeof(tfont24) / sizeof(typFNT_GB24); // 统计汉字数目
	for (k = 0; k < HZnum; k++)
	{
		if ((tfont24[k].Index[0] == *(font)) && (tfont24[k].Index[1] == *(font + 1)))
		{
			for (i = 0; i < TypefaceNum; i++)
			{
				for (j = 0; j < 8; j++)
				{
					if (tfont24[k].Msk[i] & (0x01 << j))
						XUI_point(x, y, fc);
					else if (!mode)
						XUI_point(x, y, bc);
					x++;
					if ((x - x0) == 24)
					{
						x = x0;
						y++;
						break;
					}
				}
			}
			break;
		}
	}
}
void XUI_font(int16 x, int16 y, u8 *font, u8 size, u8 mode, u8 fc, u8 bc)
{
	if (size == 12)
		XUI_font12(x, y, font, mode, fc, bc);
	else if (size == 16)
		XUI_font16(x, y, font, mode, fc, bc);
	else if (size == 24)
		XUI_font24(x, y, font, mode, fc, bc);
}

void XUI_str(int16 x, int16 y, u8 width, u8 height, u8 *str, u8 size, u8 mode, u8 color1, u8 color2)
{
	int16 x0 = x;
	int16 y0 = y;
	u8 bHz = 0;		  // 字符或者中文
	while (*str != 0) // 数据未结束
	{
		if (!bHz)
		{
			if (*str > 0x80)
				bHz = 1; // 中文
			else		 // 字符
			{
				if (x > (x0 + width - size / 2)) // 换行
				{
					y += size;
					x = x0;
				}
				if (y > (y0 + height - size))
					break;		// 越界返回
				if (*str == 13) // 换行符号
				{
					y += size;
					x = x0;
					str++;
				}
				else
					XUI_char(x, y, *str, size, mode, color1, color2); // 有效部分写入
				str++;
				x += size / 2; // 字符,为全字的一半
			}
		}
		else // 中文
		{
			bHz = 0;					 // 有汉字库
			if (x > (x0 + width - size)) // 换行
			{
				y += size;
				x = x0;
			}
			if (y > (y0 + height - size))
				break;										 // 越界返回
			XUI_font(x, y, str, size, mode, color1, color2); // 显示这个汉字,空心显示
			str += 2;
			x += size; // 下一个汉字偏移
		}
	}
}

#include "math.h"

const signed char xx[44] = {0, -1, -2, -2, -4, -5, -15, -15, -13, -6, -9, -9, -6, -3, -3, -2, -1, -1, 0, 1, 1, 2, 3, 3, 6, 9, 9, 6, 13, 15, 15, 5, 4, 2, 2, 1, -3, -6, -6, -3, 3, 6, 6, 3};
const signed char yy[44] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -5, -5, 0, 0, -5, -5, 0};
const signed char zz[44] = {-20, -18, -14, -10, -8, -2, 7, 9, 11, 13, 16, 19, 20, 17, 15, 16, 15, 17, 18, 17, 15, 16, 15, 17, 20, 19, 16, 13, 11, 9, 7, -2, -8, -10, -14, -18, 8, 11, 14, 16, 8, 11, 14, 16};
const u8 ss[44] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44};
const u8 ee[44] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 1, 38, 39, 40, 37, 42, 43, 44, 41};

void XUI_point_ss(u8 x, u8 y, u8 color)
{
	if (x <= x_min || x >= x_max)
		return;
	if (y <= y_min || y >= y_max)
		return;
	x += x_offset;
	y += y_offset;
	if (y >= lcd_h || x >= lcd_w)
		return;

#if DMA == 0
	gram[y * lcd_w + x] = color;
#else
	gram[(y * lcd_w + x) * 2] = color8_index[color] >> 8;
	gram[(y * lcd_w + x) * 2 + 1] = color8_index[color];
#endif
}

void XUI_line_ss(u8 sx, u8 sy, u8 ex, u8 ey, u8 color)
{
	u16 t;
	int xerr = 0, yerr = 0, delta_x, delta_y, distance;
	int incx, incy, uRow, uCol;
	delta_x = ex - sx; // 计算坐标增量
	delta_y = ey - sy;
	uRow = sx;
	uCol = sy;
	if (delta_x > 0)
		incx = 1; // 设置单步方向
	else if (delta_x == 0)
		incx = 0; // 垂直线
	else
	{
		incx = -1;
		delta_x = -delta_x;
	}
	if (delta_y > 0)
		incy = 1;
	else if (delta_y == 0)
		incy = 0; // 水平线
	else
	{
		incy = -1;
		delta_y = -delta_y;
	}
	if (delta_x > delta_y)
		distance = delta_x; // 选取基本增量坐标轴
	else
		distance = delta_y;
	for (t = 0; t <= distance + 1; t++) // 画线输出
	{
		XUI_point_ss(uRow, uCol, color); // 画点
		xerr += delta_x;
		yerr += delta_y;
		if (xerr > distance)
		{
			xerr -= distance;
			uRow += incx;
		}
		if (yerr > distance)
		{
			yerr -= distance;
			uCol += incy;
		}
	}
}

void XUI_3d(float rotx, float roty, float rotz, u8 big, u8 c) //?????,???
{
	static u8 i;
	u16 sx, sy, ex, ey;
	u16 newx[44];
	u16 newy[44];
	float xt, yt, zt, x, y, z, sinax, cosax, sinay, cosay, sinaz, cosaz;

	sinax = sin(rotx);		 // precalculate the sin and cos values
	cosax = cos(rotx);		 // for the rotation as this saves a
	sinay = sin(roty);		 // little time when running as we
	cosay = cos(roty);		 // call sin and cos less often
	sinaz = sin(rotz);		 // they are slow routines
	cosaz = cos(rotz);		 // and we dont want slow
	for (i = 0; i < 44; i++) // translate 3d vertex position to 2d screen position
	{
		x = xx[i];					// get x for vertex i
		y = yy[i];					// get y for vertex i
		z = zz[i];					// get z for vertex i
		yt = y * cosax - z * sinax; // rotate around the x axis
		zt = y * sinax + z * cosax; // using the Y and Z for the rotation
		y = yt;
		z = zt;
		xt = x * cosay - z * sinay; // rotate around the Y axis
		zt = x * sinay + z * cosay; // using X and Z
		x = xt;
		z = zt;
		xt = x * cosaz - y * sinaz; // finaly rotate around the Z axis
		yt = x * sinaz + y * cosaz; // using X and Y
		x = xt;
		y = yt;
		z = z + 30;							// as well as Z
		newx[i] = (x * big / z) + x_center; // translate 3d to 2d coordinates for screen
		newy[i] = (y * big / z) + y_center; // drawing so we can see the cube
	}
	for (i = 0; i < 44; i++) // draw the lines that make up the object
	{
		sx = newx[ss[i] - 1];			// set line start x to vertex x position
		sy = newy[ss[i] - 1];			// set line start y to vertex y position
		ex = newx[ee[i] - 1];			// set line end x to vertex[i+1] x position
		ey = newy[ee[i] - 1];			// set line end y to vertex[i+1] y position
		XUI_line_ss(sx, sy, ex, ey, c); //???????????
	}
}

const unsigned char asc2_0608[95][6] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // sp
	0x00, 0x00, 0x00, 0x2f, 0x00, 0x00, // !
	0x00, 0x00, 0x07, 0x00, 0x07, 0x00, // "
	0x00, 0x14, 0x7f, 0x14, 0x7f, 0x14, // #
	0x00, 0x24, 0x2a, 0x7f, 0x2a, 0x12, // $
	0x00, 0x62, 0x64, 0x08, 0x13, 0x23, // %
	0x00, 0x36, 0x49, 0x55, 0x22, 0x50, // &
	0x00, 0x00, 0x05, 0x03, 0x00, 0x00, // '
	0x00, 0x00, 0x1c, 0x22, 0x41, 0x00, // (
	0x00, 0x00, 0x41, 0x22, 0x1c, 0x00, // )
	0x00, 0x14, 0x08, 0x3E, 0x08, 0x14, // *
	0x00, 0x08, 0x08, 0x3E, 0x08, 0x08, // +
	0x00, 0x00, 0x00, 0xA0, 0x60, 0x00, // ,
	0x00, 0x08, 0x08, 0x08, 0x08, 0x08, // -
	0x00, 0x00, 0x60, 0x60, 0x00, 0x00, // .
	0x00, 0x20, 0x10, 0x08, 0x04, 0x02, // /
	0x00, 0x3E, 0x51, 0x49, 0x45, 0x3E, // 0
	0x00, 0x00, 0x42, 0x7F, 0x40, 0x00, // 1
	0x00, 0x42, 0x61, 0x51, 0x49, 0x46, // 2
	0x00, 0x21, 0x41, 0x45, 0x4B, 0x31, // 3
	0x00, 0x18, 0x14, 0x12, 0x7F, 0x10, // 4
	0x00, 0x27, 0x45, 0x45, 0x45, 0x39, // 5
	0x00, 0x3C, 0x4A, 0x49, 0x49, 0x30, // 6
	0x00, 0x01, 0x71, 0x09, 0x05, 0x03, // 7
	0x00, 0x36, 0x49, 0x49, 0x49, 0x36, // 8
	0x00, 0x06, 0x49, 0x49, 0x29, 0x1E, // 9
	0x00, 0x00, 0x36, 0x36, 0x00, 0x00, // :
	0x00, 0x00, 0x56, 0x36, 0x00, 0x00, // ;
	0x00, 0x08, 0x14, 0x22, 0x41, 0x00, // <
	0x00, 0x14, 0x14, 0x14, 0x14, 0x14, // =
	0x00, 0x00, 0x41, 0x22, 0x14, 0x08, // >
	0x00, 0x02, 0x01, 0x51, 0x09, 0x06, // ?
	0x00, 0x32, 0x49, 0x59, 0x51, 0x3E, // @
	0x00, 0x7C, 0x12, 0x11, 0x12, 0x7C, // A
	0x00, 0x7F, 0x49, 0x49, 0x49, 0x36, // B
	0x00, 0x3E, 0x41, 0x41, 0x41, 0x22, // C
	0x00, 0x7F, 0x41, 0x41, 0x22, 0x1C, // D
	0x00, 0x7F, 0x49, 0x49, 0x49, 0x41, // E
	0x00, 0x7F, 0x09, 0x09, 0x09, 0x01, // F
	0x00, 0x3E, 0x41, 0x49, 0x49, 0x7A, // G
	0x00, 0x7F, 0x08, 0x08, 0x08, 0x7F, // H
	0x00, 0x00, 0x41, 0x7F, 0x41, 0x00, // I
	0x00, 0x20, 0x40, 0x41, 0x3F, 0x01, // J
	0x00, 0x7F, 0x08, 0x14, 0x22, 0x41, // K
	0x00, 0x7F, 0x40, 0x40, 0x40, 0x40, // L
	0x00, 0x7F, 0x02, 0x0C, 0x02, 0x7F, // M
	0x00, 0x7F, 0x04, 0x08, 0x10, 0x7F, // N
	0x00, 0x3E, 0x41, 0x41, 0x41, 0x3E, // O
	0x00, 0x7F, 0x09, 0x09, 0x09, 0x06, // P
	0x00, 0x3E, 0x41, 0x51, 0x21, 0x5E, // Q
	0x00, 0x7F, 0x09, 0x19, 0x29, 0x46, // R
	0x00, 0x46, 0x49, 0x49, 0x49, 0x31, // S
	0x00, 0x01, 0x01, 0x7F, 0x01, 0x01, // T
	0x00, 0x3F, 0x40, 0x40, 0x40, 0x3F, // U
	0x00, 0x1F, 0x20, 0x40, 0x20, 0x1F, // V
	0x00, 0x3F, 0x40, 0x38, 0x40, 0x3F, // W
	0x00, 0x63, 0x14, 0x08, 0x14, 0x63, // X
	0x00, 0x07, 0x08, 0x70, 0x08, 0x07, // Y
	0x00, 0x61, 0x51, 0x49, 0x45, 0x43, // Z
	0x00, 0x00, 0x7F, 0x41, 0x41, 0x00, // [
	0x00, 0x03, 0x0C, 0x30, 0xC0, 0x00, //
	0x00, 0x00, 0x41, 0x41, 0x7F, 0x00, // ]
	0x00, 0x04, 0x02, 0x01, 0x02, 0x04, // ^
	0x00, 0x40, 0x40, 0x40, 0x40, 0x40, // _
	0x00, 0x00, 0x01, 0x02, 0x04, 0x00, // '
	0x00, 0x20, 0x54, 0x54, 0x54, 0x78, // a
	0x00, 0x7F, 0x48, 0x44, 0x44, 0x38, // b
	0x00, 0x38, 0x44, 0x44, 0x44, 0x20, // c
	0x00, 0x38, 0x44, 0x44, 0x48, 0x7F, // d
	0x00, 0x38, 0x54, 0x54, 0x54, 0x18, // e
	0x00, 0x08, 0x7E, 0x09, 0x01, 0x02, // f
	0x00, 0x18, 0xA4, 0xA4, 0xA4, 0x7C, // g
	0x00, 0x7F, 0x08, 0x04, 0x04, 0x78, // h
	0x00, 0x00, 0x44, 0x7D, 0x40, 0x00, // i
	0x00, 0x40, 0x80, 0x84, 0x7D, 0x00, // j
	0x00, 0x7F, 0x10, 0x28, 0x44, 0x00, // k
	0x00, 0x00, 0x41, 0x7F, 0x40, 0x00, // l
	0x00, 0x7C, 0x04, 0x18, 0x04, 0x78, // m
	0x00, 0x7C, 0x08, 0x04, 0x04, 0x78, // n
	0x00, 0x38, 0x44, 0x44, 0x44, 0x38, // o
	0x00, 0xFC, 0x24, 0x24, 0x24, 0x18, // p
	0x00, 0x18, 0x24, 0x24, 0x18, 0xFC, // q
	0x00, 0x7C, 0x08, 0x04, 0x04, 0x08, // r
	0x00, 0x48, 0x54, 0x54, 0x54, 0x20, // s
	0x00, 0x04, 0x3F, 0x44, 0x40, 0x20, // t
	0x00, 0x3C, 0x40, 0x40, 0x20, 0x7C, // u
	0x00, 0x1C, 0x20, 0x40, 0x20, 0x1C, // v
	0x00, 0x3C, 0x40, 0x30, 0x40, 0x3C, // w
	0x00, 0x44, 0x28, 0x10, 0x28, 0x44, // x
	0x00, 0x1C, 0xA0, 0xA0, 0xA0, 0x7C, // y
	0x00, 0x44, 0x64, 0x54, 0x4C, 0x44, // z
	0x00, 0x18, 0xFF, 0x81, 0x00, 0x00, //{
	0x00, 0x00, 0xFE, 0xFE, 0x00, 0x00, //|
	0x00, 0x00, 0x81, 0x7E, 0x18, 0x00, //}
	0x18, 0x18, 0x18, 0x18, 0x18, 0x18, // ——
};

void XUI_char8(int16 x, int16 y, u8 p, u8 mode, u8 color1, u8 color2)
{
	u8 j, temp, t;
	y += 8;
	p = p - ' ';
	for (j = 0; j < 6; j++)
	{
		temp = asc2_0608[p][j];
		for (t = 0; t < 8; t++)
		{
			if (temp & 0x80)
				XUI_point(x, y, color1);
			else if (mode == 0)
				XUI_point(x, y, color2);
			y--;
			temp <<= 1;
		}
		y += 8;
		x++;
	}
}

void XUI_num8(int16 x, int16 y, int num, u8 len, u8 mode, u8 color1, u16 color2)
{
	u8 t, temp;
	u8 enshow = 0;
	if (num < 0)
	{
		num = -num;
		XUI_char8(x, y, '-', mode, color1, color2);
		x += 6;
	}
	for (t = 0; t < len; t++)
	{
		temp = (num / LCD_Pow(10, len - t - 1)) % 10;
		if (enshow == 0 && t < (len - 1))
		{
			if (temp == 0)
			{
				XUI_char8(x, y, ' ', mode, color1, color2);
				x += 6;
				continue;
			}
			else
				enshow = 1;
		}
		XUI_char8(x, y, temp + 48, mode, color1, color2);
		x += 6;
	}
}

void XUI_string8(int16 x, int16 y, u8 *p, u8 mode, u16 color1, u16 color2)
{
	u8 t, j, k = 0;
	u8 temp;
	y += 8;
	while (p[k] != '\0')
	{
		for (j = 0; j < 6; j++)
		{
			temp = asc2_0608[(p[k] - 32)][j];
			for (t = 0; t < 8; t++)
			{
				if (temp & 0x80)
					XUI_point(x, y, color1);
				else if (mode == 0)
					XUI_point(x, y, color2);
				y--;
				temp <<= 1;
			}
			y += 8;
			x++;
		}
		k++;
	}
}

void XUI_num16_8(u16 x, u16 y, u32 num, u8 len, u8 mode, u16 color1, u16 color2)
{
	u8 t, temp;
	u8 enshow = 0;
	// LCD_ShowChar_color(x,y,'0',size,0,color1,color2);
	// LCD_ShowChar_color(x+size/2,y,'x',size,0,color1,color2);
	// x+=size;
	for (t = 0; t < len; t++)
	{
		temp = (num / LCD_Pow(16, len - t - 1)) % 16;
		if (enshow == 0 && t < (len - 1))
		{
			if (temp == 0)
			{
				XUI_char8(x + 6 * t, y, '0', mode, color1, color2);
				continue;
			}
			else
				enshow = 1;
		}
		if (temp == 10)
			XUI_char8(x + 6 * t, y, 'A', mode, color1, color2);
		else if (temp == 11)
			XUI_char8(x + 6 * t, y, 'B', mode, color1, color2);
		else if (temp == 12)
			XUI_char8(x + 6 * t, y, 'C', mode, color1, color2);
		else if (temp == 13)
			XUI_char8(x + 6 * t, y, 'D', mode, color1, color2);
		else if (temp == 14)
			XUI_char8(x + 6 * t, y, 'E', mode, color1, color2);
		else if (temp == 15)
			XUI_char8(x + 6 * t, y, 'F', mode, color1, color2);
		else
			XUI_char8(x + 6 * t, y, temp + '0', mode, color1, color2);
	}
}

/******************************************************************************
	  函数说明：在指定区域填充颜色
	  入口数据：xsta,ysta   起始坐标
				xend,yend   终止坐标
								color       要填充的颜色
	  返回值：  无
******************************************************************************/
void LCD_Fill(u16 xsta, u16 ysta, u16 xend, u16 yend, u16 color)
{
	u16 i, j;
	LCD_Address_Set(xsta, ysta, xend - 1, yend - 1); // 设置显示范围
	for (i = ysta; i < yend; i++)
	{
		for (j = xsta; j < xend; j++)
		{
			LCD_WR_DATA(color);
		}
	}
}
