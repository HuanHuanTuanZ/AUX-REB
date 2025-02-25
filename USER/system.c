#include "common.h"
#include "stmflash.h"
#include "XUI.h"
#include "usart.h"
// #include "lcd.h"
#include "ips.h"
#include "string.h"

void save16(u16 addr, u16 v)
{
	u16 tmp[1];
	tmp[0] = v;
	STMFLASH_Write(FLASH_SAVE_ADDR + addr * 2, tmp, 1);
}

void save32(u16 addr, u32 v)
{
	u16 tmp[2];
	tmp[0] = v >> 16;
	tmp[1] = v;
	STMFLASH_Write(FLASH_SAVE_ADDR + addr * 2, tmp, 2);
}

u16 read16(u16 addr)
{
	return Read_StmflashData(FLASH_SAVE_ADDR + addr * 2);
}

u32 read32(u16 addr)
{
	u16 v1 = Read_StmflashData(FLASH_SAVE_ADDR + addr * 2);
	u16 v2 = Read_StmflashData(FLASH_SAVE_ADDR + (addr + 1) * 2);
	return v1 << 16 | v2;
}

#define model_param_max 172
void model_reset()
{
	u8 i;
	u16 temp[model_param_max];
	u32 addr = FLASH_SAVE_ADDR + model_current * model_size * 2 + 200;
	save16(model_addr + model_size - 1, 1);
	for (i = 0; i < 16; i++) // 通道输入参数   64   0-63
	{
		temp[i] = i;
		temp[i + 16] = 0;
		temp[i + 32] = 0;
		temp[i + 48] = 0;
	}
	for (i = 0; i < 4; i++) // 混控参数   16   64-79
	{
		temp[i + 64] = 0;
		temp[i + 68] = 0;
		temp[i + 72] = 0;
		temp[i + 76] = 0;
	}
	for (i = 0; i < 16; i++) // 通道输出参数  64   80-143
	{
		temp[i + 80] = i;
		temp[i + 96] = 100;
		temp[i + 112] = 0;
		temp[i + 128] = 100;
	}
	for (i = 0; i < 4; i++) // 逻辑开关参数  20   144-163
	{
		temp[i + 144] = 0;
		temp[i + 148] = 0;
		temp[i + 152] = 0;
		temp[i + 156] = 0;
		temp[i + 160] = 0;
	}
	STMFLASH_Write(addr, temp, model_param_max);
	sprintf(model_name, "Model%02d", model_current + 1);
	model_name_save(model_addr + 164); // 164-171
}

void model_read() // 通道数据读取
{
	u8 i;
	if (read16(model_addr + model_size - 1) != 1)
		model_reset();
	for (i = 0; i < 16; i++)
	{
		ch_map[i] = read16(model_addr + i);
		ch_min[i] = read16(model_addr + 16 + i);
		ch_mid[i] = read16(model_addr + 32 + i);
		ch_max[i] = read16(model_addr + 48 + i);
	}
	for (i = 0; i < 4; i++)
	{
		mixer_type[i] = read16(model_addr + 64 + i);
		mixer_in1[i] = read16(model_addr + 68 + i);
		mixer_in2[i] = read16(model_addr + 72 + i);
		mixer_rate[i] = read16(model_addr + 76 + i);
	}
	for (i = 0; i < 16; i++)
	{
		output_map[i] = read16(model_addr + 80 + i);
		output_min[i] = read16(model_addr + 96 + i);
		ch_rev[i] = read16(model_addr + 112 + i);
		output_max[i] = read16(model_addr + 128 + i);
	}
	for (i = 0; i < 4; i++)
	{
		logic_condition[i] = read16(model_addr + 144 + i);
		logic_a[i] = read16(model_addr + 148 + i);
		logic_x_value[i] = read16(model_addr + 152 + i);
		logic_last_time[i] = read16(model_addr + 156 + i);
		logic_mode[i] = read16(model_addr + 160 + i);
	}
	model_name_read(model_addr + 164); // 164-171
}

#define system_param_max 35
void factory() // 初始化
{
	u8 i;
	u16 temp[system_param_max];
	temp[0] = aux_version; // 软件版本
	temp[1] = 5;		   // 界面主题
	temp[2] = 0;		   // 模型选择
	temp[3] = 0;		   // 远程遥控
	temp[4] = 0;		   // 默认界面
	temp[5] = 0;		   // 自动背光
	temp[6] = 60;		   // 背光时间
	temp[7] = read16(7);   /////////////////////7
	temp[8] = 0;		   // ADC输入
	temp[9] = 9;

	for (i = 0; i < 4; i++)
	{
		temp[10 + i] = 0;
		temp[14 + i] = 2047;
		temp[18 + i] = 4095;
	}
	temp[22] = 2;	// 输入信号类型
	temp[23] = 1;	// MAV输出
	temp[24] = 499; // 电压校准
	temp[25] = 1;	// 蜂鸣器
	temp[26] = 0;	// UI背景类型
	temp[27] = 0;	// 屏幕翻转
	temp[28] = 0;	// 屏幕镜像
	temp[29] = 0;	// HUD类型
	temp[30] = 0;	// 串口2功能
	temp[31] = 0;	// HUD TYPE
	temp[32] = 0;
	temp[33] = 0;
	temp[34] = 1152; // MAVLINK波特率
	STMFLASH_Write(FLASH_SAVE_ADDR, temp, system_param_max);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*---------------------------------读取设置---------------------------------------------------------------------------*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void system_read()
{
	u8 i;
	hardware_id = *(vu32 *)(0x1FFFF7E8) + *(vu32 *)(0x1FFFF7EC) + *(vu32 *)(0x1FFFF7F0);
	if (read16(0) != aux_version)
		factory(); // 首次运行初始化
	theme = read16(1);
	model_current = read16(2);
	remote_ctrl = read16(3);
	boot_menu = read16(4);
	led_enable = read16(5);
	led_time0 = read16(6);
	admin = read16(7);
	adc_enable = read16(8);
	elrs_tlm = read16(9);

	for (i = 0; i < 4; i++) // 10-21
	{
		adc_min[i] = read16(10 + i);
		adc_mid[i] = read16(14 + i);
		adc_max[i] = read16(18 + i);
	}
	input_type = read16(22);
	mavlink_enable = read16(23);
	bat_vol = read16(24);
	beep_enable = read16(25);
	bkg_type = read16(26);
	lcd_dir = read16(27);
	lcd_mirror = read16(28);
	hud_data_type = read16(29);
	usart2_function = read16(30);
	hud_type = read16(31);
	hud_type2 = read16(32);
	hud_type3 = read16(33);
	mavlink_buad = read16(34);

	theme_change(theme);
	model_read();
}

void model_name_save(int16 addr)
{
	u16 temp[8];
	u8 i;
	for (i = 0; i < 8; i++)
		temp[i] = model_name[2 * i] << 8 | model_name[2 * i + 1];
	STMFLASH_Write(FLASH_SAVE_ADDR + addr * 2, temp, 8);
}

void model_name_read(int16 addr)
{
	u16 temp[8];
	u8 i;
	STMFLASH_Read(FLASH_SAVE_ADDR + addr * 2, temp, 8);
	for (i = 0; i < 8; i++)
	{
		model_name[2 * i] = temp[i] >> 8;
		model_name[2 * i + 1] = temp[i];
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
void XUI_bar_string(u16 x0, u16 y0, u16 dx, u16 dy, u8 r, u8 color1, u8 color2, char *p, u8 size)
{
	u8 len = strlen(p);
	XUI_bar(x0, y0, dx, dy, r, color2);
	XUI_str(x0 - len * size / 4 + 1, y0 - size / 2 + 1, 100, 40, (u8 *)p, size, 0, color1, color2);
	XUI_bar0(x0, y0, dx, dy, r, color1);
}

char num_temp[16];
float input_num(float value, char *p)
{
	u8 len = 0;
	u8 flag = 200;
	u8 seq = 0;
	XUI_bar(80, 40, 52, 37, 3, BC);
	XUI_bar0(80, 40, 52, 37, 3, PC);
	XUI_str(80 - strlen(p) * 3, 6, 98, 12, (u8 *)p, 12, 0, PC, BC);
	if (first_splash)
	{
		first_splash = 0;
		px = py = 0;
		memset(num_temp, 0, 16);
		if (value != 0)
		{
			sprintf(num_temp, "%g", value);
			len = strlen(num_temp);
		}
		else
			len = 0;
	}
	len = strlen(num_temp);
	seq = py * 5 + px + 1;
	XUI_bar_string(40, 40, 9, 6, 3, PC, seq == 1 ? RC : BC, "1", 12);
	XUI_bar_string(60, 40, 9, 6, 3, PC, seq == 2 ? RC : BC, "2", 12);
	XUI_bar_string(80, 40, 9, 6, 3, PC, seq == 3 ? RC : BC, "3", 12);
	XUI_bar_string(100, 40, 9, 6, 3, PC, seq == 4 ? RC : BC, "4", 12);
	XUI_bar_string(120, 40, 9, 6, 3, PC, seq == 5 ? RC : BC, "C", 12);
	XUI_bar_string(40, 54, 9, 6, 3, PC, seq == 6 ? RC : BC, "5", 12);
	XUI_bar_string(60, 54, 9, 6, 3, PC, seq == 7 ? RC : BC, "6", 12);
	XUI_bar_string(80, 54, 9, 6, 3, PC, seq == 8 ? RC : BC, "7", 12);
	XUI_bar_string(100, 54, 9, 6, 3, PC, seq == 9 ? RC : BC, "8", 12);
	XUI_bar_string(120, 54, 9, 6, 3, PC, seq == 10 ? RC : BC, "X", 12);
	XUI_bar_string(40, 68, 9, 6, 3, PC, seq == 11 ? RC : BC, "9", 12);
	XUI_bar_string(60, 68, 9, 6, 3, PC, seq == 12 ? RC : BC, "0", 12);
	XUI_bar_string(80, 68, 9, 6, 3, PC, seq == 13 ? RC : BC, ".", 12);
	XUI_bar_string(100, 68, 9, 6, 3, PC, seq == 14 ? RC : BC, "-", 12);
	XUI_bar_string(120, 68, 9, 6, 3, PC, seq == 15 ? RC : BC, "O", 12);
	if (UP)
	{
		UP = 0;
		py += py == 0 ? 2 : -1;
	}
	if (DOWN)
	{
		DOWN = 0;
		py += py == 2 ? -2 : 1;
	}
	if (LEFT)
	{
		LEFT = 0;
		px += px == 0 ? 4 : -1;
	}
	if (RIGHT)
	{
		RIGHT = 0;
		px += px == 4 ? -4 : 1;
	}
	if (OK_release == 1)
	{
		OK_release = 0;
		flag = seq;
	}
	if (ESC_release == 1)
	{
		ESC_release = 0;
		flag = 5;
	}
	if (flag && flag < 20)
	{
		switch (flag)
		{
		case 1:
			num_temp[len++] = '1';
			break;
		case 2:
			num_temp[len++] = '2';
			break;
		case 3:
			num_temp[len++] = '3';
			break;
		case 4:
			num_temp[len++] = '4';
			break;
		case 5:
			if (len)
				len--;
			num_temp[len] = '\0';
			break;
		case 6:
			num_temp[len++] = '5';
			break;
		case 7:
			num_temp[len++] = '6';
			break;
		case 8:
			num_temp[len++] = '7';
			break;
		case 9:
			num_temp[len++] = '8';
			break;
		case 10:
			flag = 100;
			break;
		case 11:
			num_temp[len++] = '9';
			break;
		case 12:
			num_temp[len++] = '0';
			break;
		case 13:
			num_temp[len++] = '.';
			break;
		case 14:
			num_temp[len++] = '-';
			break;
		case 15:
			flag = 101;
			break;
		}
		if (flag < 99)
			flag = 99;
	}
	XUI_str(80 - len * 3, 20, 98, 12, (u8 *)num_temp, 12, 0, PC, BC);
	XUI_bar0(80, 26, 49, 6, 3, PC); // 28--80   52   10
	if (OK_release >= 20 || flag == 101)
	{
		sscanf((char *)num_temp, "%f", &value);
		menu3 = 0;
		OK_release = 0;
		return value;
	}
	if (ESC_release >= 20 || flag == 100)
	{
		menu3 = 0;
		ESC_release = 0;
	}
	return 65535;
}

void version_info()
{
	char temp[20];
	if (first_splash == 1)
	{
		first_splash = 0;
		menu3 = 0;
	}
	XUI_str(56, 4, 64, 12, (u8 *)"版本信息", 12, 1, PC, BC);
	XUI_str(38, 20, 160, 24, (u8 *)"XRC-AUX", 24, 1, PC, BC);
	XUI_str(17, 44, 160, 12, (u8 *)"Designed by Xuan-Yuan", 12, 1, PC, BC);
	sprintf(temp, "Ver:%1.2f", aux_version / 100.0f);
	XUI_str(20, 56, 160, 12, (u8 *)temp, 12, 1, PC, BC);
	sprintf(temp, "CPU:%dMHz", CPU_SPEED);
	XUI_str(84, 56, 160, 12, (u8 *)temp, 12, 1, PC, BC);
	sprintf(temp, "ID:%X", hardware_id);
	XUI_str(10, 68, 160, 12, (u8 *)temp, 12, 1, PC, BC);
	XUI_str(80, 68, 160, 12, (u8 *)"QQ:1104639376", 12, 1, PC, BC);

	if (menu3 == 0)
	{
		if (OK == 1)
		{
			OK = 0;
			menu3 = 1;
			first_splash = 2;
		}
	}
	else if (menu3 == 1)
	{
		float code = input_num(0, "Admin Code");
		if (code == 8023)
		{
			admin = 1;
			save16(7, admin);
			message("OK", 20);
		}
		else if (code != 65535)
			message("ERROR", 20);
	}
}

///////////////////////////////////////////////////////////////////////////////////////
void logo()
{
	u8 i = 0;
	theme = read16(1);
	theme_change(theme);
	while (1)
	{
		if (i == 0)
		{
			char temp[10];
#if BKG == 0
			XUI_clear(BC); // 清除屏幕
#else
#if DMA
			XUI_bmp16_160_80(bkg);
#else
			XUI_clear(BC); // 清除屏幕
#endif
#endif
			XUI_str(38, 8, 160, 24, (u8 *)"XRC-AUX", 24, 1, PC, BC);
			XUI_str(17, 32, 160, 12, (u8 *)"Designed by Xuan-Yuan", 12, 1, PC, BC);
			sprintf(temp, "Ver:%1.2f", aux_version / 100.0f);
			XUI_str(59, 44, 160, 12, (u8 *)temp, 12, 1, PC, BC);
			XUI_reflash(); // 刷新屏幕
			delay_ms(200); //
			LCD_BLK_Set(); // 打开背光
			system_read(); // 系统数据读取
		}
		else
		{
			XUI_rectangle(10, 60, 150, 70, PC);
			XUI_fill(12, 62, 12 + i, 68, PC);
			// XUI_reflash();//刷新屏幕
			XUI_reflash_part(0, 60, 159, 11); // 刷新屏幕
		}
		if (i < 136)
			i += 2;
		else
			break;
	}
}

u8 message_time = 0;
char message_str[32];

void message_reflash()
{
	if (message_time)
	{
		XUI_bar(80, 40, 56, 8, 4, BC);
		XUI_bar0(80, 40, 56, 8, 4, PC);
		XUI_str(80 - strlen(message_str) * 3, 34, 100, 12, (u8 *)message_str, 12, 1, PC, BC);
	}
}

void message(char *p, u8 time)
{
	message_time = time;
	strcpy(message_str, p);
}

#include "mpu6050.h"

void link_status_check()
{
	if (sbus_status)
	{
		sbus_status--;
		if (sbus_connected == 0)
		{
			sbus_connected = 1;
			message("SBUS已连接", 20);
			beep_positive();
		}
	}
	else
	{
		if (sbus_connected)
		{
			sbus_connected = 0;
			message("SBUS已断开", 20);
			beep_negative();
		}
	}
	////////////////////////////////////////////////////////////////////////////////////////
	if (ppm_status)
	{
		ppm_status--;
		if (ppm_connected == 0)
		{
			ppm_connected = 1;
			message("PPM已连接", 20);
			beep_positive();
		}
	}
	else
	{
		if (ppm_connected)
		{
			ppm_connected = 0;
			message("PPM已断开", 20);
			beep_negative();
		}
	}
	////////////////////////////////////////////////////////////////////////////////////////
	if (CRSF_rate[14])
	{
		if (elrs_connected == 0)
		{
			crsf_param_buf = elrs_tlm - 1;
			crsf_param_flag = 2;
			message("ELRS-TX已连接", 20);
			beep_positive();
		}
		elrs_connected = 3;
	}
	else
	{
		if (elrs_connected)
		{
			elrs_connected--;
			if (elrs_connected == 0)
				message("ELRS-TX已断开", 20);
			beep_negative();
			// crsf_clear();
		}
	}
	////////////////////////////////////////////////////////////////////////////////////////
	if (tele_flag)
	{
		if (elrs_back_connected == 0)
		{
			elrs_back_connected = 1;
			first_splash = 1;
			message("ELRS回传已连接", 20);
			beep_positive();
		}
	}
	else
	{
		if (elrs_back_connected)
		{
			elrs_back_connected = 0;
			message("ELRS回传已断开", 20);
			beep_negative();
		}
	}

	if (mavlink_flag)
	{
		if (mavlink_connected == 0)
		{
			mavlink_connected = 1;
			first_splash = 1;
			message("Mavlink已连接", 20);
			beep_positive();
		}
	}
	else
	{
		if (mavlink_connected)
		{
			mavlink_connected = 0;
			message("Mavlink已断开", 20);
			beep_negative();
		}
	}

	if (usart2_function == 1)
	{
		if (gyro_connected == 0)
		{
			MPU6050_Init();
			if (GetData(ACCEL_ZOUT_H) != -1)
			{
				gyro_connected = 1;
				first_splash = 1;
				message("Mpu6050已连接", 20);
				beep_positive();
			}
		}
	}
}
