#include "common.h"
// #include "lcd.h"
#include "XUI.h"
// #include "usart.h"

#define mix_x_count 6
#define mix_y_count 4

#define mixer_type_count 7
const char mixer_index[][4] =
	{
		"----",
		"常规",
		"三角",
		"差速",
		"V尾 ",
		"叠中",
		"叠低",
};

#define mixer_src_count 6
const char mixer_src_index[][5] =
	{
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

int16 mixer_source(u8 seq)
{
	if (seq <= 16)
		return CH_input[seq];
	else if (seq <= 24)
		return hardware_value[seq - 16];
	else if (seq <= 32)
		return ppm_value[seq - 24];
	else if (seq <= 48)
		return sbus_value[seq - 32];
	else if (seq <= 56)
		return 1500;
	else
		return 0;
}

u16 mix_value[16] = {1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500};
u8 mixer_type[mixer_count] = {0, 0, 0, 0};
u8 mixer_in1[mixer_count] = {0, 0, 0, 0};
u8 mixer_in2[mixer_count] = {1, 1, 1, 1};
int16 mixer_rate[mixer_count] = {-10, 10, -20, 0};

void mix()
{
	u8 i;
	u16 in1;
	u16 in2;
	for (i = 0; i < 16; i++)
		mix_value[i] = CH_input[i];

	for (i = 0; i < mixer_count; i++)
	{
		in1 = mixer_source(mixer_in1[i]);
		in2 = mixer_source(mixer_in2[i]);
		switch (mixer_type[i])
		{
		case 2:
			mix_value[mixer_in1[i]] = ((in1 - 1500) + (in2 - 1500)) / 2 + 1500;
			mix_value[mixer_in2[i]] = 1500 - ((in2 - 1500) - (in1 - 1500)) / 2;
			break;
		case 3:
			mix_value[mixer_in1[i]] = ((in1 - 1500) + (in2 - 1500)) / 2 + 1500;
			mix_value[mixer_in2[i]] = 1500 - ((in2 - 1500) - (in1 - 1500)) / 2;
			break;
		case 4:
			mix_value[mixer_in1[i]] = ((in1 - 1500) + (in2 - 1500)) / 2 + 1500;
			mix_value[mixer_in2[i]] = 1500 - ((in2 - 1500) - (in1 - 1500)) / 2;
			break;
		case 5:
			mix_value[mixer_in1[i]] = in1 + mixer_rate[i] * (in2 - 1500) / 100;
			break;
		case 6:
			mix_value[mixer_in1[i]] = in1 + mixer_rate[i] * (in2 - 1000) / 100;
			break;
		default:
			break;
		}
		if (mix_value[mixer_in1[i]] > 2500)
			mix_value[mixer_in1[i]] = 2500;
		if (mix_value[mixer_in1[i]] < 500)
			mix_value[mixer_in1[i]] = 500;
		if (mix_value[mixer_in2[i]] > 2500)
			mix_value[mixer_in2[i]] = 2500;
		if (mix_value[mixer_in2[i]] < 500)
			mix_value[mixer_in2[i]] = 500;
	}
}

void draw_mixer_info(u8 i)
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
	XUI_str(0, 32 + i * 12, 24, 12, (u8 *)"MX0", 12, px == 0 ? mode : 1, px == 0 ? point_color : PC, px == 0 ? back_color : BC);
	if (seq < 9)
		XUI_num(18, 32 + i * 12, 1 + seq, 1, 12, px == 0 ? mode : 1, px == 0 ? point_color : PC, px == 0 ? back_color : BC);
	else
		XUI_num(12, 32 + i * 12, 1 + seq, 2, 12, px == 0 ? mode : 1, px == 0 ? point_color : PC, px == 0 ? back_color : BC);
	XUI_str(27, 32 + i * 12, 24, 12, (u8 *)mixer_index[mixer_type[seq]], 12, px == 1 ? mode : 1, px == 1 ? point_color : PC, px == 1 ? back_color : BC);
	if (mixer_type[seq])
	{
		XUI_str(54, 32 + i * 12, 24, 12, (u8 *)mixer_src_index[mixer_in1[seq]], 12, px == 2 ? mode : 1, px == 2 ? point_color : PC, px == 2 ? back_color : BC);
		XUI_str(81, 32 + i * 12, 24, 12, (u8 *)mixer_src_index[mixer_in2[seq]], 12, px == 3 ? mode : 1, px == 3 ? point_color : PC, px == 3 ? back_color : BC);
		if (mixer_type[seq] == 5 || mixer_type[seq] == 6)
		{
			XUI_num(120 - len(mixer_rate[seq]) * 3, 32 + i * 12, mixer_rate[seq], len(mixer_rate[seq]), 12, px == 4 ? mode : 1, px == 4 ? point_color : PC, px == 4 ? back_color : BC);
			XUI_num(135, 32 + i * 12, mix_value[mixer_in1[seq]], 4, 12, px == 5 ? mode : 1, px == 5 ? point_color : PC, px == 5 ? back_color : BC);
		}
		else
		{
			XUI_num(108, 32 + i * 12, mix_value[mixer_in1[seq]], 4, 12, px == 4 ? mode : 1, px == 4 ? point_color : PC, px == 4 ? back_color : BC);
			XUI_num(135, 32 + i * 12, mix_value[mixer_in2[seq]], 4, 12, px == 5 ? mode : 1, px == 5 ? point_color : PC, px == 5 ? back_color : BC);
		}
	}
}

void mix_setup()
{
	u8 i;
	XUI_str(56, 4, 64, 12, (u8 *)"通道混控", 12, 1, PC, BC);
	XUI_str(0, 20, 24, 12, (u8 *)"序号", 12, 1, RC, BC);
	XUI_str(27, 20, 24, 12, (u8 *)"类型", 12, 1, RC, BC);

	if (first_splash)
	{
		first_splash = 0;
		if (mixer_count > mix_y_count)
			menu_len = mix_y_count;
		else
			menu_len = mixer_count;
		px = py = 0;
		menu_y = 0;
	}

	if (mixer_type[py] <= 4)
	{
		XUI_str(54, 20, 24, 12, (u8 *)"入1", 12, 1, RC, BC);
		XUI_str(81, 20, 24, 12, (u8 *)"入2", 12, 1, RC, BC);
		XUI_str(108, 20, 24, 12, (u8 *)"出1", 12, 1, RC, BC);
		XUI_str(135, 20, 24, 12, (u8 *)"出2", 12, 1, RC, BC);
	}
	else if (mixer_type[py] == 5 || mixer_type[py] == 6)
	{
		XUI_str(54, 20, 24, 12, (u8 *)"主入", 12, 1, RC, BC);
		XUI_str(81, 20, 24, 12, (u8 *)"辅入", 12, 1, RC, BC);
		XUI_str(108, 20, 24, 12, (u8 *)"比例", 12, 1, RC, BC);
		XUI_str(135, 20, 24, 12, (u8 *)"数据", 12, 1, RC, BC);
	}

	for (i = 0; i < menu_len; i++) // 全部刷新
		draw_mixer_info(i);

	if (LEFT)
	{
		LEFT = 0;
		if (px)
			px--;
		else
			px = mix_x_count - 1;

		if (mixer_type[py] == 0)
		{
			if (px > 1)
				px = 1;
		}
	}
	if (RIGHT)
	{
		RIGHT = 0;
		if (px < (mix_x_count - 1))
			px++;
		else
			px = 0;

		if (mixer_type[py] == 0)
		{
			if (px > 1)
				px = 0;
		}
	}
	if (param_edit)
	{
		if (ESC == 1 || OK == 1)
		{
			ESC = 0;
			OK = 0;
			param_edit = 0;

			save16(model_addr + 64 + py, mixer_type[py]);
			save16(model_addr + 68 + py, mixer_in1[py]);
			save16(model_addr + 72 + py, mixer_in2[py]);
			save16(model_addr + 76 + py, mixer_rate[py]);
		}
		if (UP)
		{
			switch (px)
			{
			case 1:
				if (mixer_type[py] < (mixer_type_count - 1))
					mixer_type[py]++;
				else
					mixer_type[py] = 0;
				break;
			case 2:
				if (mixer_in1[py] < (mixer_src_count - 1))
					mixer_in1[py]++;
				else
					mixer_in1[py] = 0;
				break;
			case 3:
				if (mixer_in2[py] < (mixer_src_count - 1))
					mixer_in2[py]++;
				else
					mixer_in2[py] = 0;
				break;
			case 4:
				if (mixer_type[py] == 5 || mixer_type[py] == 6)
				{
					mixer_rate[py] += UP;
					if (mixer_rate[py] >= 150)
						mixer_rate[py] = 150;
				}
				break;
			}
			UP = 0;
		}
		if (DOWN)
		{
			switch (px)
			{
			case 1:
				if (mixer_type[py])
					mixer_type[py]--;
				else
					mixer_type[py] = mixer_type_count - 1;
				break;
			case 2:
				if (mixer_in1[py])
					mixer_in1[py]--;
				else
					mixer_in1[py] = mixer_src_count - 1;
				break;
			case 3:
				if (mixer_in2[py])
					mixer_in2[py]--;
				else
					mixer_in2[py] = mixer_src_count - 1;
				break;
			case 4:
				if (mixer_type[py] == 5 || mixer_type[py] == 6)
				{
					mixer_rate[py] -= DOWN;
					if (mixer_rate[py] <= -150)
						mixer_rate[py] = -150;
				}
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
				py = mixer_count - 1;
				if (mixer_count > mix_y_count)
					menu_y = mixer_count - mix_y_count;
				else
					menu_y = 0;
			}
			if (py < menu_y)
				menu_y--;

			if (mixer_type[py] == 0)
			{
				if (px > 1)
					px = 1;
			}
		}
		if (DOWN)
		{
			DOWN = 0;
			py++;
			if (py >= mixer_count)
			{
				py = 0;
				menu_y = 0;
			}
			else if (py >= (menu_y + mix_y_count))
				menu_y++;
			if (mixer_type[py] == 0)
			{
				if (px > 1)
					px = 1;
			}
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
