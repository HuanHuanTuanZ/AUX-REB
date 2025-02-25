#include "common.h"
// #include "lcd.h"
#include "XUI.h"
#include "usart.h"
#include "string.h"

#define desk_max 4
#define desk_count1 6
#define desk_count2 7
#define desk_count3 3
#define desk_count4 5

char model_name[16] = {"MODEL01"};

const char sw_icon1[] = {0x08, 0x14, 0x22, 0x49, 0x08, 0x08, 0x08, 0x08};				/*"?????",0*/
const char sw_icon2[] = {0x00, 0x00, 0x00, 0x7E, 0x7E, 0x00, 0x00, 0x00};				/*"?????",0*/
const char sw_icon3[] = {0x08, 0x08, 0x08, 0x08, 0x49, 0x22, 0x14, 0x08}; /*"?????",0*/ /*"?????",0*/

void draw_sw_sta(u8 x, u8 y, u8 sta)
{
	if (sta == 0)
		XUI_bmp1(x, y, 8, 8, (u8 *)sw_icon1, 1, PC, BC);
	else if (sta == 1)
		XUI_bmp1(x, y, 8, 8, (u8 *)sw_icon2, 1, PC, BC);
	else if (sta == 2)
		XUI_bmp1(x, y, 8, 8, (u8 *)sw_icon3, 1, PC, BC);
}

u8 str_t = 0;
void XUI_desk1()
{
	u8 trim1;
	u8 trim2;
	u8 trim3;
	u8 trim4;
	u8 x1, y1, x2, y2;
	u8 len = 0;

	statusbar();
	len = strlen(model_name);
	if (len <= 8)
		XUI_str(80 - len * 4, 2, 64, 16, (u8 *)model_name, 16, 1, PC, BC);
	else
	{
		XUI_str(48, 2, 64, 16, (u8 *)(model_name + str_t), 16, 1, PC, BC);
		if (ui_1hz)
			ui_1hz = 0, str_t++;
		if (str_t > (len - 8))
			str_t = 0;
	}
	trim1 = (ch_mid[0]) * 20 / 250 + 33;
	trim2 = (-ch_mid[1]) * 20 / 250 + 47;
	trim3 = (-ch_mid[2]) * 20 / 250 + 47;
	trim4 = (ch_mid[3]) * 20 / 250 + 127;
	XUI_line(5, 24, 5, 70, PC);		// 微调中线
	XUI_line(155, 24, 155, 70, PC); // 微调中线
	XUI_line(10, 75, 56, 75, PC);	// 微调中线
	XUI_line(104, 75, 150, 75, PC); // 微调中线

	XUI_bar(5, trim2, 3, 3, 2, BC);				  // 微调滑块
	XUI_bar(155, trim3, 3, 3, 2, BC);			  // 微调滑块
	XUI_bar0(5, trim2, 3, 3, 2, PC);			  // 微调滑块
	XUI_bar0(155, trim3, 3, 3, 2, PC);			  // 微调滑块
	XUI_line(4, trim2 - 1, 6, trim2 - 1, PC);	  // 微调滑块分割线
	XUI_line(4, trim2 + 1, 6, trim2 + 1, PC);	  // 微调滑块分割线
	XUI_line(154, trim3 - 1, 156, trim3 - 1, PC); // 微调滑块分割线
	XUI_line(154, trim3 + 1, 156, trim3 + 1, PC); // 微调滑块分割线

	XUI_bar(trim1, 75, 3, 3, 2, BC);
	XUI_bar(trim4, 75, 3, 3, 2, BC);
	XUI_bar0(trim1, 75, 3, 3, 2, PC);
	XUI_bar0(trim4, 75, 3, 3, 2, PC);
	XUI_line(trim1 - 1, 74, trim1 - 1, 76, PC);
	XUI_line(trim1 + 1, 74, trim1 + 1, 76, PC);
	XUI_line(trim4 - 1, 74, trim4 - 1, 76, PC);
	XUI_line(trim4 + 1, 74, trim4 + 1, 76, PC);

	XUI_rectangle(10, 24, 56, 70, PC);	 // 摇杆方框
	XUI_rectangle(104, 24, 150, 70, PC); // 摇杆方框
	XUI_line(31, 47, 35, 47, SC);		 // 摇杆十字
	XUI_line(33, 45, 33, 49, SC);		 // 摇杆十字

	XUI_line(125, 47, 129, 47, SC); // 摇杆十字
	XUI_line(127, 45, 127, 49, SC); // 摇杆十字

	x1 = (CH_out[3] - 1500) * 20 / 500 + 33;
	y1 = -(CH_out[2] - 1500) * 20 / 500 + 47;
	x2 = (CH_out[0] - 1500) * 20 / 500 + 127;
	y2 = -(CH_out[1] - 1500) * 20 / 500 + 47;
	XUI_circle(x1, y1, 4, PC); // 摇杆圆圈
	XUI_circle(x2, y2, 4, PC); // 摇杆圆圈

	XUI_rectangle(60, 24, 66, 70, PC);
	XUI_rectangle(94, 24, 100, 70, PC);
	XUI_string8(58, 72, (u8 *)"S1", 1, PC, BC);
	XUI_string8(91, 72, (u8 *)"S2", 1, PC, BC);
	XUI_fill(61, 25 + (69 - 25) * (2000 - CH_out[4]) / 1000, 65, 69, SC);
	XUI_fill(95, 25 + (69 - 25) * (2000 - CH_out[5]) / 1000, 99, 69, SC);

	XUI_string8(70, 24, (u8 *)"SA", 1, PC, BC);
	XUI_string8(70, 36, (u8 *)"SB", 1, PC, BC);
	XUI_string8(70, 48, (u8 *)"SC", 1, PC, BC);
	XUI_string8(70, 60, (u8 *)"SD", 1, PC, BC);

	draw_sw_sta(83, 24, CH_out[6] < 1250 ? 0 : CH_out[6] < 1750 ? 1
																: 2);
	draw_sw_sta(83, 36, CH_out[7] < 1250 ? 0 : CH_out[7] < 1750 ? 1
																: 2);
	draw_sw_sta(83, 48, CH_out[8] < 1250 ? 0 : CH_out[8] < 1750 ? 1
																: 2);
	draw_sw_sta(83, 60, CH_out[9] < 1250 ? 0 : CH_out[9] < 1750 ? 1
																: 2);
}

void XUI_desk2()
{
	u8 i;
	char temp[5];
	XUI_fill(0, 0, 159, 15, RC);
	XUI_bmp1(2, 0, 16, 16, download_icon, 1, PC, RC);
	//	XUI_bmp1(126,0,32,16,ui_battery_icon,PC,RC);
	sprintf(temp, "%2.1fV", battery / 1000.0f);
	XUI_str(134, 1, 24, 12, (u8 *)temp, 12, 10, PC, RC);
	if (tx_signal)
		XUI_num(18, 1, tx_signal, 3, 12, 1, PC, RC);
	else
		XUI_str(18, 2, 24, 12, (u8 *)"NC", 12, 10, PC, RC);
	XUI_num(114, 1, fps, 2, 12, 1, PC, RC);
	XUI_str(56, 2, 48, 12, (u8 *)"通道数据", 12, 1, PC, BC);
	XUI_string8(1, 16, (u8 *)"CH1", 0, PC, BC);
	XUI_string8(1, 24, (u8 *)"CH2", 0, PC, BC);
	XUI_string8(1, 32, (u8 *)"CH3", 0, PC, BC);
	XUI_string8(1, 40, (u8 *)"CH4", 0, PC, BC);
	XUI_string8(1, 48, (u8 *)"CH5", 0, PC, BC);
	XUI_string8(1, 56, (u8 *)"CH6", 0, PC, BC);
	XUI_string8(1, 64, (u8 *)"CH7", 0, PC, BC);
	XUI_string8(1, 72, (u8 *)"CH8", 0, PC, BC);
	XUI_string8(81, 16, (u8 *)"CH9", 0, PC, BC);
	XUI_string8(81, 24, (u8 *)"C10", 0, PC, BC);
	XUI_string8(81, 32, (u8 *)"C11", 0, PC, BC);
	XUI_string8(81, 40, (u8 *)"C12", 0, PC, BC);
	XUI_string8(81, 48, (u8 *)"C13", 0, PC, BC);
	XUI_string8(81, 56, (u8 *)"C14", 0, PC, BC);
	XUI_string8(81, 64, (u8 *)"C15", 0, PC, BC);
	XUI_string8(81, 72, (u8 *)"C16", 0, PC, BC);
	for (i = 0; i < 8; i++)
	{
		XUI_rectangle(20, 17 + i * 8, 78, 17 + i * 8 + 6, PC);
		XUI_rectangle(100, 17 + i * 8, 158, 17 + i * 8 + 6, PC);
		XUI_fill(20, 17 + i * 8 + 1, 20 + (CH_out[i] - 1000) * 58 / 1000, 17 + i * 8 + 5, PC);
		XUI_fill(100, 17 + i * 8 + 1, 100 + (CH_out[i + 8] - 1000) * 58 / 1000, 17 + i * 8 + 5, PC);
	}
}

void XUI_desk3()
{
	statusbar();
	XUI_str(52, 2, 64, 16, (u8 *)"PPM输入", 16, 1, PC, BC);
	if (ppm_status)
	{
		XUI_str(1, 22, 100, 12, (u8 *)"PPM1:", 12, 1, FC, BC);
		XUI_str(1, 37, 100, 12, (u8 *)"PPM2:", 12, 1, FC, BC);
		XUI_str(1, 52, 100, 12, (u8 *)"PPM3:", 12, 1, FC, BC);
		XUI_str(1, 67, 100, 12, (u8 *)"PPM4:", 12, 1, FC, BC);
		XUI_str(81, 22, 100, 12, (u8 *)"PPM5:", 12, 1, FC, BC);
		XUI_str(81, 37, 100, 12, (u8 *)"PPM6:", 12, 1, FC, BC);
		XUI_str(81, 52, 100, 12, (u8 *)"PPM7:", 12, 1, FC, BC);
		XUI_str(81, 67, 100, 12, (u8 *)"PPM8:", 12, 1, FC, BC);
		XUI_num(30, 22, ppm_value[0], 4, 12, 1, PC, RC);
		XUI_num(30, 37, ppm_value[1], 4, 12, 1, PC, RC);
		XUI_num(30, 52, ppm_value[2], 4, 12, 1, PC, RC);
		XUI_num(30, 67, ppm_value[3], 4, 12, 1, PC, RC);
		XUI_num(110, 22, ppm_value[4], 4, 12, 1, PC, RC);
		XUI_num(110, 37, ppm_value[5], 4, 12, 1, PC, RC);
		XUI_num(110, 52, ppm_value[6], 4, 12, 1, PC, RC);
		XUI_num(110, 67, ppm_value[7], 4, 12, 1, PC, RC);
	}
	else
		XUI_str(40, 38, 160, 16, (u8 *)"无信号输入", 16, 1, FC, BC);
}

void XUI_desk4()
{
	statusbar();
	XUI_str(48, 2, 64, 16, (u8 *)"SBUS输入", 16, 1, PC, BC);
	if (sbus_status)
	{
		u8 i;
		XUI_str(0, 22, 100, 12, (u8 *)"1:", 12, 1, FC, BC);
		XUI_str(0, 37, 100, 12, (u8 *)"2:", 12, 1, FC, BC);
		XUI_str(0, 52, 100, 12, (u8 *)"3:", 12, 1, FC, BC);
		XUI_str(0, 67, 100, 12, (u8 *)"4:", 12, 1, FC, BC);
		XUI_str(40, 22, 100, 12, (u8 *)"5:", 12, 1, FC, BC);
		XUI_str(40, 37, 100, 12, (u8 *)"6:", 12, 1, FC, BC);
		XUI_str(40, 52, 100, 12, (u8 *)"7:", 12, 1, FC, BC);
		XUI_str(40, 67, 100, 12, (u8 *)"8:", 12, 1, FC, BC);
		XUI_str(80, 22, 100, 12, (u8 *)"9:", 12, 1, FC, BC);
		XUI_str(80, 37, 100, 12, (u8 *)"10", 12, 1, FC, BC);
		XUI_str(80, 52, 100, 12, (u8 *)"11", 12, 1, FC, BC);
		XUI_str(80, 67, 100, 12, (u8 *)"12", 12, 1, FC, BC);
		XUI_str(120, 22, 100, 12, (u8 *)"13", 12, 1, FC, BC);
		XUI_str(120, 37, 100, 12, (u8 *)"14", 12, 1, FC, BC);
		XUI_str(120, 52, 100, 12, (u8 *)"15", 12, 1, FC, BC);
		XUI_str(120, 67, 100, 12, (u8 *)"16", 12, 1, FC, BC);
		for (i = 0; i < 4; i++)
		{
			XUI_num(12 + 40 * i, 22, sbus_value[4 * i], 4, 12, 1, PC, BC);
			XUI_num(12 + 40 * i, 37, sbus_value[4 * i + 1], 4, 12, 1, PC, BC);
			XUI_num(12 + 40 * i, 52, sbus_value[4 * i + 2], 4, 12, 1, PC, BC);
			XUI_num(12 + 40 * i, 67, sbus_value[4 * i + 3], 4, 12, 1, PC, BC);
		}
	}
	else
		XUI_str(40, 38, 160, 16, (u8 *)"无信号输入", 16, 1, FC, BC);
}

void XUI_desk5()
{
	statusbar();
	XUI_str(52, 2, 64, 16, (u8 *)"ADC输入", 16, 1, PC, BC);
	XUI_str(1, 22, 100, 12, (u8 *)"ADC1:", 12, 1, FC, BC);
	XUI_str(1, 37, 100, 12, (u8 *)"ADC2:", 12, 1, FC, BC);
	XUI_str(1, 52, 100, 12, (u8 *)"ADC3:", 12, 1, FC, BC);
	XUI_str(1, 67, 100, 12, (u8 *)"ADC4:", 12, 1, FC, BC);
	XUI_str(81, 22, 100, 12, (u8 *)"ADC5:", 12, 1, FC, BC);
	XUI_str(81, 37, 100, 12, (u8 *)"ADC6:", 12, 1, FC, BC);
	XUI_str(81, 52, 100, 12, (u8 *)"ADC7:", 12, 1, FC, BC);
	XUI_str(81, 67, 100, 12, (u8 *)"ADC8:", 12, 1, FC, BC);
	XUI_num(30, 22, hardware_value[0], 4, 12, 1, PC, BC);
	XUI_num(30, 37, hardware_value[1], 4, 12, 1, PC, BC);
	XUI_num(30, 52, hardware_value[2], 4, 12, 1, PC, BC);
	XUI_num(30, 67, hardware_value[3], 4, 12, 1, PC, BC);
	XUI_num(110, 22, hardware_value[4], 4, 12, 1, PC, BC);
	XUI_num(110, 37, hardware_value[5], 4, 12, 1, PC, BC);
	XUI_num(110, 52, hardware_value[6], 4, 12, 1, PC, BC);
	XUI_num(110, 67, hardware_value[7], 4, 12, 1, PC, BC);
}

void led_status(u8 x, u8 y, u8 sta)
{
	if (sta == 0)
	{
		XUI_fill_circle(x, y, 5, RED8);
		XUI_circle(x, y, 5, PC);
	}
	else if (sta != 255)
	{
		XUI_fill_circle(x, y, 5, GREEN8);
		XUI_circle(x, y, 5, PC);
	}
	else
	{
		XUI_fill_circle(x, y, 5, GRAY8);
		XUI_circle(x, y, 5, PC);
	}
}

extern const char mixer_index[][4];
void system_status()
{
	u8 i = 0;
	u8 x = 0;
	u8 y = 0;
	statusbar();
	XUI_str(48, 2, 64, 16, (u8 *)"系统状态", 16, 1, PC, BC);

	XUI_str(10, 20, 64, 12, (u8 *)"PPM输入", 12, 1, PC, BC);
	led_status(66, 26, ppm_connected);
	XUI_str(10, 32, 64, 12, (u8 *)"SBUS输入", 12, 1, PC, BC);
	led_status(66, 38, sbus_connected);

	XUI_str(10, 44, 64, 12, (u8 *)"ADC输入", 12, 1, PC, BC);
	led_status(66, 50, adc_enable);

	XUI_str(10, 56, 64, 12, (u8 *)"ELRS-TX", 12, 1, PC, BC);
	led_status(66, 62, elrs_connected);
	XUI_str(10, 68, 64, 12, (u8 *)"ELRS回传", 12, 1, PC, BC);
	led_status(66, 74, elrs_back_connected);

#if MAVLINK

	if (usart2_function == 0)
	{
		XUI_str(90, 20, 64, 12, (u8 *)"Mavlink", 12, 1, PC, BC);
		if (mavlink_enable)
			led_status(146, 26, mavlink_connected);
		else
			led_status(146, 26, 255);
	}
	else if (usart2_function == 1)
	{
		XUI_str(90, 20, 64, 12, (u8 *)"Mpu6050", 12, 1, PC, BC);
		led_status(146, 26, gyro_connected);
	}
	x = 90;
	y = 20;
#else

	if (usart2_function == 1)
	{
		XUI_str(90, 20, 64, 12, (u8 *)"Mpu6050", 12, 1, PC, BC);
		led_status(146, 26, gyro_connected);
		x = 90;
		y = 20;
	}
	else
	{
		x = 10;
		y = 68;
	}
#endif

	for (i = 0; i < 4; i++)
	{
		if (mixer_type[i])
		{
			y += 12;
			if (y >= 80)
			{
				y = 20;
				x = 90;
			}
			XUI_str(x, y, 24, 12, (u8 *)mixer_index[mixer_type[i]], 12, 1, PC, BC);
			XUI_str(x + 24, y, 24, 12, (u8 *)"混控", 12, 1, PC, BC);
			led_status(x + 56, y + 6, 1);
		}
	}

	for (i = 0; i < 4; i++)
	{
		if (logic_condition[i])
		{
			y += 12;
			if (y >= 80)
			{
				y = 20;
				x = 90;
			}
			XUI_str(x, y, 24, 12, (u8 *)"逻辑", 12, 1, PC, BC);
			XUI_num(x + 24, y, i, 1, 12, 1, PC, BC);
			led_status(x + 56, y + 6, logic_status[i]);
		}
	}
}

u8 dir = 0; // 上1 下2 左3 右4
int16 pixel = 0;

void ui_change()
{

#if USE_animation == 1
	if (dir == 3) // 左
	{
		if (pixel < 160)
			pixel += 20;
		else
		{
			dir = 0;
			pixel = 0;
			first_splash = 1;
			if (ui_current)
				ui_current--;
			else
				ui_current = desk_max - 1;
		}
		x_offset = pixel - 160;
		if (ui_menu_edit == 0)
		{
			switch (ui_current)
			{
			///////////////遥控界面
			case 1:
				switch (ui_flag[0])
				{
				case 0:
					XUI_desk1();
					break;
				case 1:
					XUI_desk2();
					break;
				case 2:
					XUI_desk3();
					break;
				case 3:
					XUI_desk4();
					break;
				case 4:
					XUI_desk5();
					break;
				case 5:
					system_status();
					break;
				}
				break;
			///////////////数据回传
			case 2:
				switch (ui_flag[1])
				{
				case 0:
					hud();
					break;
				case 1:
					hud2();
					break;
				case 2:
					hud3();
					break;
				case 3:
					XUI_data1();
					break;
				case 4:
					XUI_data2();
					break;
				case 5:
					XUI_data3();
					break;
				case 6:
					crsf_test();
					break;
				}
				break;
			///////////////实用工具
			case 3:
				switch (ui_flag[2])
				{
				case 0:
					XUI_tool1();
					break;
				case 1:
					XUI_tool2();
					break;
				case 2:
					XUI_tool3();
					break;
				case 3:
					clock();
					break;
				case 4:
					triangle();
					break;
				}
				break;
				///////////////Mavlink
			case 0:
				switch (ui_flag[3])
				{
				case 0:
					mavlink_data1();
					break;
				case 1:
					mavlink_data2();
					break;
				case 2:
					mavlink_data3();
					break;
				case 3:
					mavlink_data4();
					break;
				case 4:
					mavlink_test();
					break;
				}
				break;
			}
		}
		x_offset = pixel;
	}
	else if (dir == 4) // 右
	{
		if (pixel > -160)
			pixel -= 20;
		else
		{
			dir = 0;
			pixel = 0;
			first_splash = 1;
			if (ui_current < (desk_max - 1))
				ui_current++;
			else
				ui_current = 0;
		}
		x_offset = pixel + 160;
		if (ui_menu_edit == 0)
		{
			switch (ui_current)
			{
			///////////////遥控界面
			case 3:
				switch (ui_flag[0])
				{
				case 0:
					XUI_desk1();
					break;
				case 1:
					XUI_desk2();
					break;
				case 2:
					XUI_desk3();
					break;
				case 3:
					XUI_desk4();
					break;
				case 4:
					XUI_desk5();
					break;
				case 5:
					system_status();
					break;
				}
				break;
			///////////////数据回传
			case 0:
				switch (ui_flag[1])
				{
				case 0:
					hud();
					break;
				case 1:
					hud2();
					break;
				case 2:
					hud3();
					break;
				case 3:
					XUI_data1();
					break;
				case 4:
					XUI_data2();
					break;
				case 5:
					XUI_data3();
					break;
				case 6:
					crsf_test();
					break;
				}
				break;
			///////////////实用工具
			case 1:
				switch (ui_flag[2])
				{
				case 0:
					XUI_tool1();
					break;
				case 1:
					XUI_tool2();
					break;
				case 2:
					XUI_tool3();
					break;
				case 3:
					clock();
					break;
				case 4:
					triangle();
					break;
				}
				break;
				///////////////Mavlink
			case 2:
				switch (ui_flag[3])
				{
				case 0:
					mavlink_data1();
					break;
				case 1:
					mavlink_data2();
					break;
				case 2:
					mavlink_data3();
					break;
				case 3:
					mavlink_data4();
					break;
				case 4:
					mavlink_test();
					break;
				}
				break;
			}
		}
		x_offset = pixel;
	}
	else if (dir == 1) // 上
	{
		if (pixel < 80)
			pixel += 10;
		else
		{
			dir = 0;
			pixel = 0;
			first_splash = 1;
			if (ui_current == 0)
			{
				if (ui_flag[ui_current])
					ui_flag[ui_current]--;
				else
					ui_flag[ui_current] = desk_count1 - 1;
			}
			else if (ui_current == 1)
			{
				if (ui_flag[ui_current])
					ui_flag[ui_current]--;
				else
					ui_flag[ui_current] = desk_count2 - 1;
			}
			else if (ui_current == 2)
			{
				if (ui_flag[ui_current])
					ui_flag[ui_current]--;
				else
					ui_flag[ui_current] = desk_count3 - 1;
			}
			else if (ui_current == 3)
			{
				if (ui_flag[ui_current])
					ui_flag[ui_current]--;
				else
					ui_flag[ui_current] = desk_count4 - 1;
			}
		}
		y_offset = pixel - 80;
		if (ui_menu_edit == 0)
		{
			switch (ui_current)
			{
			///////////////遥控界面
			case 0:
				switch (ui_flag[0])
				{
				case 1:
					XUI_desk1();
					break;
				case 2:
					XUI_desk2();
					break;
				case 3:
					XUI_desk3();
					break;
				case 4:
					XUI_desk4();
					break;
				case 5:
					XUI_desk5();
					break;
				case 0:
					system_status();
					break;
				}
				break;
			///////////////数据回传
			case 1:
				switch (ui_flag[1])
				{
				case 1:
					hud();
					break;
				case 2:
					hud2();
					break;
				case 3:
					hud3();
					break;
				case 4:
					XUI_data1();
					break;
				case 5:
					XUI_data2();
					break;
				case 6:
					XUI_data3();
					break;
				case 0:
					crsf_test();
					break;
				}
				break;
			///////////////实用工具
			case 2:
				switch (ui_flag[2])
				{
				case 1:
					XUI_tool1();
					break;
				case 2:
					XUI_tool2();
					break;
				case 3:
					XUI_tool3();
					break;
				case 4:
					clock();
					break;
				case 0:
					triangle();
					break;
				}
				break;
				///////////////Mavlink
			case 3:
				switch (ui_flag[3])
				{
				case 1:
					mavlink_data1();
					break;
				case 2:
					mavlink_data2();
					break;
				case 3:
					mavlink_data3();
					break;
				case 4:
					mavlink_data4();
					break;
				case 0:
					mavlink_test();
					break;
				}
				break;
			}
		}
		y_offset = pixel;
	}
	else if (dir == 2) // 下
	{
		if (pixel > -80)
			pixel -= 10;
		else
		{
			dir = 0;
			pixel = 0;
			first_splash = 1;
			if (ui_current == 0)
			{
				if (ui_flag[ui_current] < (desk_count1 - 1))
					ui_flag[ui_current]++;
				else
					ui_flag[ui_current] = 0;
			}
			else if (ui_current == 1)
			{
				if (ui_flag[ui_current] < (desk_count2 - 1))
					ui_flag[ui_current]++;
				else
					ui_flag[ui_current] = 0;
			}
			else if (ui_current == 2)
			{
				if (ui_flag[ui_current] < (desk_count3 - 1))
					ui_flag[ui_current]++;
				else
					ui_flag[ui_current] = 0;
			}
			else if (ui_current == 3)
			{
				if (ui_flag[ui_current] < (desk_count4 - 1))
					ui_flag[ui_current]++;
				else
					ui_flag[ui_current] = 0;
			}
		}
		y_offset = pixel + 80;
		if (ui_menu_edit == 0)
		{
			switch (ui_current)
			{
			///////////////遥控界面
			case 0:
				switch (ui_flag[0])
				{
				case 5:
					XUI_desk1();
					break;
				case 0:
					XUI_desk2();
					break;
				case 1:
					XUI_desk3();
					break;
				case 2:
					XUI_desk4();
					break;
				case 3:
					XUI_desk5();
					break;
				case 4:
					system_status();
					break;
				}
				break;
			///////////////数据回传
			case 1:
				switch (ui_flag[1])
				{
				case 6:
					hud();
					break;
				case 0:
					hud2();
					break;
				case 1:
					hud3();
					break;
				case 2:
					XUI_data1();
					break;
				case 3:
					XUI_data2();
					break;
				case 4:
					XUI_data3();
					break;
				case 5:
					crsf_test();
					break;
				}
				break;
			///////////////实用工具
			case 2:
				switch (ui_flag[2])
				{
				case 4:
					XUI_tool1();
					break;
				case 0:
					XUI_tool2();
					break;
				case 1:
					XUI_tool3();
					break;
				case 2:
					clock();
					break;
				case 3:
					triangle();
					break;
				}
				break;
				///////////////Mavlink
			case 3:
				switch (ui_flag[3])
				{
				case 4:
					mavlink_data1();
					break;
				case 0:
					mavlink_data2();
					break;
				case 1:
					mavlink_data3();
					break;
				case 2:
					mavlink_data4();
					break;
				case 3:
					mavlink_test();
					break;
				}
				break;
			}
		}
		y_offset = pixel;
	}
	else if (dir == 5) // 进入设置
	{
		if (pixel < 80)
			pixel += 10;
		else
		{
			dir = 0;
			pixel = 0;
			ui_menu_edit = 1;
			first_splash = 1;
		}
		y_offset = pixel - 80;
		if (ui_menu_edit == 0)
		{
			statusbar();
			XUI_setup_menu();
		}
		y_offset = pixel;
	}
	else if (dir == 6) // 退出设置
	{
		if (pixel > -80)
			pixel -= 10;
		else
		{
			dir = 0;
			pixel = 0;
			ui_menu_edit = 0;
			first_splash = 1;
		}
		y_offset = pixel + 80;
		if (ui_menu_edit == 1)
		{
			switch (ui_current)
			{
			///////////////遥控界面
			case 0:
				switch (ui_flag[ui_current])
				{
				case 0:
					XUI_desk1();
					break;
				case 1:
					XUI_desk2();
					break;
				case 2:
					XUI_desk3();
					break;
				case 3:
					XUI_desk4();
					break;
				case 4:
					XUI_desk5();
					break;
				case 5:
					system_status();
					break;
				}
				break;
			///////////////数据回传
			case 1:
				switch (ui_flag[ui_current])
				{
				case 0:
					hud();
					break;
				case 1:
					hud2();
					break;
				case 2:
					hud3();
					break;
				case 3:
					XUI_data1();
					break;
				case 4:
					XUI_data2();
					break;
				case 5:
					XUI_data3();
					break;
				case 6:
					crsf_test();
					break;
				}
				break;
			///////////////实用工具
			case 2:
				switch (ui_flag[ui_current])
				{
				case 0:
					XUI_tool1();
					break;
				case 1:
					XUI_tool2();
					break;
				case 2:
					XUI_tool3();
					break;
				case 3:
					clock();
					break;
				case 4:
					triangle();
					break;
				}
				break;
				///////////////Mavlink
			case 3:
				switch (ui_flag[ui_current])
				{
				case 0:
					mavlink_data1();
					break;
				case 1:
					mavlink_data2();
					break;
				case 2:
					mavlink_data3();
					break;
				case 3:
					mavlink_data4();
					break;
				case 4:
					mavlink_test();
					break;
				}
				break;
			}
		}
		y_offset = pixel;
	}
	else if (dir >= 7 && dir <= 14) // 进入设置
	{
		if (pixel < 80)
			pixel += 10;
		else
		{
			dir = 0;
			pixel = 0;
			ui_menu_flag = ui_select_flag + 1;
			first_splash = 1;
		}
		y_offset = pixel - 80;
		statusbar();
		if (first_splash <= 1)
		{
			switch (ui_select_flag + 1)
			{
			case 1:
				model_setup();
				break;
			case 2:
				input_setup();
				break;
			case 3:
				mix_setup();
				break;
			case 4:
				output_setup();
				break;
			case 5:
				logic_setup();
				break;
			case 6:
				system_setup();
				break;
			case 7:
				adc_adjust();
				break;
			case 8:
				version_info();
				break;
			}
		}
		y_offset = pixel;
	}
	else if (dir == 15) // 退出设置
	{
		if (pixel > -80)
			pixel -= 10;
		else
		{
			dir = 0;
			pixel = 0;
			ui_menu_flag = 0;
			first_splash = 1;
			menu3 = 0;
		}
		y_offset = pixel + 80;
		statusbar();
		XUI_setup_menu();
		y_offset = pixel;
	}
#endif

	if (ui_menu_edit == 0 && dir == 0)
	{
		if (OK)
		{
			OK = 0;
#if USE_animation == 0
			ui_menu_edit = 1;
			first_splash = 1;
#else
			dir = 5;
#endif
		}
		if (LEFT)
		{
			LEFT = 0;
#if USE_animation == 0
			if (ui_current)
				ui_current--;
			else
				ui_current = desk_max - 1;
#else
			dir = 3;
#endif
		}
		if (RIGHT)
		{
			RIGHT = 0;
#if USE_animation == 0
			if (ui_current < (desk_max - 1))
				ui_current++;
			else
				ui_current = 0;
#else
			dir = 4;
#endif
		}
		if (DOWN)
		{
			DOWN = 0;
#if USE_animation == 0
			if (ui_current == 0)
			{
				if (ui_flag[ui_current] < (desk_count1 - 1))
					ui_flag[ui_current]++;
				else
					ui_flag[ui_current] = 0;
			}
			else if (ui_current == 1)
			{
				if (ui_flag[ui_current] < (desk_count2 - 1))
					ui_flag[ui_current]++;
				else
					ui_flag[ui_current] = 0;
			}
			else if (ui_current == 2)
			{
				if (ui_flag[ui_current] < (desk_count3 - 1))
					ui_flag[ui_current]++;
				else
					ui_flag[ui_current] = 0;
			}
			else if (ui_current == 3)
			{
				if (ui_flag[ui_current] < (desk_count4 - 1))
					ui_flag[ui_current]++;
				else
					ui_flag[ui_current] = 0;
			}
#else
			dir = 2;
#endif
		}
		if (UP)
		{
			UP = 0;
#if USE_animation == 0
			if (ui_current == 0)
			{
				if (ui_flag[ui_current])
					ui_flag[ui_current]--;
				else
					ui_flag[ui_current] = (desk_count1 - 1);
			}
			else if (ui_current == 1)
			{
				if (ui_flag[ui_current])
					ui_flag[ui_current]--;
				else
					ui_flag[ui_current] = (desk_count2 - 1);
			}
			else if (ui_current == 2)
			{
				if (ui_flag[ui_current])
					ui_flag[ui_current]--;
				else
					ui_flag[ui_current] = (desk_count3 - 1);
			}
			else if (ui_current == 3)
			{
				if (ui_flag[ui_current])
					ui_flag[ui_current]--;
				else
					ui_flag[ui_current] = (desk_count4 - 1);
			}
#else
			dir = 1;
#endif
		}
	}
	else if (dir == 0)
	{
		if (ui_menu_flag == 0)
		{
			if (OK == 1)
			{
				OK = 0;
#if USE_animation == 0
				ui_menu_flag = ui_select_flag + 1;
				first_splash = 1;
#else
				dir = ui_select_flag + 1 + 6;
#endif
			}
			if (UP | LEFT)
			{
				UP = 0;
				LEFT = 0;
#if USE_animation == 0
				if (ui_select_flag)
					ui_select_flag--;
				else
					ui_select_flag = 7;
#else
				dir = 16;
#endif
			}

			if (DOWN | RIGHT)
			{
				DOWN = 0;
				RIGHT = 0;
#if USE_animation == 0
				if (ui_select_flag < 7)
					ui_select_flag++;
				else
					ui_select_flag = 0;
#else
				dir = 17;
#endif
			}
			if (ESC)
			{
				ESC = 0;
#if USE_animation == 0
				ui_menu_edit = 0;
#else
				dir = 6;
#endif
			}
		}
		else
		{
			if (ESC)
			{
				// if(!menu3)ESC=0;
				if (menu3)
				{
				}
				else if (param_edit)
				{
				} // param_edit=0;
				else if (ui_menu_flag)
				{
#if USE_animation == 0
					ui_menu_flag = 0;
#else
					dir = 15;
#endif
					ESC = 0;
				}
			}
		}
	}
}

void statusbar()
{
	char temp[5];
#if BKG == 0
	if (bkg_type <= 1)
		XUI_fill(0, 0, 159, 19, RC);
#else
	if (bkg_type == 1)
		XUI_fill(0, 0, 159, 19, RC);
#endif
	XUI_bmp1(2, 2, 16, 16, download_icon, 1, PC, RC);
	{
		u8 x = 0;
		if (battery > 3500 && battery < 4200)
			x = (battery - 3500) * 29 / 700;
		else if (battery > 4500 && battery < 5500)
			x = 29;
		else if (battery > 7000 && battery < 8400)
			x = (battery - 7000) * 29 / 1400;
		else
			x = 29;
		XUI_fill(127, 4, 127 + x, 15, SC);
	}
	XUI_bmp1(126, 2, 32, 16, ui_battery_icon, 1, PC, RC);
	sprintf(temp, "%2.2f", battery / 1000.0f);
	XUI_str(128, 4, 24, 12, (u8 *)temp, 12, 10, PC, RC);
	if (tx_signal)
		XUI_num(18, 4, tx_signal, 3, 12, 1, PC, RC);
	else
		XUI_str(18, 4, 24, 12, (u8 *)"NC", 12, 10, PC, RC);
	XUI_num(114, 4, fps, 2, 12, 1, PC, RC);
}

void background()
{
#if BKG == 0
	if (bkg_type == 0)
		XUI_clear(BC); // 清除屏幕
	else if (bkg_type == 1)
		XUI_clear(BC); // 清除屏幕
	else if (bkg_type == 2)
	{
		if (ui_current != 1 || ui_flag[ui_current] > 2 || dir || ui_menu_edit)
			XUI_ground2(hud_roll, hud_pitch, LIGHTBLUE8, BROWN8);
	}
#else
	if (bkg_type == 0)
#if DMA
	{
		if (ui_current != 1 || ui_flag[ui_current] > 2 || dir || ui_menu_flag)
			XUI_bmp16_160_80(bkg);
	}
#else
	{
		if (ui_current != 1 || ui_flag[ui_current] > 2 || dir || ui_menu_flag)
			XUI_clear(BC); // 清除屏幕
	}
#endif
	else if (bkg_type == 1)
	{
		if (ui_current != 1 || ui_flag[ui_current] > 2 || dir || ui_menu_flag)
			XUI_clear(BC); // 清除屏幕
	}
	else if (bkg_type == 2)
	{
		if (ui_current != 1 || ui_flag[ui_current] > 2 || dir || ui_menu_flag)
			XUI_ground2(hud_roll, hud_pitch, LIGHTBLUE8, BROWN8);
	}
	else if (bkg_type == 3)
	{
	}
#endif
}
