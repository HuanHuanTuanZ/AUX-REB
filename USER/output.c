#include "common.h"
// #include "lcd.h"
#include "XUI.h"
// #include "usart.h"

#define output_count 16
#define output_x_count 6
#define output_y_count 4

#define ch_output_count 56
const char output_map_index[][5] =
	{
		"MX01",
		"MX02",
		"MX03",
		"MX04",
		"MX05",
		"MX06",
		"MX07",
		"MX08",
		"MX09",
		"MX10",
		"MX11",
		"MX12",
		"MX13",
		"MX14",
		"MX15",
		"MX16",
		"CH01",
		"CH02",
		"CH03",
		"CH04",
		"CH05",
		"CH06",
		"CH07",
		"CH08",
		"CH09",
		"CH10",
		"CH11",
		"CH12",
		"CH13",
		"CH14",
		"CH15",
		"CH16",
		"ADC1",
		"ADC2",
		"ADC3",
		"ADC4",
		"ADC5",
		"ADC6",
		"ADC7",
		"ADC8",
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

void output()
{
	u8 i;
	u16 temp;
	for (i = 0; i < 16; i++)
	{
		if (output_map[i] < 16)
			temp = mix_value[output_map[i]]; // 混控
		else if (output_map[i] < 32)
			temp = CH_input[output_map[i] - 16]; // 外部输入
		else if (output_map[i] < 40)
			temp = hardware_value[output_map[i] - 32]; // ADC输入
		else if (output_map[i] < 48)
			temp = ppm_value[output_map[i] - 40]; // PPM
		else if (output_map[i] < 64)
			temp = sbus_value[output_map[i] - 48]; // SBUS
		else if (ch_map[i] < 72)
			temp = 1500;

		if (temp >= 1500)
			CH_out[i] = 1500 + (temp - 1500) * output_max[i] / 100; // 通道输出比例运算
		else
			CH_out[i] = 1500 - (1500 - temp) * output_min[i] / 100; // 通道输出比例运算
	}
}

void draw_output_info(u8 i)
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
		XUI_str(0, 32 + i * 12, 24, 12, (u8 *)"OT0", 12, px == 0 ? mode : 1, px == 0 ? point_color : PC, px == 0 ? back_color : BC);
		XUI_num(18, 32 + i * 12, 1 + seq, 1, 12, px == 0 ? mode : 1, px == 0 ? point_color : PC, px == 0 ? back_color : BC);
	}
	else
	{
		XUI_str(0, 32 + i * 12, 24, 12, (u8 *)"OT", 12, px == 0 ? mode : 1, px == 0 ? point_color : PC, px == 0 ? back_color : BC);
		XUI_num(12, 32 + i * 12, 1 + seq, 2, 12, px == 0 ? mode : 1, px == 0 ? point_color : PC, px == 0 ? back_color : BC);
	}
	XUI_str(27, 32 + i * 12, 24, 12, (u8 *)output_map_index[output_map[seq]], 12, px == 1 ? mode : 1, px == 1 ? point_color : PC, px == 1 ? back_color : BC);
	XUI_num(57, 32 + i * 12, output_min[seq], 3, 12, px == 2 ? mode : 1, px == 2 ? point_color : PC, px == 2 ? back_color : BC);
	if (ch_rev[seq] == 0)
		XUI_str(87, 32 + i * 12, 24, 12, (u8 *)"正", 12, px == 3 ? mode : 1, px == 3 ? point_color : PC, px == 3 ? back_color : BC);
	else
		XUI_str(87, 32 + i * 12, 24, 12, (u8 *)"反", 12, px == 3 ? mode : 1, px == 3 ? point_color : PC, px == 3 ? back_color : BC);
	XUI_num(108, 32 + i * 12, output_max[seq], 3, 12, px == 4 ? mode : 1, px == 4 ? point_color : PC, px == 4 ? back_color : BC);
	XUI_num(135, 32 + i * 12, CH_out[seq], 4, 12, px == 5 ? mode : 1, px == 5 ? point_color : PC, px == 5 ? back_color : BC);
}

void output_setup() // 通道输出界面
{
	u8 i;
	XUI_str(56, 4, 64, 12, (u8 *)"通道输出", 12, 1, PC, BC);
	XUI_str(0, 20, 24, 12, (u8 *)"通道", 12, 1, RC, BC);
	XUI_str(27, 20, 24, 12, (u8 *)"输入", 12, 1, RC, BC);
	XUI_str(54, 20, 24, 12, (u8 *)"负比", 12, 1, RC, BC);
	XUI_str(81, 20, 24, 12, (u8 *)"正反", 12, 1, RC, BC);
	XUI_str(108, 20, 24, 12, (u8 *)"正比", 12, 1, RC, BC);
	XUI_str(135, 20, 24, 12, (u8 *)"数据", 12, 1, RC, BC);
	if (first_splash)
	{
		first_splash = 0;
		if (output_count > output_y_count)
			menu_len = output_y_count;
		else
			menu_len = output_count;
		px = py = 0;
		menu_y = 0;
	}
	for (i = 0; i < menu_len; i++) // 全部刷新
		draw_output_info(i);

	if (LEFT)
	{
		LEFT = 0;
		if (px)
			px--;
		else
			px = output_x_count - 1;
	}
	if (RIGHT)
	{
		RIGHT = 0;
		if (px < (output_x_count - 1))
			px++;
		else
			px = 0;
	}
	if (param_edit)
	{
		if (ESC == 1 || OK == 1)
		{
			ESC = 0;
			OK = 0;
			param_edit = 0;
			save16(model_addr + 80 + py, output_map[py]);
			save16(model_addr + 96 + py, output_min[py]);
			save16(model_addr + 112 + py, ch_rev[py]);
			save16(model_addr + 128 + py, output_max[py]);
		}
		if (UP)
		{
			switch (px)
			{
			case 1:
				if (output_map[py] < (ch_output_count - 1))
					output_map[py]++;
				else
					output_map[py] = 0;
				break;
			case 2:
				if (output_min[py] < 150)
					output_min[py] += UP;
				if (output_min[py] > 150)
					output_min[py] = 150;
				break;
			case 3:
				ch_rev[py] = !ch_rev[py];
				break;
			case 4:
				if (output_max[py] < 150)
					output_max[py] += UP;
				if (output_max[py] > 150)
					output_max[py] = 150;
				break;
			}
			UP = 0;
		}
		if (DOWN)
		{
			switch (px)
			{
			case 1:
				if (output_map[py])
					output_map[py]--;
				else
					output_map[py] = ch_output_count - 1;
				break;
			case 2:
				if (output_min[py] > 50)
					output_min[py] -= DOWN;
				if (output_min[py] < 50)
					output_min[py] = 50;
				break;
			case 3:
				ch_rev[py] = !ch_rev[py];
				break;
			case 4:
				if (output_max[py] > 50)
					output_max[py] -= DOWN;
				if (output_max[py] < 50)
					output_max[py] = 50;
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
				py = output_count - 1;
				if (output_count > output_y_count)
					menu_y = output_count - output_y_count;
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
			if (py >= output_count)
			{
				py = 0;
				menu_y = 0;
			}
			else if (py >= (menu_y + output_y_count))
				menu_y++;
		}
		if (ESC == 1)
		{
			ESC = 0;
			ui_menu_flag = 0;
		}
		if (OK == 1)
		{
			OK = 0;
			param_edit = 1;
		}
	}
}
