
#include "common.h"
// #include "lcd.h"
#include "XUI.h"
#include "usart.h"

#define ch_input_count 42
const char input_map_index[ch_input_count][5] =
	{
		"ADC1",
		"ADC2",
		"ADC3",
		"ADC4",
		" S1 ",
		" S2 ",
		" SA ",
		" SB ",
		" SC ",
		" SD ",
		"PPM1",
		"PPM2",
		"PPM3",
		"PPM4",
		"PPM5",
		"PPM6",
		"PPM7",
		"PPM8",
		"SBS1",
		"SBS2",
		"SBS3",
		"SBS4",
		"SBS5",
		"SBS6",
		"SBS7",
		"SBS8",
		"SBS9",
		"SB10",
		"SB11",
		"SB12",
		"SB13",
		"SB14",
		"SB15",
		"SB16",
		"LG01",
		"LG02",
		"LG03",
		"LG04",
		"LG05",
		"LG06",
		"LG07",
		"LG08",
};

u8 menu_y = 0;
u8 py = 0, px = 0;
u8 menu_len = 0;
u8 param_edit = 0;

#define channel_count 16  // 通道总个数，总列数
#define channel_x_count 6 // 通道一列显示的参数个数
#define channel_y_count 4 // 一页显示的列数

void draw_channel_info(u8 i)
{
	u8 back_color = BC, point_color = PC;
	u8 seq = menu_y + i;
	u8 mode = 1;
	if (py == seq)
	{
		if (param_edit == 0)
		{
			back_color = SC;
			point_color = PC;
		}
		else
		{
			back_color = PC;
			point_color = BC;
		}
		mode = 0;
	}

	if (seq < 9)
	{
		XUI_str(0, 32 + i * 12, 24, 12, (u8 *)"CH0", 12, px == 0 ? mode : 1, px == 0 ? point_color : PC, px == 0 ? back_color : BC);
		XUI_num(18, 32 + i * 12, 1 + seq, 1, 12, px == 0 ? mode : 1, px == 0 ? point_color : PC, px == 0 ? back_color : BC);
	}
	else
	{
		XUI_str(0, 32 + i * 12, 24, 12, (u8 *)"CH", 12, px == 0 ? mode : 1, px == 0 ? point_color : PC, px == 0 ? back_color : BC);
		XUI_num(12, 32 + i * 12, 1 + seq, 2, 12, px == 0 ? mode : 1, px == 0 ? point_color : PC, px == 0 ? back_color : BC);
	}

	XUI_str(27, 32 + i * 12, 24, 12, (u8 *)input_map_index[ch_map[seq]], 12, px == 1 ? mode : 1, px == 1 ? point_color : PC, px == 1 ? back_color : BC);

	XUI_num(54, 32 + i * 12, ch_min[seq], 3, 12, px == 2 ? mode : 1, px == 2 ? point_color : PC, px == 2 ? back_color : BC);

	XUI_num(93 - len(ch_mid[seq]) * 3, 32 + i * 12, ch_mid[seq], len(ch_mid[seq]), 12, px == 3 ? mode : 1, px == 3 ? point_color : PC, px == 3 ? back_color : BC);

	XUI_num(108, 32 + i * 12, ch_max[seq], 3, 12, px == 4 ? mode : 1, px == 4 ? point_color : PC, px == 4 ? back_color : BC);
	XUI_num(135, 32 + i * 12, CH_input[seq], 4, 12, px == 5 ? mode : 1, px == 5 ? point_color : PC, px == 5 ? back_color : BC);
}
void input_setup() // 信号输入界面
{
	u8 i;
	XUI_str(56, 4, 64, 12, (u8 *)"信号输入", 12, 1, PC, RC);
	XUI_str(0, 20, 24, 12, (u8 *)"通道", 12, 1, RC, BC);
	XUI_str(27, 20, 24, 12, (u8 *)"输入", 12, 1, RC, BC);
	XUI_str(54, 20, 24, 12, (u8 *)"低位", 12, 1, RC, BC);
	XUI_str(81, 20, 24, 12, (u8 *)"中位", 12, 1, RC, BC);
	XUI_str(108, 20, 24, 12, (u8 *)"高位", 12, 1, RC, BC);
	XUI_str(135, 20, 24, 12, (u8 *)"数据", 12, 1, RC, BC);
	if (first_splash == 1)
	{
		first_splash = 0;
		if (channel_count > channel_y_count)
			menu_len = channel_y_count;
		else
			menu_len = channel_count;
		px = py = 0;
		menu_y = 0;
	}

	for (i = 0; i < menu_len; i++) // 全部刷新
		draw_channel_info(i);

	if (LEFT)
	{
		LEFT = 0;
		if (px)
			px--;
		else
			px = channel_x_count - 1;
	}
	if (RIGHT)
	{
		RIGHT = 0;
		if (px < (channel_x_count - 1))
			px++;
		else
			px = 0;
	}

	if (param_edit)
	{
		if (ESC || OK)
		{
			ESC = 0;
			OK = 0;
			param_edit = 0;

			save16(model_addr + py, ch_map[py]);
			save16(model_addr + 16 + py, ch_min[py]);
			save16(model_addr + 32 + py, ch_mid[py]);
			save16(model_addr + 48 + py, ch_max[py]);
		}
		if (UP)
		{
			switch (px)
			{
			case 1:
				if (ch_map[py] < (ch_input_count - 1))
					ch_map[py]++;
				else
					ch_map[py] = 0;
				break;
			case 2:
				if (ch_min[py] < 250)
					ch_min[py] += UP;
				if (ch_min[py] > 250)
					ch_min[py] = 250;
				break;
			case 3:
				if (ch_mid[py] < 250)
					ch_mid[py] += UP;
				if (ch_mid[py] > 250)
					ch_mid[py] = 250;
				break;
			case 4:
				if (ch_max[py] < 250)
					ch_max[py] += UP;
				if (ch_max[py] > 250)
					ch_max[py] = 250;
				break;
			}
			UP = 0;
		}

		if (DOWN)
		{
			switch (px)
			{
			case 1:
				if (ch_map[py])
					ch_map[py]--;
				else
					ch_map[py] = ch_input_count - 1;
				break;
			case 2:
				if (ch_min[py] > -250)
					ch_min[py] -= DOWN;
				if (ch_min[py] < -250)
					ch_min[py] = -250;
				break;
			case 3:
				if (ch_mid[py] > -250)
					ch_mid[py] -= DOWN;
				if (ch_mid[py] < -250)
					ch_mid[py] = -250;
				break;
			case 4:
				if (ch_max[py] > -250)
					ch_max[py] -= DOWN;
				if (ch_max[py] < -250)
					ch_max[py] = -250;
				break;
			}
			DOWN = 0;
		}
	}
	else
	{
		if (UP)
		{
			UP = 0;
			if (py)
				py--;
			else
			{
				py = channel_count - 1;
				if (channel_count > channel_y_count)
					menu_y = channel_count - channel_y_count;
				else
					menu_y = 0;
			}
			if (py < menu_y)
				menu_y--;
		}
		if (DOWN)
		{
			DOWN = 0;
			py++;
			if (py >= channel_count)
			{
				py = 0;
				menu_y = 0;
			}
			else if (py >= (menu_y + channel_y_count))
				menu_y++;
		}
		if (ESC)
		{
			ESC = 0;
			ui_menu_flag = 0;
		}
		if (OK)
		{
			OK = 0;
			param_edit = 1;
		}
	}
	index_bar(channel_count, 4, menu_y, 32, 80);
}
