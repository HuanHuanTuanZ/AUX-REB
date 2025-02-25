// #include "open_tel_mavlink.h"
#include "common.h"
// #include "lcd.h"
#include "XUI.h"
#include "usart.h"
#include "math.h"

#define pi 3.141592f

void XUI_data1()
{
	char temp[20];

	x_center = 80;
	y_center = 40;
	x_min = 48;
	y_min = 12;
	x_max = 111;
	y_max = 67;

	XUI_3d(3.1416f - hud_pitch * pi / 180.0, 0, 3.1416f - hud_roll * pi / 180.0f, 40, PC);

	statusbar();
	XUI_str(52, 2, 64, 16, (u8 *)"HUD姿态", 16, 1, PC, BC);

	XUI_str(0, 20, 64, 12, (u8 *)"高:", 12, 1, FC, BC);
	XUI_str(0, 32, 64, 12, (u8 *)"速:", 12, 1, FC, BC);
	XUI_str(0, 44, 64, 12, (u8 *)"压:", 12, 1, FC, BC);
	XUI_str(0, 56, 64, 12, (u8 *)"流:", 12, 1, FC, BC);

	XUI_str(112, 20, 64, 12, (u8 *)"俯:", 12, 1, FC, BC);
	XUI_str(112, 32, 64, 12, (u8 *)"横:", 12, 1, FC, BC);
	XUI_str(112, 44, 64, 12, (u8 *)"偏:", 12, 1, FC, BC);
	XUI_str(112, 56, 64, 12, (u8 *)"模:", 12, 1, FC, BC);

	XUI_str(0, 68, 64, 12, (u8 *)"经:", 12, 1, FC, BC);
	XUI_str(80, 68, 64, 12, (u8 *)"纬:", 12, 1, FC, BC);

	if (hud_alt < 100)
	{
		sprintf(temp, "%4.1fM", hud_alt / 10.0);
		XUI_str(18, 20, 30, 12, (u8 *)temp, 12, 1, PC, BC);
	}
	else
		XUI_num(18, 20, hud_alt / 10, 4, 12, 1, PC, BC);

	sprintf(temp, "%2.1fkH", hud_groundspeed / 10.0f);
	XUI_str(18, 32, 30, 12, (u8 *)temp, 12, 1, PC, BC);

	sprintf(temp, "%4.1fV", hud_bat / 100.0);
	XUI_str(18, 44, 30, 12, (u8 *)temp, 12, 1, PC, BC);
	sprintf(temp, "%4.1fA", hud_cur / 10.0);
	XUI_str(18, 56, 30, 12, (u8 *)temp, 12, 1, PC, BC);

	XUI_num(130, 20, hud_pitch, len(hud_pitch), 12, 1, PC, BC);
	XUI_num(130, 32, hud_roll, len(hud_roll), 12, 1, PC, BC);

	XUI_num(130, 44, hud_yaw, len(hud_yaw), 12, 1, PC, BC);
	XUI_num(130, 56, crsf_flight_mode, 3, 12, 1, PC, BC);
	sprintf(temp, "%9.5fE", crsf_longitude / 10000000.0);
	XUI_str(18, 68, 60, 12, (u8 *)temp, 12, 1, PC, BC);
	sprintf(temp, "%9.5fN", crsf_latitude / 10000000.0);
	XUI_str(98, 68, 60, 12, (u8 *)temp, 12, 1, PC, BC);
}

const char elrs_speed[][8] = {
	"未连接  ",
	"F1000Hz ",
	" F500Hz ",
	" D500Hz ",
	" D250Hz ",
	" 500Hz  ",
	"333 Full",
	" 250Hz  ",
	" 150Hz  ",
	"100 Full",
	"  50Hz  "};

const char elrs_power[][8] = {
	"未连接  ",
	"  10mW  ",
	"  25mW  ",
	"  50mW  ",
	" 100mW  ",
	" 250mW  ",
	" 500mW  ",
	" 1000mW ",
	" 2000mW "};

const char elrs_tlm_index[][5] = {
	"-----",
	"  Std",
	"  Off",
	"1:128",
	" 1:64",
	" 1:32",
	" 1:16",
	"  1:8",
	"  1:4",
	"  1:2",
	" Race",
};

void XUI_data2()
{
	//	u8 tlm0=0;
	char temp[20];
	statusbar();

	XUI_str(48, 2, 64, 16, (u8 *)"连接信息", 16, 1, PC, BC);

	XUI_str(0, 20, 100, 12, (u8 *)"TRSSI:", 12, 1, FC, BC);
	XUI_str(0, 32, 100, 12, (u8 *)"TX_LQ:", 12, 1, FC, BC);
	XUI_str(0, 44, 100, 12, (u8 *)"TX_RT:", 12, 1, FC, BC);
	XUI_str(0, 56, 100, 12, (u8 *)"TX_PR:", 12, 1, FC, BC);

	XUI_str(80, 20, 100, 12, (u8 *)"RRSSI:", 12, 1, FC, BC);
	XUI_str(80, 32, 100, 12, (u8 *)"RX_LQ:", 12, 1, FC, BC);
	XUI_str(92, 44, 100, 12, (u8 *)"TLM:", 12, 1, FC, BC);

	if (crsf_tx_rssi)
	{
		sprintf(temp, "%3ddB", crsf_tx_rssi - 255);
		XUI_str(36, 20, 100, 12, (u8 *)temp, 12, 1, PC, BC);
	}
	else
		XUI_str(36, 20, 100, 12, (u8 *)"未连接", 12, 1, PC, BC);

	if (crsf_tx_lq)
	{
		sprintf(temp, "%3d%%", crsf_tx_lq);
		XUI_str(48, 32, 100, 12, (u8 *)temp, 12, 1, PC, BC);
	}
	else
		XUI_str(36, 32, 100, 12, (u8 *)"未连接", 12, 1, PC, BC);

	XUI_str(36, 44, 48, 12, (u8 *)elrs_speed[elrs_rate], 12, 1, PC, BC);
	XUI_str(36, 56, 48, 12, (u8 *)elrs_power[elrs_pwr], 12, 1, PC, BC);

	if (crsf_rx_rssi)
	{
		sprintf(temp, "%3ddB", crsf_rx_rssi - 255);
		XUI_str(116, 20, 100, 12, (u8 *)temp, 12, 1, PC, BC);
	}
	else
		XUI_str(116, 20, 100, 12, (u8 *)"未连接", 12, 1, PC, BC);

	if (crsf_rx_lq)
	{
		sprintf(temp, "%3d%%", crsf_rx_lq);
		XUI_str(128, 32, 100, 12, (u8 *)temp, 12, 1, PC, BC);
	}
	else
		XUI_str(116, 32, 100, 12, (u8 *)"未连接", 12, 1, PC, BC);

	XUI_str(122, 44, 30, 12, (u8 *)elrs_tlm_index[elrs_tlm], 12, 1, PC, BC);
}

void XUI_data3()
{
	statusbar();
	XUI_str(48, 2, 64, 16, (u8 *)"其它信息", 16, 1, PC, BC);

	XUI_str(0, 20, 100, 12, (u8 *)"BAT_CAPC:", 12, 1, FC, BC);
	XUI_str(0, 32, 100, 12, (u8 *)"BAT_REMN:", 12, 1, FC, BC);
	XUI_str(0, 44, 100, 12, (u8 *)"BARO_ALT:", 12, 1, FC, BC);
	XUI_str(0, 56, 100, 12, (u8 *)"VER_SPED:", 12, 1, FC, BC);
	XUI_str(0, 68, 100, 12, (u8 *)"GPS_NUMS:", 12, 1, FC, BC);

	XUI_num(54, 20, crsf_capacity, 4, 12, 1, PC, BC);
	XUI_num(54, 32, crsf_remaining, 4, 12, 1, PC, BC);
	XUI_num(54, 44, crsf_baro_altitude, 4, 12, 1, PC, BC);
	XUI_num(54, 56, crsf_vertical_speed, 4, 12, 1, PC, BC);
	XUI_num(54, 68, crsf_satellites, 4, 12, 1, PC, BC);

	XUI_str(80, 20, 100, 12, (u8 *)"RX_PERC:", 12, 1, FC, BC);
	XUI_str(80, 32, 100, 12, (u8 *)"TX_POWR:", 12, 1, FC, BC);
	XUI_str(80, 44, 100, 12, (u8 *)"TX_PERC:", 12, 1, FC, BC);
	XUI_str(80, 56, 100, 12, (u8 *)"RX_POWE:", 12, 1, FC, BC);
	XUI_str(80, 68, 100, 12, (u8 *)"TX_FPS:", 12, 1, FC, BC);

	XUI_num(134, 20, crsf_rx_rssi_perc, 4, 12, 1, PC, BC);
	XUI_num(134, 32, crsf_tx_rf_power, 4, 12, 1, PC, BC);
	XUI_num(134, 44, crsf_tx_rssi_perc, 4, 12, 1, PC, BC);
	XUI_num(134, 56, crsf_rx_rf_power, 4, 12, 1, PC, BC);
	XUI_num(134, 68, crsf_tx_fps, 4, 12, 1, PC, BC);
}
extern const u8 crsf_id[15];

void crsf_test()
{
	u8 i = 0;
	statusbar();
	XUI_str(52, 2, 72, 16, (u8 *)"CRSF_ID", 16, 1, PC, BC);

	// XUI_num8(0,0,crsf_update_interval,2,1,RED8,BC);
	// XUI_num8(80,0,crsf_offset,2,1,RED8,BC);

	XUI_str(0, 20, 100, 12, (u8 *)"ID", 12, 1, PC, BC);
	XUI_str(20, 20, 100, 12, (u8 *)"HZ", 12, 1, PC, BC);
	XUI_str(50, 20, 100, 12, (u8 *)"ID", 12, 1, PC, BC);
	XUI_str(70, 20, 100, 12, (u8 *)"HZ", 12, 1, PC, BC);
	XUI_str(100, 20, 100, 12, (u8 *)"ID", 12, 1, PC, BC);
	XUI_str(120, 20, 100, 12, (u8 *)"HZ", 12, 1, PC, BC);
	for (i = 0; i < 5; i++)
	{
		XUI_num16_8(0, 32 + 8 * i, crsf_id[i], 2, 1, PC, BC);
		XUI_num16_8(50, 32 + 8 * i, crsf_id[i + 5], 2, 1, PC, BC);
		XUI_num16_8(100, 32 + 8 * i, crsf_id[i + 10], 2, 1, PC, BC);
		if (CRSF_rate[i])
			XUI_num8(20, 32 + 8 * i, CRSF_rate[i], 2, 1, RED8, BC);
		else
			XUI_string8(20, 32 + 8 * i, (u8 *)"--", 1, PC, BC);
		if (CRSF_rate[i + 5])
			XUI_num8(70, 32 + 8 * i, CRSF_rate[i + 5], 2, 1, RED8, BC);
		else
			XUI_string8(70, 32 + 8 * i, (u8 *)"--", 1, PC, BC);
		if (CRSF_rate[i + 10])
			XUI_num8(120, 32 + 8 * i, CRSF_rate[i + 10], 2, 1, RED8, BC);
		else
			XUI_string8(120, 32 + 8 * i, (u8 *)"--", 1, PC, BC);
	}
}
