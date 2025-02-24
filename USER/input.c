#include "common.h"
// #include "lcd.h"
#include "adc.h"
#include "ips.h"
#include "math.h"
#include "gpio.h"

int abs(int v)
{
	if (v >= 0)
		return v;
	else
		return -v;
}

u16 hardware_value[10];
u16 adc_min[8] = {0, 0, 0, 0, 0, 0, 0, 0};
u16 adc_mid[8] = {2047, 2047, 2047, 2047, 2047, 2047, 2047, 2047};
u16 adc_max[8] = {4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095};
u16 hardware_input[8];

int16 bat_adc = 0;
u32 bat_sum = 0;
u8 bat_n = 0;
#define bat_sum_max 20

void battery_check()
{
	int16 bat_adc_temp;
	int16 bat_delta;
	bat_sum += Get_Adc(9);
	bat_n++;
	if (bat_n >= bat_sum_max)
	{
		bat_adc_temp = bat_sum / bat_sum_max;
		bat_delta = abs(bat_adc_temp - bat_adc); // 获取本次与上次数据的差值
		if (bat_delta < 20)
			bat_adc = bat_adc * 0.98 + bat_adc_temp * 0.02;
		else if (bat_delta < 100)
			bat_adc = bat_adc * 0.95 + bat_adc_temp * 0.05;
		else
			bat_adc = bat_adc * 0.9 + bat_adc_temp * 0.1;
		bat_n = 0;
		bat_sum = 0;
		battery = bat_adc * bat_vol / 100.0;
	}
}

const u8 adc_map[6] = {1, 4, 5, 6, 7, 8};

void hardinput()
{
	u8 i;
	int16 hardware_input_temp;
	int16 delta;

	for (i = 0; i < 6; i++)
	{
		// 获取硬件输入原始值
		hardware_input_temp = Get_Adc(adc_map[i]);
		delta = abs(hardware_input_temp - hardware_input[i]); // 获取本次与上次数据的差值
		if (delta < 10)
			hardware_input[i] = hardware_input[i] * 0.9 + hardware_input_temp * 0.1;
		else if (delta < 20)
			hardware_input[i] = hardware_input[i] * 0.7 + hardware_input_temp * 0.3;
		else
			hardware_input[i] = hardware_input_temp;
		// 数据限位--防止超出ADC校准范围
		if (adc_max[i] <= hardware_input[i])
			adc_max[i] = hardware_input[i];
		else if (adc_min[i] >= hardware_input[i])
			adc_min[i] = hardware_input[i];
		// 预算硬件输入值，主要是引入adc校准参数
		if (hardware_input[i] < adc_mid[i])
			hardware_value[i] = (hardware_input[i] - adc_min[i]) * 500 / (adc_mid[i] - adc_min[i]) + 1000;
		else
			hardware_value[i] = (hardware_input[i] - adc_mid[i]) * 500 / (adc_max[i] - adc_mid[i]) + 1500;
	}
	hardware_value[6] = !ch7 * 1000 + 1000;
	hardware_value[7] = !ch8 * 1000 + 1000;
	hardware_value[8] = !ch9 * 1000 + 1000;
	hardware_value[9] = !ch10 * 1000 + 1000;
}

void input()
{
	u8 i;
	u16 temp[16];
	hardinput();
	for (i = 0; i < 16; i++)
	{
		if (ch_map[i] < 10)
		{
			if (adc_enable)
				temp[i] = hardware_value[ch_map[i]];
			else
				temp[i] = 1500;
		}
		else if (ch_map[i] < 18)
		{
			if (ppm_value[ch_map[i] - 10] >= 500)
				temp[i] = ppm_value[ch_map[i] - 10];
			else
				temp[i] = 1500;
		}
		else if (ch_map[i] < 34)
		{
			if (sbus_value[ch_map[i] - 18] >= 500)
				temp[i] = sbus_value[ch_map[i] - 18];
			else
				temp[i] = 1500;
		}
		else if (ch_map[i] < 42)
			temp[i] = 1500;

		// 运算通道数据
		if (temp[i] <= 1500)
			CH_input[i] = 3000 * ch_rev[i] + (1 - 2 * ch_rev[i]) * ((temp[i] - 1000) * (1500 + ch_mid[i] - 1000 - ch_min[i]) / 500 + 1000 + ch_min[i]);
		else
			CH_input[i] = 3000 * ch_rev[i] + (1 - 2 * ch_rev[i]) * ((temp[i] - 1500) * (2000 + ch_max[i] - 1500 - ch_mid[i]) / 500 + 1500 + ch_mid[i]);
	}
	battery_check();
}

////////菜单按键检测/////////////////////////////////////////////////////////////////////////////
u8 flag1 = 0, flag2 = 0;
u8 count1 = 0, count2 = 0;
extern u8 menu0;
extern u8 menu0_temp;
u8 remote_ctrl = 1;

extern int16 logic_source(u8 seq);

int16 remote_ctrl_source(u8 seq)
{
	if (seq == 0)
		return 0;
	else if (seq <= 16)
		return CH_input[seq - 1];
	else if (seq <= 24)
		return hardware_value[seq - 17];
	else if (seq <= 32)
		return ppm_value[seq - 25];
	else if (seq <= 48)
		return sbus_value[seq - 33];
	else
		return 0;
}

u8 remote_ctrl_check()
{
	if (remote_ctrl == 0)
		return 0;
	if (remote_ctrl_source(remote_ctrl) > 1750)
		return 1;
	return 0;
}
#include "timer.h"

void menu_check()
{
	if (menu_up == 0)
	{
		if (count1 <= 10)
			count1++;
		if (flag1 == 0)
			UP = 1, beep(1000, 2);
		flag1 = 1;
		if (count1 >= 10 && UP < 20)
			UP += 5;
	}
	else if (menu_down == 0)
	{
		if (count1 <= 10)
			count1++;
		if (flag1 == 0)
			DOWN = 1, beep(1000, 2);
		flag1 = 1;
		if (count1 >= 10 && DOWN < 20)
			DOWN += 5;
	}
	else if (menu_left == 0)
	{
		if (count1 <= 10)
			count1++;
		if (flag1 == 0)
			LEFT = 1, beep(1000, 2);
		flag1 = 1;
		if (count1 >= 10 && LEFT < 20)
			LEFT += 5;
	}
	else if (menu_right == 0)
	{
		if (count1 <= 10)
			count1++;
		if (flag1 == 0)
			RIGHT = 1, beep(1000, 2);
		flag1 = 1;
		if (count1 >= 10 && RIGHT < 20)
			RIGHT += 5;
	}
	else
	{

		flag1 = 0;
		if (count1)
			led_time = led_time0, LCD_BLK_Set();
		count1 = 0;
		if (UP != 10)
			UP = 0;
		if (DOWN != 10)
			DOWN = 0;
		if (LEFT != 10)
			LEFT = 0;
		if (RIGHT != 10)
			RIGHT = 0;
	}

	if (menu_esc == 0)
	{
		if (count2 <= 10)
			count2++;
		if (flag2 == 0)
			ESC = 1, beep(1000, 2);
		flag2 = 1;
		if (count2 >= 10 && ESC < 20)
			ESC += 5;
	}
	else if (menu_ok == 0)
	{
		if (count2 <= 10)
			count2++;
		if (flag2 == 0)
			OK = 1, beep(1000, 2);
		flag2 = 1;
		if (count2 >= 10 && OK < 20)
			OK += 5;
	}
	else
	{
		if (count2)
			led_time = led_time0, LCD_BLK_Set();
		count2 = 0;
		flag2 = 0;
		if (OK)
			OK_release = OK;
		if (ESC)
			ESC_release = ESC;
		ESC = 0;
		OK = 0;
	}

	if (lcd_dir) // 屏幕翻转了
	{
		u8 temp = LEFT;
		LEFT = RIGHT;
		RIGHT = temp;
		temp = UP;
		UP = DOWN;
		DOWN = temp;
	}
}
