#include "common.h"
// #include "lcd.h"
#include "XUI.h"
#include "usart.h"
#include "gpio.h"
#include "string.h"

#define model_count 3
#define model_x_count 2
#define model_y_count 5

// 大写字母键盘库
unsigned char jpk[48] = {49, 55, 37, 50, 52, 57, 53, 41, 47, 48, 28, 30, 60, 17, 18, 19,
						 33, 51, 36, 38, 39, 40, 42, 43, 44, 63, 59, 61, 29, 20, 21, 22,
						 96, 58, 56, 35, 54, 34, 46, 45, 27, 14, 11, 13, 16, 23, 24, 25};

u8 menu3 = 0;
u8 shift = 0;
extern const char sw_icon1[];
extern const char sw_icon2[];
extern const char sw_icon3[];

void draw_key(u8 seq)
{
	if (seq == 32)
	{
		if (shift)
		{
			XUI_bar((seq % 16) * 10 + 2 + 3, 38 + (seq / 16) * 14 + 6, 5, 7, 3, FC);
			XUI_bmp1((seq % 16) * 10 + 1, 38 + (seq / 16) * 14 + 2, 8, 8, (u8 *)sw_icon1, 1, BC, FC);
			XUI_bar0((seq % 16) * 10 + 2 + 3, 38 + (seq / 16) * 14 + 6, 5, 7, 3, PC);
		}
		else if (seq == (py * 16 + px))
		{
			XUI_bar((seq % 16) * 10 + 2 + 3, 38 + (seq / 16) * 14 + 6, 5, 7, 3, PC);
			XUI_bmp1((seq % 16) * 10 + 1, 38 + (seq / 16) * 14 + 2, 8, 8, (u8 *)sw_icon1, 1, BC, PC);
		}
		else
		{
			XUI_bmp1((seq % 16) * 10 + 1, 38 + (seq / 16) * 14 + 2, 8, 8, (u8 *)sw_icon1, 1, PC, BC);
			XUI_bar0((seq % 16) * 10 + 2 + 3, 38 + (seq / 16) * 14 + 6, 5, 7, 3, PC);
		}
	}
	else
	{
		if (seq == (py * 16 + px))
		{
			XUI_bar((seq % 16) * 10 + 2 + 3, 38 + (seq / 16) * 14 + 6, 5, 7, 3, PC);
			if ((jpk[seq] + 32) >= 65 && (jpk[seq] + 32) <= 90 && shift == 0)
				XUI_char((seq % 16) * 10 + 3, 38 + (seq / 16) * 14, jpk[seq] + 32 + 32, 12, 1, BC, PC);
			else
				XUI_char((seq % 16) * 10 + 3, 38 + (seq / 16) * 14, jpk[seq] + 32, 12, 1, BC, PC);
		}
		else
		{
			XUI_bar0((seq % 16) * 10 + 2 + 3, 38 + (seq / 16) * 14 + 6, 5, 7, 3, PC);
			if ((jpk[seq] + 32) >= 65 && (jpk[seq] + 32) <= 90 && shift == 0)
				XUI_char((seq % 16) * 10 + 3, 38 + (seq / 16) * 14, jpk[seq] + 32 + 32, 12, 1, PC, BC);
			else
				XUI_char((seq % 16) * 10 + 3, 38 + (seq / 16) * 14, jpk[seq] + 32, 12, 1, PC, BC);
		}
	}
}

u8 model_name_len = 0;
u8 ok_flag = 0;
u8 esc_flag = 0;
u8 key_flag = 0;
u8 key_flag2 = 0;

/////////////////////////////////////////////////////////////////////////////////////////
void model_rename()
{
	u8 i;
	if (first_splash)
	{
		first_splash = 0;
		px = py = 0;
		model_name_len = strlen(model_name);
	}
	XUI_str(56, 4, 64, 12, (u8 *)"模型命名", 12, 1, PC, BC);

	for (i = 0; i < 48; i++)
		draw_key(i);
	XUI_line(159, 39, 159, 49, PC);
	XUI_line(159, 53, 159, 63, PC);
	XUI_line(159, 67, 159, 77, PC);
	XUI_bar0(80, 28, 80, 7, 4, PC);
	XUI_line(159, 23, 159, 33, PC);

	XUI_str(80 - strlen(model_name) * 3, 22, 160, 12, (u8 *)model_name, 12, 1, PC, BC);

	if (UP)
	{
		UP = 0;
		if (py)
			py--;
		else
			py = 2;
	}
	if (DOWN)
	{
		DOWN = 0;
		if (py < 2)
			py++;
		else
			py = 0;
	}
	if (LEFT)
	{
		LEFT = 0;
		if (px)
			px--;
		else
			px = 15;
	}
	if (RIGHT)
	{
		RIGHT = 0;
		if (px < 15)
			px++;
		else
			px = 0;
	}

	if (ESC)
		key_flag2 = ESC;
	else
	{
		if (key_flag2 == 1)
			esc_flag = 1;
		key_flag2 = 0;
	}

	if (OK)
		key_flag = OK;
	else
	{
		if (key_flag == 1)
			ok_flag = 1;
		key_flag = 0;
	}

	if (ok_flag == 1)
	{
		ok_flag = 0;
		if ((py * 16 + px) == 32)
			shift = !shift;
		else
		{
			u8 n = jpk[py * 16 + px] + 32;
			if (n >= 65 && n <= 90 && shift == 0)
				model_name[model_name_len++] = jpk[py * 16 + px] + 32 + 32;
			else
				model_name[model_name_len++] = jpk[py * 16 + px] + 32;
		}
	}
	else if (esc_flag == 1)
	{
		esc_flag = 0;
		if (model_name_len)
		{
			model_name[model_name_len - 1] = '\0';
			model_name_len--;
		}
	}

	if (OK >= 20 || ESC >= 20)
	{
		menu3 = 0;
		OK = ESC = 0;
		model_name_save(model_addr + 164);
		first_splash = 1;
	}
}
/////////////////////////////////////////////////////////////////////////////////////////
u8 model_current = 0;
const char model_select_index[][8] = {
	" 模型01 ",
	" 模型02 ",
	" 模型03 ",
	" 模型04 ",
};
u8 sy = 0;
void select_menu(u8 *index, u8 len)
{
	u8 i;
	XUI_bar(80, 40, 30, len * 6 + 4, 4, BC);
	XUI_bar0(80, 40, 30, len * 6 + 4, 4, PC);
	for (i = 0; i < len; i++)
	{
		if (sy == i)
			XUI_str(56, 40 - len * 6 + i * 12, 48, 12, &index[i * 8], 12, 0, BC, PC);
		else
			XUI_str(56, 40 - len * 6 + i * 12, 48, 12, &index[i * 8], 12, 0, PC, BC);
	}
	if (UP)
	{
		UP = 0;
		if (sy)
			sy--;
		else
			sy = len - 1;
	}
	if (DOWN)
	{
		DOWN = 0;
		if (sy < (len - 1))
			sy++;
		else
			sy = 0;
	}
	if (OK)
	{
		OK = 0;
		model_current = sy;
		save16(2, model_current);
		model_read();
		menu3 = 0;
		sy = 0;
	}
	if (ESC)
	{
		ESC = 0;
		menu3 = 0;
		sy = 0;
	}
}
/////////////////////////////////////////////////////////////////////////////////////////
u8 check_ok(char *p)
{
	u8 len = strlen(p);
	u8 h = (len - 1) / 16 + 1;

	XUI_bar(80, 40 + 2, 54, (h + 1) * 6 + 4, 4, BC);
	XUI_bar0(80, 40 + 2, 54, (h + 1) * 6 + 4, 4, PC);
	if (len <= 16)
		XUI_str(80 - len * 3, 40 - (h + 1) * 6, 96, 60, (u8 *)p, 12, 0, PC, BC);
	else
		XUI_str(32, 40 - (h + 1) * 6, 96, 60, (u8 *)p, 12, 0, PC, BC);

	if (ESC)
	{
		XUI_bar(54, 40 + (h + 1) * 6 - 3, 24, 7, 4, PC);
		XUI_str(45, 40 + h * 6 - 2, 30, 12, (u8 *)"ESC", 12, 0, BC, PC);
	}
	else
	{
		XUI_bar0(54, 40 + (h + 1) * 6 - 3, 24, 7, 4, PC);
		XUI_str(45, 40 + h * 6 - 2, 30, 12, (u8 *)"ESC", 12, 0, PC, BC);
	}
	if (OK)
	{
		XUI_bar(106, 40 + (h + 1) * 6 - 3, 24, 7, 4, PC);
		XUI_str(100, 40 + h * 6 - 2, 30, 12, (u8 *)"OK", 12, 0, BC, PC);
	}
	else
	{
		XUI_bar0(106, 40 + (h + 1) * 6 - 3, 24, 7, 4, PC);
		XUI_str(100, 40 + h * 6 - 2, 30, 12, (u8 *)"OK", 12, 0, PC, BC);
	}
	if (OK_release == 1)
	{
		OK_release = 0;
		menu3 = 0;
		return 1;
	}
	if (ESC_release == 1)
	{
		ESC_release = 0;
		menu3 = 0;
		return 0;
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////
const char model_index1[][8] = {
	{"模型选择"},
	{"模型命名"},
	{"模型复位"},
	{"模型复制"},
};

const char model_index2[][8] = {
	{"--------"},
	{"--------"},
	{"--------"},
};

void draw_model_info1(u8 i)
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
	XUI_str(0, 20 + i * 12, 48, 12, (u8 *)model_index1[seq], 12, px == 0 ? mode : 1, px == 0 ? point_color : PC, px == 0 ? back_color : BC);
	switch (seq)
	{
	case 0:
		XUI_num(48, 20 + i * 12, model_current + 1, 5, 12, px == 0 ? mode : 1, px == 0 ? point_color : PC, px == 0 ? back_color : BC);
		break;
	case 1:
	{
		extern u8 str_t;
		u8 len = strlen(model_name);
		if (len <= 5)
			XUI_str(78 - len * 6, 20 + i * 12, 30, 12, (u8 *)model_name, 12, px == 0 ? mode : 1, px == 0 ? point_color : PC, px == 0 ? back_color : BC);
		else
		{
			XUI_str(48, 20 + i * 12, 30, 12, (u8 *)(model_name + str_t), 12, px == 0 ? mode : 1, px == 0 ? point_color : PC, px == 0 ? back_color : BC);
			if (ui_1hz)
				ui_1hz = 0, str_t++;
			if (str_t > (len - 5))
				str_t = 0;
		}
	}
	break;
	case 2:
		XUI_str(48, 20 + i * 12, 30, 12, (u8 *)"   OK", 12, px == 0 ? mode : 1, px == 0 ? point_color : PC, px == 0 ? back_color : BC);
		break;

		// default:XUI_num(48,20+i*12,param[0][seq],5,12,0,RC,back_color);break;
	}
}

void draw_model_info2(u8 i)
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
	XUI_str(80, 20 + i * 12, 48, 12, (u8 *)model_index2[seq], 12, px == 1 ? mode : 1, px == 1 ? point_color : PC, px == 1 ? back_color : BC);
	switch (seq)
	{

		// default:XUI_num(128,20+i*12,param[1][seq],5,12,0,RC,back_color);break;
	}
}

void model_setup()
{
	u8 i;
	if (menu3 == 1)
	{
		model_rename();
		if (menu3 == 1)
			return;
		while (menu_esc == 0 || menu_ok == 0)
			;
		ESC = 0;
		OK = 0;
	}
	else
		XUI_str(56, 4, 64, 12, (u8 *)"模型设置", 12, 1, PC, BC);

	if (first_splash == 1)
	{
		first_splash = 0;
		if (model_count > model_y_count)
			menu_len = model_y_count;
		else
			menu_len = model_count;
		px = py = 0;
		menu_y = 0;
		param[0][0] = model_current;
	}
	if (menu3 == 0)
	{
		for (i = 0; i < menu_len; i++) // 全部刷新
		{
			draw_model_info1(i);
			draw_model_info2(i);
		}
		if (LEFT)
		{
			LEFT = 0;
			if (px)
				px--;
			else
				px = model_x_count - 1;
		}
		if (RIGHT)
		{
			RIGHT = 0;
			if (px < (model_x_count - 1))
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

				// 差保存参数的程序
			}
			if (DOWN || UP)
			{
				if (px == 0)
				{
					switch (py)
					{
						//				case 0:
						//					break;
						//				case 1:
						//					break;
						//				case 2:
						//					break;
						//				case 3:
						//					break;
					}
				}
				else
				{
					switch (py)
					{
					}
				}
				UP = 0;
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
					py = model_count - 1;
					//					if(model_count>=model_y_count)menu_y=model_count-model_y_count;
					//					else
					menu_y = 0;
				}
				if (py < menu_y)
					menu_y--;
			}
			if (DOWN)
			{
				DOWN = 0;
				py++;
				if (py >= model_count)
				{
					py = 0;
					menu_y = 0;
				}
				else if (py >= (menu_y + model_y_count))
					menu_y++;
			}
			if (ESC)
			{
				ESC = 0;
#if USE_animation == 0
				ui_menu_flag = 0;
#else
				dir = 15;
#endif
			}
			if (OK)
			{
				OK = 0;
				if (py == 1 && px == 0)
				{
					menu3 = 1;
					first_splash = 1;
				}
				else if (py == 0 && px == 0)
				{
					menu3 = 2;
					sy = model_current;
					// first_splash=1;
				}
				else if (py == 2 && px == 0)
				{
					menu3 = 3;
				}
				else if (px == 1 && py == 0)
					message("Disable", 20);
				else if (px == 1 && py == 1)
					message("Disable", 20);
				else if (px == 1 && py == 2)
					message("Disable", 20);
				else
					param_edit = 1;
			}
		}
	}
	else if (menu3 == 2)
	{
		for (i = 0; i < menu_len; i++) // 全部刷新
		{
			draw_model_info1(i);
			draw_model_info2(i);
		}
		select_menu((u8 *)model_select_index, 4);
	}
	else if (menu3 == 3)
	{
		for (i = 0; i < menu_len; i++) // 全部刷新
		{
			draw_model_info1(i);
			draw_model_info2(i);
		}
		if (check_ok("Reset ModelData?"))
		{
			model_reset();
			model_read();
			message("OK!!", 20);
		}
	}
}
