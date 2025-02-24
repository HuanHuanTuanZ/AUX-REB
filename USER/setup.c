#include "common.h"
#include "XUI.h"
#include "usart.h"
#include "usart3.h"

extern void LCD_dir(u8 dir, u8 mirror);

const u8 work_icon[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x63, 0x00, 0x00, 0x00, 0xC6, 0x00, 0x1C, 0x00, 0xCC, 0x00, 0x1C, 0x00, 0xCC, 0x08,
	0x1E, 0x00, 0xCC, 0x1C, 0x07, 0x00, 0xCC, 0x3C, 0x03, 0x80, 0xCF, 0xEC, 0x01, 0xC0, 0xCF, 0xCC, 0x00, 0xED, 0x80, 0x0C, 0x00, 0x7B, 0x00, 0x18, 0x00, 0x36, 0x0F, 0xF0, 0x00, 0x6C, 0x1F, 0xE0,
	0x00, 0x58, 0x30, 0x00, 0x00, 0x30, 0x60, 0x00, 0x00, 0x60, 0xD8, 0x00, 0x00, 0xC1, 0xBC, 0x00, 0x01, 0x83, 0x66, 0x00, 0x03, 0x06, 0xC3, 0x00, 0x06, 0x0D, 0x81, 0x80, 0x0C, 0x18, 0xC0, 0xC0,
	0x18, 0x30, 0x60, 0x60, 0x18, 0x60, 0x30, 0x60, 0x18, 0xC0, 0x18, 0x60, 0x1F, 0x80, 0x0F, 0xE0, 0x1F, 0x00, 0x07, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; /*"?????",0*/

const u8 input_icon[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x7F, 0xFF, 0xFC, 0x06, 0x3F, 0xE0, 0x04, 0x07, 0x1F, 0xE0, 0x04,
	0x07, 0x8F, 0xFF, 0xFC, 0x06, 0xC0, 0x00, 0x00, 0x06, 0x60, 0x00, 0x00, 0x3E, 0x33, 0xFF, 0xFC, 0x3E, 0x19, 0xFF, 0x04, 0x30, 0x0C, 0xFF, 0x04, 0x30, 0x06, 0x7F, 0xFC, 0x30, 0x03, 0x00, 0x00,
	0x30, 0x03, 0x00, 0x00, 0x30, 0x06, 0x7F, 0xFC, 0x30, 0x0C, 0xFF, 0x04, 0x3E, 0x19, 0xFF, 0x04, 0x3E, 0x33, 0xFF, 0xFC, 0x06, 0x60, 0x00, 0x00, 0x06, 0xC0, 0x00, 0x00, 0x07, 0x8F, 0xFF, 0xFC,
	0x07, 0x1F, 0xE0, 0x04, 0x06, 0x3F, 0xE0, 0x04, 0x04, 0x7F, 0xFF, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*"?????",0*/
};

const u8 model_icon[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x7C, 0x00, 0x01, 0xF0, 0x3F, 0xC0, 0x03, 0xF0, 0x1F, 0xF8, 0x07, 0xF0, 0x0F, 0xFF, 0x0F, 0xE0, 0x07, 0xFF, 0xFF, 0xC0,
	0x01, 0xFF, 0xFF, 0x80, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x3F, 0xFE, 0x00, 0x00, 0x1F, 0xFC, 0x00, 0x00, 0x07, 0xFA, 0x00, 0x00, 0x07, 0xF6, 0x00, 0x00, 0x0F, 0xEE, 0x00, 0x00, 0x1F, 0xDE, 0x00,
	0x00, 0x3F, 0xBE, 0x00, 0x00, 0x7F, 0x7F, 0x00, 0x20, 0xFE, 0x3F, 0x00, 0x7F, 0xFC, 0x1F, 0x00, 0x3F, 0xF8, 0x1F, 0x80, 0x1F, 0xF0, 0x0F, 0x80, 0x0F, 0xE0, 0x07, 0x80, 0x03, 0xC0, 0x07, 0xC0,
	0x01, 0xC0, 0x03, 0xC0, 0x01, 0xC0, 0x01, 0xC0, 0x00, 0xE0, 0x01, 0xC0, 0x00, 0x40, 0x00, 0xC0, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; /*"C:\Users\86134\Desktop\???.bmp",0*/

const u8 output_icon[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0xFF, 0xD0, 0x00, 0x3F, 0xF0, 0x58, 0x00, 0x3F, 0xF0, 0x5C, 0x00,
	0x3F, 0xFF, 0xDE, 0x00, 0x00, 0x00, 0x1B, 0x00, 0x00, 0x00, 0x19, 0x80, 0x3F, 0xFE, 0xF8, 0xC0, 0x3F, 0x02, 0xF8, 0x60, 0x3F, 0x02, 0xC0, 0x30, 0x3F, 0xFE, 0xC0, 0x18, 0x00, 0x00, 0xC0, 0x0C,
	0x00, 0x00, 0xC0, 0x0C, 0x3F, 0xFE, 0xC0, 0x18, 0x3F, 0x02, 0xC0, 0x30, 0x3F, 0x02, 0xF8, 0x60, 0x3F, 0xFE, 0xF8, 0xC0, 0x00, 0x00, 0x19, 0x80, 0x00, 0x00, 0x1B, 0x00, 0x3F, 0xFF, 0xDE, 0x00,
	0x3F, 0xF0, 0x5C, 0x00, 0x3F, 0xF0, 0x58, 0x00, 0x3F, 0xFF, 0xD0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*"?????",0*/
};

const u8 logic_icon[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xFF, 0xF0, 0x70, 0x10, 0x1F, 0xF8, 0xF8, 0x20, 0x0F, 0xFD, 0xFC, 0x20, 0x0F, 0xFD, 0xFC,
	0x20, 0x0F, 0xFD, 0xFC, 0x10, 0x1F, 0xF8, 0xF8, 0x0F, 0xFF, 0xF0, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E, 0x0F, 0xFF, 0xF0, 0x1F, 0x1F, 0xF8, 0x08, 0x3F, 0xBF, 0xF0, 0x04,
	0x3F, 0xBF, 0xF0, 0x04, 0x3F, 0xBF, 0xF0, 0x0C, 0x1F, 0x1F, 0xF8, 0x08, 0x0E, 0x0F, 0xFF, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xFF, 0xF0, 0x70, 0x1F, 0xF0, 0x08, 0xF8,
	0x3F, 0xE0, 0x05, 0xFC, 0x3F, 0xE0, 0x05, 0xFC, 0x3F, 0xE0, 0x05, 0xFC, 0x1F, 0xF0, 0x08, 0xF8, 0x0F, 0xFF, 0xF0, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; /*"C:\Users\86134\Desktop\???.bmp",0*/

const u8 setup_icon[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xE0, 0x00, 0x00, 0x87, 0xE1, 0x00, 0x01, 0xDF, 0xFB, 0x80, 0x03, 0xFF, 0xFF, 0xC0, 0x07, 0xFF, 0xFF, 0xE0,
	0x0F, 0xFF, 0xFF, 0xF0, 0x07, 0xFF, 0xFF, 0xE0, 0x03, 0xFF, 0xFF, 0xC0, 0x07, 0xF8, 0x1F, 0xE0, 0x07, 0xF0, 0x0F, 0xE0, 0x1F, 0xE0, 0x07, 0xF8, 0x1F, 0xE0, 0x07, 0xF8, 0x1F, 0xE0, 0x07, 0xF8,
	0x1F, 0xE0, 0x07, 0xF8, 0x1F, 0xE0, 0x07, 0xF8, 0x1F, 0xE0, 0x07, 0xF8, 0x07, 0xF0, 0x0F, 0xE0, 0x07, 0xF8, 0x1F, 0xE0, 0x03, 0xFF, 0xFF, 0xC0, 0x07, 0xFF, 0xFF, 0xE0, 0x0F, 0xFF, 0xFF, 0xF0,
	0x07, 0xFF, 0xFF, 0xE0, 0x03, 0xFF, 0xFF, 0xC0, 0x01, 0xDF, 0xFB, 0x80, 0x00, 0x87, 0xE1, 0x00, 0x00, 0x07, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; /*"C:\Users\86134\Desktop\???.bmp",0*/

// const u8 mix_icon[]={
// 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0xE0,0x07,0xF8,0x1F,0xE0,0x07,0xF8,0x18,0x30,0x0C,0x18,0x18,0x30,0x0C,0x18,0x18,0x18,0x18,0x18,
// 0x18,0x18,0x18,0x18,0x18,0x0C,0x30,0x18,0x1E,0x0C,0x30,0x78,0x07,0x86,0x61,0xE0,0x01,0xE6,0x67,0x80,0x00,0x7B,0xDE,0x00,0x00,0x1F,0xF8,0x00,0x00,0x07,0xE0,0x00,
// 0x00,0x07,0xE0,0x00,0x00,0x1F,0xF8,0x00,0x00,0x7B,0xDE,0x00,0x01,0xE6,0x67,0x80,0x07,0x86,0x61,0xE0,0x1E,0x0C,0x30,0x78,0x18,0x0C,0x30,0x18,0x18,0x18,0x18,0x18,
// 0x18,0x18,0x18,0x18,0x18,0x30,0x0C,0x18,0x18,0x30,0x0C,0x18,0x1F,0xE0,0x07,0xF8,0x1F,0xE0,0x07,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};/*"?????",0*/
//

const u8 adjust_icon[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xF0, 0x00, 0x00, 0x3F, 0xFC, 0x00, 0x00, 0xF8, 0x1F, 0x00, 0x01, 0xE0, 0x07, 0x80, 0x03, 0x80, 0x01, 0xC0, 0x07, 0x01, 0x80, 0xE0,
	0x0E, 0x03, 0xC0, 0x70, 0x0C, 0x07, 0xE0, 0x30, 0x1C, 0x0F, 0xF0, 0x38, 0x18, 0x01, 0x80, 0x18, 0x38, 0x21, 0x84, 0x1C, 0x30, 0x61, 0x86, 0x0C, 0x30, 0xE1, 0x87, 0x0C, 0x31, 0xFF, 0xFF, 0x8C,
	0x31, 0xFF, 0xFF, 0x8C, 0x30, 0xE1, 0x87, 0x0C, 0x30, 0x61, 0x86, 0x0C, 0x38, 0x21, 0x84, 0x1C, 0x18, 0x01, 0x80, 0x18, 0x18, 0x0F, 0xF0, 0x38, 0x0C, 0x07, 0xE0, 0x30, 0x0E, 0x03, 0xC0, 0x70,
	0x06, 0x01, 0x80, 0xE0, 0x03, 0x00, 0x01, 0xC0, 0x01, 0xC0, 0x03, 0x80, 0x00, 0xF8, 0x1F, 0x00, 0x00, 0x3F, 0xFC, 0x00, 0x00, 0x0F, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; /*"C:\Users\86134\Desktop\???.bmp",0*/

// const u8 elrs_setup_icon[]={
// 0x00,0x00,0x00,0x00,0x00,0x0F,0xF8,0x00,0x00,0x3F,0xFE,0x00,0x00,0xE0,0x03,0x80,0x01,0x87,0xF0,0xC0,0x03,0x1F,0xFC,0x60,0x02,0x30,0x06,0x20,0x00,0x63,0xE3,0x00,
// 0x00,0x46,0x31,0x00,0x00,0x0C,0x98,0x00,0x03,0x01,0xC0,0x60,0x04,0x80,0x80,0x90,0x08,0xBF,0xFE,0x88,0x09,0x00,0x00,0x48,0x12,0x0F,0xF8,0x24,0x12,0xE8,0x0B,0xA4,
// 0x12,0xA8,0x0A,0xA4,0x26,0xE8,0x0B,0x92,0x24,0x0F,0xF8,0x12,0x25,0xE0,0x03,0xD2,0x24,0x10,0x04,0x12,0x22,0xC9,0xC9,0xA2,0x23,0x28,0x8A,0x62,0x26,0x28,0x8A,0x32,
// 0x20,0x2B,0xEA,0x02,0x20,0x48,0x09,0x02,0x20,0x90,0x04,0x82,0x21,0x67,0xF3,0x42,0x21,0x04,0x10,0x42,0x21,0x3F,0xFE,0x42,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};/*"?????",0*/

const u8 version_icon[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xFF, 0xFF, 0x80, 0x07, 0xFF, 0xFF, 0xE0, 0x0F, 0xFF, 0xFF, 0xF0, 0x0F, 0xF8, 0x1F, 0xF0, 0x1F, 0xF0, 0x0F, 0xF8,
	0x1F, 0xE0, 0x07, 0xF8, 0x1F, 0xE0, 0x07, 0xF8, 0x1F, 0xE0, 0x07, 0xF8, 0x1F, 0xE0, 0x07, 0xF8, 0x1F, 0xE0, 0x07, 0xF8, 0x1F, 0xF0, 0x0F, 0xF8, 0x1F, 0xF0, 0x0F, 0xF8, 0x1F, 0xF8, 0x1F, 0xF8,
	0x1F, 0xF8, 0x1F, 0xF8, 0x1F, 0xFC, 0x3F, 0xF8, 0x1F, 0xFC, 0x3F, 0xF8, 0x1F, 0xFE, 0x7F, 0xF8, 0x1F, 0xFF, 0xFF, 0xF8, 0x1F, 0xFF, 0xFF, 0xF8, 0x1F, 0xFE, 0x7F, 0xF8, 0x1F, 0xFC, 0x3F, 0xF8,
	0x1F, 0xFC, 0x3F, 0xF8, 0x0F, 0xFE, 0x7F, 0xF0, 0x0F, 0xFF, 0xFF, 0xF0, 0x07, 0xFF, 0xFF, 0xE0, 0x01, 0xFF, 0xFF, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; /*"?????",0*/

const u8 ui_battery_icon[] = {
	0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFC, 0x80, 0x00, 0x00, 0x04, 0x80, 0x00, 0x00, 0x04, 0x80, 0x00, 0x00, 0x07, 0x80, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x01,
	0x80, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x07, 0x80, 0x00, 0x00, 0x04, 0x80, 0x00, 0x00, 0x04, 0xFF, 0xFF, 0xFF, 0xFC, 0x00, 0x00, 0x00, 0x00}; /*"?????",0*/

const unsigned char download_icon[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7E, 0x0C, 0x7E, 0x0C, 0x18, 0x0C, 0x18, 0x6C, 0x18, 0x6C, 0x18, 0x6C, 0x18, 0x6C, 0x1B, 0x6C, 0x1B, 0x6C, 0x1B, 0x6C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*"?????",0*/
};

const u8 ui_setup_index[8][8] = {
	"模型设置",
	"信号输入",
	"数据混控",
	"通道输出",
	"逻辑开关",
	"系统参数",
	"摇杆校准",
	"版本信息",
};
const u8 *ui_setup_icon[8] = {
	work_icon,
	input_icon,
	model_icon,
	output_icon,
	logic_icon,
	setup_icon,
	adjust_icon,
	version_icon,
};

extern u8 dir; // 上1 下2 左3 右4

int16 step = 0;

void XUI_setup_menu()
{
	u8 sx, ex;
	XUI_str(48, 2, 64, 16, (u8 *)"系统设置", 16, 1, PC, BC);
#if USE_animation == 0

	XUI_bar0(80, 40, 18, 18, 5, PC);
	XUI_bar0(80, 76, 76, 3, 3, PC);

	sx = 4 + 152 * ui_select_flag / 8;
	ex = 4 + 152 * (ui_select_flag + 1) / 8;

	XUI_bar((sx + ex) / 2, 76, 9, 3, 3, PC);

	if (ui_select_flag == 0)
	{
		XUI_bmp1(12, 24, 32, 32, ui_setup_icon[7], 1, PC, BC);
		XUI_str(4, 60, 48, 12, (u8 *)ui_setup_index[7], 12, 1, PC, BC);
		XUI_bmp1(64, 24, 32, 32, ui_setup_icon[0], 1, PC, BC);
		XUI_str(56, 60, 48, 12, (u8 *)ui_setup_index[0], 12, 1, PC, BC);
		XUI_bmp1(116, 24, 32, 32, ui_setup_icon[1], 1, PC, BC);
		XUI_str(108, 60, 48, 12, (u8 *)ui_setup_index[1], 12, 1, PC, BC);
	}
	else if (ui_select_flag == 7)
	{
		XUI_bmp1(12, 24, 32, 32, ui_setup_icon[6], 1, PC, BC);
		XUI_str(4, 60, 48, 12, (u8 *)ui_setup_index[6], 12, 1, PC, BC);
		XUI_bmp1(64, 24, 32, 32, ui_setup_icon[7], 1, PC, BC);
		XUI_str(56, 60, 48, 12, (u8 *)ui_setup_index[7], 12, 1, PC, BC);
		XUI_bmp1(116, 24, 32, 32, ui_setup_icon[0], 1, PC, BC);
		XUI_str(108, 60, 48, 12, (u8 *)ui_setup_index[0], 12, 1, PC, BC);
	}
	else
	{
		XUI_bmp1(12, 24, 32, 32, ui_setup_icon[ui_select_flag - 1], 1, PC, BC);
		XUI_str(4, 60, 48, 12, (u8 *)ui_setup_index[ui_select_flag - 1], 12, 1, PC, BC);
		XUI_bmp1(64, 24, 32, 32, ui_setup_icon[ui_select_flag], 1, PC, BC);
		XUI_str(56, 60, 48, 12, (u8 *)ui_setup_index[ui_select_flag], 12, 1, PC, BC);
		XUI_bmp1(116, 24, 32, 32, ui_setup_icon[ui_select_flag + 1], 1, PC, BC);
		XUI_str(108, 60, 48, 12, (u8 *)ui_setup_index[ui_select_flag + 1], 12, 1, PC, BC);
	}
#else

	XUI_bar0(80, 40, 18, 18, 5, PC);
	XUI_bar0(80, 76, 76, 3, 3, PC);

	if (dir == 16) // 退出设置
	{

		if (step < 52)
			step += 8;
		else
		{
			dir = 0;
			step = 0;
			if (ui_select_flag)
				ui_select_flag--;
			else
				ui_select_flag = 7;
		}
	}
	else if (dir == 17) // 退出设置
	{
		if (step > -52)
			step -= 8;
		else
		{
			dir = 0;
			step = 0;
			if (ui_select_flag < 7)
				ui_select_flag++;
			else
				ui_select_flag = 0;
		}
	}

	sx = 4 + 152 * ui_select_flag / 8 - step / 3;
	ex = 4 + 152 * (ui_select_flag + 1) / 8 - step / 3;
	XUI_bar((sx + ex) / 2, 76, 9, 3, 3, PC);

	if (ui_select_flag == 0)
	{
		XUI_bmp1(step + 12, 24, 32, 32, ui_setup_icon[7], 1, PC, BC);
		XUI_str(step + 4, 60, 48, 12, (u8 *)ui_setup_index[7], 12, 1, PC, BC);
		XUI_bmp1(step + 64, 24, 32, 32, ui_setup_icon[0], 1, PC, BC);
		XUI_str(step + 56, 60, 48, 12, (u8 *)ui_setup_index[0], 12, 1, PC, BC);
		XUI_bmp1(step + 116, 24, 32, 32, ui_setup_icon[1], 1, PC, BC);
		XUI_str(step + 108, 60, 48, 12, (u8 *)ui_setup_index[1], 12, 1, PC, BC);
		if (dir == 16) // 左
		{
			XUI_bmp1(step + 12 - 52, 24, 32, 32, ui_setup_icon[6], 1, PC, BC);
			XUI_str(step + 4 - 52, 60, 48, 12, (u8 *)ui_setup_index[6], 12, 1, PC, BC);
		}
		else if (dir == 17) // 右
		{
			XUI_bmp1(step + 116 + 52, 24, 32, 32, ui_setup_icon[2], 1, PC, BC);
			XUI_str(step + 108 + 52, 60, 48, 12, (u8 *)ui_setup_index[2], 12, 1, PC, BC);
		}
	}
	else if (ui_select_flag == 1)
	{
		XUI_bmp1(step + 12, 24, 32, 32, ui_setup_icon[0], 1, PC, BC);
		XUI_str(step + 4, 60, 48, 12, (u8 *)ui_setup_index[0], 12, 1, PC, BC);
		XUI_bmp1(step + 64, 24, 32, 32, ui_setup_icon[1], 1, PC, BC);
		XUI_str(step + 56, 60, 48, 12, (u8 *)ui_setup_index[1], 12, 1, PC, BC);
		XUI_bmp1(step + 116, 24, 32, 32, ui_setup_icon[2], 1, PC, BC);
		XUI_str(step + 108, 60, 48, 12, (u8 *)ui_setup_index[2], 12, 1, PC, BC);
		if (dir == 16) // 左
		{
			XUI_bmp1(step + 12 - 52, 24, 32, 32, ui_setup_icon[7], 1, PC, BC);
			XUI_str(step + 4 - 52, 60, 48, 12, (u8 *)ui_setup_index[7], 12, 1, PC, BC);
		}
		else if (dir == 17) // 右
		{
			XUI_bmp1(step + 116 + 52, 24, 32, 32, ui_setup_icon[3], 1, PC, BC);
			XUI_str(step + 108 + 52, 60, 48, 12, (u8 *)ui_setup_index[3], 12, 1, PC, BC);
		}
	}
	else if (ui_select_flag == 6)
	{
		XUI_bmp1(step + 12, 24, 32, 32, ui_setup_icon[5], 1, PC, BC);
		XUI_str(step + 4, 60, 48, 12, (u8 *)ui_setup_index[5], 12, 1, PC, BC);
		XUI_bmp1(step + 64, 24, 32, 32, ui_setup_icon[6], 1, PC, BC);
		XUI_str(step + 56, 60, 48, 12, (u8 *)ui_setup_index[6], 12, 1, PC, BC);
		XUI_bmp1(step + 116, 24, 32, 32, ui_setup_icon[7], 1, PC, BC);
		XUI_str(step + 108, 60, 48, 12, (u8 *)ui_setup_index[7], 12, 1, PC, BC);
		if (dir == 16) // 左
		{
			XUI_bmp1(step + 12 - 52, 24, 32, 32, ui_setup_icon[4], 1, PC, BC);
			XUI_str(step + 4 - 52, 60, 48, 12, (u8 *)ui_setup_index[4], 12, 1, PC, BC);
		}
		else if (dir == 17) // 右
		{
			XUI_bmp1(step + 116 + 52, 24, 32, 32, ui_setup_icon[0], 1, PC, BC);
			XUI_str(step + 108 + 52, 60, 48, 12, (u8 *)ui_setup_index[0], 12, 1, PC, BC);
		}
	}
	else if (ui_select_flag == 7)
	{
		XUI_bmp1(step + 12, 24, 32, 32, ui_setup_icon[6], 1, PC, BC);
		XUI_str(step + 4, 60, 48, 12, (u8 *)ui_setup_index[6], 12, 1, PC, BC);
		XUI_bmp1(step + 64, 24, 32, 32, ui_setup_icon[7], 1, PC, BC);
		XUI_str(step + 56, 60, 48, 12, (u8 *)ui_setup_index[7], 12, 1, PC, BC);
		XUI_bmp1(step + 116, 24, 32, 32, ui_setup_icon[0], 1, PC, BC);
		XUI_str(step + 108, 60, 48, 12, (u8 *)ui_setup_index[0], 12, 1, PC, BC);
		if (dir == 16) // 左
		{
			XUI_bmp1(step + 12 - 52, 24, 32, 32, ui_setup_icon[5], 1, PC, BC);
			XUI_str(step + 4 - 52, 60, 48, 12, (u8 *)ui_setup_index[5], 12, 1, PC, BC);
		}
		else if (dir == 17) // 右
		{
			XUI_bmp1(step + 116 + 52, 24, 32, 32, ui_setup_icon[1], 1, PC, BC);
			XUI_str(step + 108 + 52, 60, 48, 12, (u8 *)ui_setup_index[1], 12, 1, PC, BC);
		}
	}
	else
	{
		XUI_bmp1(step + 12, 24, 32, 32, ui_setup_icon[ui_select_flag - 1], 1, PC, BC);
		XUI_str(step + 4, 60, 48, 12, (u8 *)ui_setup_index[ui_select_flag - 1], 12, 1, PC, BC);
		XUI_bmp1(step + 64, 24, 32, 32, ui_setup_icon[ui_select_flag], 1, PC, BC);
		XUI_str(step + 56, 60, 48, 12, (u8 *)ui_setup_index[ui_select_flag], 12, 1, PC, BC);
		XUI_bmp1(step + 116, 24, 32, 32, ui_setup_icon[ui_select_flag + 1], 1, PC, BC);
		XUI_str(step + 108, 60, 48, 12, (u8 *)ui_setup_index[ui_select_flag + 1], 12, 1, PC, BC);
		if (dir == 16) // 左
		{
			XUI_bmp1(step + 12 - 52, 24, 32, 32, ui_setup_icon[ui_select_flag - 2], 1, PC, BC);
			XUI_str(step + 4 - 52, 60, 48, 12, (u8 *)ui_setup_index[ui_select_flag - 2], 12, 1, PC, BC);
		}
		else if (dir == 17) // 右
		{
			XUI_bmp1(step + 116 + 52, 24, 32, 32, ui_setup_icon[ui_select_flag + 2], 1, PC, BC);
			XUI_str(step + 108 + 52, 60, 48, 12, (u8 *)ui_setup_index[ui_select_flag + 2], 12, 1, PC, BC);
		}
	}

#endif
}

#define setup_count 13
#define setup_x_count 2
#define setup_y_count 5

const char setup_index1[][8] = {
	{"ELRS功率"},
	{"ELRS速率"},
	{"ELRS-TLM"},
	{"SW_mode "},
	{"Bind    "},
	{"BT_joy  "},
	{"Wifi    "},
	{"VT_admin"},
	{"蜂鸣器  "},
	{"背景类型"},
	{"屏幕翻转"},
	{"屏幕镜像"},
	{"--------"},
};

const char setup_index2[][8] = {
	{"界面主题"},
	{"自动背光"},
	{"背光时间"},
	{"远程控制"},
	{"默认界面"},
	{"出厂设置"},
	{"ADC输入 "},
	{"输入信号"},
	{"Mavlink "},
	{"电压校准"},
	{"HUD数据 "},
	{"串口功能"},
	{"串口速率"},

};

const char elrs_setup_speed[][5] = {
	"   NC",
	"F1000",
	" F500",
	" D500",
	" D250",
	"  500",
	"333Fu",
	"  250",
	"  150",
	"100Fu",
	"   50",
};

const char elrs_setup_power[][5] = {
	"   NC",
	" 10mW",
	" 25mW",
	" 50mW",
	"100mW",
	"250mW",
	"500mW",
	"1000m",
	"2000m"};

const char elrs_setup_tlm[][5] = {
	"   NC",
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

const u8 theme_index[][2] = {"黑", "白", "红", "橙", "黄", "绿", "青", "蓝", "紫"};

void theme_change(u8 seq)
{
	switch (seq)
	{
	case 0:
		PC = BLACK1;
		BC = BLACK2;
		RC = BLACK3;
		SC = BLACK4;
		FC = BLACK5;
		break;
	case 1:
		PC = WHITE1;
		BC = WHITE2;
		RC = WHITE3;
		SC = WHITE4;
		FC = WHITE5;
		break;
	case 2:
		PC = RED1;
		BC = RED2;
		RC = RED3;
		SC = RED4;
		FC = RED5;
		break;
	case 3:
		PC = ORANGE1;
		BC = ORANGE2;
		RC = ORANGE3;
		SC = ORANGE4;
		FC = ORANGE5;
		break;
	case 4:
		PC = YELLOW1;
		BC = YELLOW2;
		RC = YELLOW3;
		SC = YELLOW4;
		FC = YELLOW5;
		break;
	case 5:
		PC = GREEN1;
		BC = GREEN2;
		RC = GREEN3;
		SC = GREEN4;
		FC = GREEN5;
		break;
	case 6:
		PC = CYAN1;
		BC = CYAN2;
		RC = CYAN3;
		SC = CYAN4;
		FC = CYAN5;
		break;
	case 7:
		PC = BLUE1;
		BC = BLUE2;
		RC = BLUE3;
		SC = BLUE4;
		FC = BLUE5;
		break;
	case 8:
		PC = PURPLE1;
		BC = PURPLE2;
		RC = PURPLE3;
		SC = PURPLE4;
		FC = PURPLE5;
		break;
	}
}

void draw_setup_info1(u8 i)
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

	XUI_str(0, 20 + i * 12, 48, 12, (u8 *)setup_index1[seq], 12, px == 0 ? mode : 1, px == 0 ? point_color : PC, px == 0 ? back_color : BC);
	switch (seq)
	{
	case 0:
		if (elrs_connected)
			XUI_str(48, 20 + i * 12, 30, 12, (u8 *)elrs_setup_power[param[0][seq]], 12, px == 0 ? mode : 1, px == 0 ? point_color : PC, px == 0 ? back_color : BC);
		else
			XUI_str(48, 20 + i * 12, 30, 12, (u8 *)elrs_setup_power[0], 12, px == 0 ? mode : 1, px == 0 ? point_color : PC, px == 0 ? back_color : BC);
		break;
	case 1:
		if (elrs_connected)
			XUI_str(48, 20 + i * 12, 30, 12, (u8 *)elrs_setup_speed[param[0][seq]], 12, px == 0 ? mode : 1, px == 0 ? point_color : PC, px == 0 ? back_color : BC);
		else
			XUI_str(48, 20 + i * 12, 30, 12, (u8 *)elrs_setup_speed[0], 12, px == 0 ? mode : 1, px == 0 ? point_color : PC, px == 0 ? back_color : BC);
		break;
	case 2:
		if (elrs_connected)
			XUI_str(48, 20 + i * 12, 30, 12, (u8 *)elrs_setup_tlm[param[0][seq]], 12, px == 0 ? mode : 1, px == 0 ? point_color : PC, px == 0 ? back_color : BC);
		else
			XUI_str(48, 20 + i * 12, 30, 12, (u8 *)elrs_setup_tlm[0], 12, px == 0 ? mode : 1, px == 0 ? point_color : PC, px == 0 ? back_color : BC);
		break;
	case 4:
	case 5:
	case 6:
	case 7:
		XUI_str(48, 20 + i * 12, 30, 12, (u8 *)" 禁用", 12, px == 0 ? mode : 1, px == 0 ? point_color : PC, px == 0 ? back_color : BC);
		break;
	case 8:
		if (param[0][seq] == 0)
			XUI_str(48, 20 + i * 12, 30, 12, (u8 *)"   关", 12, px == 0 ? mode : 1, px == 0 ? point_color : PC, px == 0 ? back_color : BC);
		else
			XUI_str(48, 20 + i * 12, 30, 12, (u8 *)"   开", 12, px == 0 ? mode : 1, px == 0 ? point_color : PC, px == 0 ? back_color : BC);
		break;
	case 9:
		if (param[0][seq] == 0)
			XUI_str(48, 20 + i * 12, 30, 12, (u8 *)" 图片", 12, px == 0 ? mode : 1, px == 0 ? point_color : PC, px == 0 ? back_color : BC);
		else if (param[0][seq] == 1)
			XUI_str(48, 20 + i * 12, 30, 12, (u8 *)" 纯色", 12, px == 0 ? mode : 1, px == 0 ? point_color : PC, px == 0 ? back_color : BC);
		else if (param[0][seq] == 2)
			XUI_str(48, 20 + i * 12, 30, 12, (u8 *)" 姿态", 12, px == 0 ? mode : 1, px == 0 ? point_color : PC, px == 0 ? back_color : BC);
		break;
	case 10:
	case 11:
		if (param[0][seq] == 0)
			XUI_str(48, 20 + i * 12, 30, 12, (u8 *)"   正", 12, px == 0 ? mode : 1, px == 0 ? point_color : PC, px == 0 ? back_color : BC);
		else
			XUI_str(48, 20 + i * 12, 30, 12, (u8 *)"   反", 12, px == 0 ? mode : 1, px == 0 ? point_color : PC, px == 0 ? back_color : BC);
		break;

	case 12:
		XUI_str(48, 20 + i * 12, 30, 12, (u8 *)"-----", 12, px == 0 ? mode : 1, px == 0 ? point_color : PC, px == 0 ? back_color : BC);
		break;

	default:
		XUI_num(48, 20 + i * 12, param[0][seq], 5, 12, px == 0 ? mode : 1, px == 0 ? point_color : PC, px == 0 ? back_color : BC);
		break;
	}
}

extern const char logic_source_index[][4];
const char desk_index[][5] = {"DESK1", "DESK2", " HUD1", " HUD2", " HUD3", " TOOL", "SETUP"};
u8 boot_menu = 0; // 启动默认界面
void draw_setup_info2(u8 i)
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
	XUI_str(80, 20 + i * 12, 48, 12, (u8 *)setup_index2[seq], 12, px == 1 ? mode : 1, px == 1 ? point_color : PC, px == 1 ? back_color : BC);
	switch (seq)
	{
	case 0:
		XUI_str(128, 20 + i * 12, 30, 12, (u8 *)"   ", 12, px == 1 ? mode : 1, px == 1 ? point_color : PC, px == 1 ? back_color : BC);
		XUI_str(146, 20 + i * 12, 12, 12, (u8 *)theme_index[param[1][0]], 12, px == 1 ? mode : 1, px == 1 ? point_color : PC, px == 1 ? back_color : BC);
		break;
#if MAVLINK
	case 8:
#else
	case 8:
		XUI_str(128, 20 + i * 12, 30, 12, (u8 *)"-----", 12, px == 1 ? mode : 1, px == 1 ? point_color : PC, px == 1 ? back_color : BC);
		break;
#endif
	case 1:
	case 6:
		if (param[1][seq] == 0)
			XUI_str(128, 20 + i * 12, 30, 12, (u8 *)"   关", 12, px == 1 ? mode : 1, px == 1 ? point_color : PC, px == 1 ? back_color : BC);
		else
			XUI_str(128, 20 + i * 12, 30, 12, (u8 *)"   开", 12, px == 1 ? mode : 1, px == 1 ? point_color : PC, px == 1 ? back_color : BC);
		break;
	case 2:
	{
		char temp[10];
		sprintf(temp, "%4dS", param[1][seq]);
		XUI_str(128, 20 + i * 12, 30, 12, (u8 *)temp, 12, px == 1 ? mode : 1, px == 1 ? point_color : PC, px == 1 ? back_color : BC);
		break;
	}
	case 3:
		if (param[1][seq] == 0)
			XUI_str(128, 20 + i * 12, 30, 12, (u8 *)"   关", 12, px == 1 ? mode : 1, px == 1 ? point_color : PC, px == 1 ? back_color : BC);
		else
			XUI_str(134, 20 + i * 12, 24, 12, (u8 *)logic_source_index[param[1][seq]], 12, px == 1 ? mode : 1, px == 1 ? point_color : PC, px == 1 ? back_color : BC);
		break;
	case 4:
		XUI_str(128, 20 + i * 12, 30, 12, (u8 *)desk_index[param[1][seq]], 12, px == 1 ? mode : 1, px == 1 ? point_color : PC, px == 1 ? back_color : BC);
		break;
	case 5:
		XUI_str(128, 20 + i * 12, 30, 12, (u8 *)"   OK", 12, px == 1 ? mode : 1, px == 1 ? point_color : PC, px == 1 ? back_color : BC);
		break;
	case 7:
		if (param[1][seq] == 0)
			XUI_str(128, 20 + i * 12, 30, 12, (u8 *)"  ADC", 12, px == 1 ? mode : 1, px == 1 ? point_color : PC, px == 1 ? back_color : BC);
		else if (param[1][seq] == 1)
			XUI_str(128, 20 + i * 12, 30, 12, (u8 *)"  PPM", 12, px == 1 ? mode : 1, px == 1 ? point_color : PC, px == 1 ? back_color : BC);
		else if (param[1][seq] == 2)
			XUI_str(128, 20 + i * 12, 30, 12, (u8 *)" SBUS", 12, px == 1 ? mode : 1, px == 1 ? point_color : PC, px == 1 ? back_color : BC);
		break;
	case 9:
	{
		char temp[20];
		sprintf(temp, "%2.2fV", param[1][seq] / 1000.0f);
		XUI_str(128, 20 + i * 12, 30, 12, (u8 *)temp, 12, px == 1 ? mode : 1, px == 1 ? point_color : PC, px == 1 ? back_color : BC);
	}
	break;

	case 10:
		if (param[1][seq] == 0)
			XUI_str(128, 20 + i * 12, 30, 12, (u8 *)" 自动", 12, px == 1 ? mode : 1, px == 1 ? point_color : PC, px == 1 ? back_color : BC);
		else if (param[1][seq] == 1)
			XUI_str(128, 20 + i * 12, 30, 12, (u8 *)" ELRS", 12, px == 1 ? mode : 1, px == 1 ? point_color : PC, px == 1 ? back_color : BC);
		else if (param[1][seq] == 2)
			XUI_str(128, 20 + i * 12, 30, 12, (u8 *)" MAVL", 12, px == 1 ? mode : 1, px == 1 ? point_color : PC, px == 1 ? back_color : BC);
		else if (param[1][seq] == 3)
			XUI_str(128, 20 + i * 12, 30, 12, (u8 *)" 陀螺", 12, px == 1 ? mode : 1, px == 1 ? point_color : PC, px == 1 ? back_color : BC);
		else if (param[1][seq] == 4)
			XUI_str(128, 20 + i * 12, 30, 12, (u8 *)" 通道", 12, px == 1 ? mode : 1, px == 1 ? point_color : PC, px == 1 ? back_color : BC);
		break;

	case 11:
		if (param[1][seq] == 0)
			XUI_str(128, 20 + i * 12, 30, 12, (u8 *)" Mavl", 12, px == 1 ? mode : 1, px == 1 ? point_color : PC, px == 1 ? back_color : BC);
		else if (param[1][seq] == 1)
			XUI_str(128, 20 + i * 12, 30, 12, (u8 *)" 陀螺", 12, px == 1 ? mode : 1, px == 1 ? point_color : PC, px == 1 ? back_color : BC);
		break;

	case 12:
		if (param[1][seq] == 576)
			XUI_str(128, 20 + i * 12, 30, 12, (u8 *)"57600", 12, px == 1 ? mode : 1, px == 1 ? point_color : PC, px == 1 ? back_color : BC);
		else if (param[1][seq] == 1152)
			XUI_str(128, 20 + i * 12, 30, 12, (u8 *)"11520", 12, px == 1 ? mode : 1, px == 1 ? point_color : PC, px == 1 ? back_color : BC);
		break;

	default:
		XUI_num(128, 20 + i * 12, param[1][seq], 5, 12, px == 1 ? mode : 1, px == 1 ? point_color : PC, px == 1 ? back_color : BC);
		break;
	}
}

// count--参数总数
// max----一个页面显示的最大数目
// menu0--第一个项目的序号

void index_bar(u8 count, u8 max, u8 menu0, u8 y0, u8 y1)
{
	u16 sy = y0;
	u16 ey;
	if (menu0 == 0)
	{
		if (max > count)
			max = count;
	}
	else
		sy = y0 + (y1 - y0) * (menu0) / count;
	ey = y0 + (y1 - y0) * (menu0 + max) / count;
	XUI_fill(158, sy, 159, ey, PC);
}

void system_setup()
{
	u8 i;
	XUI_str(56, 4, 64, 12, (u8 *)"系统参数", 12, 1, PC, RC);
	if (first_splash == 1)
	{
		first_splash = 0;
		if (setup_count > setup_y_count)
			menu_len = setup_y_count;
		else
			menu_len = setup_count;
		px = py = 0;
		menu_y = 0;
		param[0][0] = elrs_pwr;
		param[0][1] = elrs_rate;
		param[0][2] = elrs_tlm;
		param[0][8] = beep_enable;
		param[0][9] = bkg_type;
		param[0][10] = lcd_dir;
		param[0][11] = lcd_mirror;

		param[1][0] = theme;
		param[1][1] = led_enable;
		param[1][2] = led_time0;
		param[1][3] = remote_ctrl;
		param[1][4] = boot_menu;
		param[1][6] = adc_enable;
		param[1][7] = input_type;

		param[1][8] = mavlink_enable;
		param[1][9] = battery;
		param[1][10] = hud_data_type;
		param[1][11] = usart2_function;
		param[1][12] = mavlink_buad;
	}

	for (i = 0; i < menu_len; i++) // 全部刷新
	{
		draw_setup_info1(i);
		draw_setup_info2(i);
	}

	if (menu3 == 0)
	{
		if (LEFT)
		{
			LEFT = 0;
			if (px)
				px--;
			else
				px = setup_x_count - 1;
		}
		if (RIGHT)
		{
			RIGHT = 0;
			if (px < (setup_x_count - 1))
				px++;
			else
				px = 0;
		}

		if (param_edit) // 数据正在编辑
		{
			if (ESC || OK)
			{
				ESC = 0;
				OK = 0;
				param_edit = 0;
				switch (save_flag)
				{
				case 1:
					save16(24, bat_vol);
					break;
				case 2:
					save16(25, beep_enable);
					break;

				default:
					break;
				}
				save_flag = 0;
				// 差保存参数的程序
			}

			if (DOWN || UP)
			{
				if (px == 0)
				{
					switch (py)
					{
					case 0:
						if (UP)
						{
							if (param[px][py] < 8)
								param[px][py]++;
						}
						if (DOWN)
						{
							if (param[px][py] > 1)
								param[px][py]--;
						}
						if (param[px][py])
						{
							crsf_param_buf = param[px][py] - 1;
							crsf_param_flag = 3;
						}
						break;
					case 1:
						if (UP)
						{
							if (param[px][py] < 10)
								param[px][py]++;
						}
						if (DOWN)
						{
							if (param[px][py] > 1)
								param[px][py]--;
						}
						if (param[px][py])
						{
							crsf_param_buf = 10 - param[px][py];
							crsf_param_flag = 1;
						}
						break;
					case 2:
						if (UP)
						{
							if (param[px][py] < 10)
								param[px][py]++;
						}
						if (DOWN)
						{
							if (param[px][py] > 1)
								param[px][py]--;
						}
						elrs_tlm = param[px][py];
						save16(9, elrs_tlm);
						if (param[px][py])
						{
							crsf_param_buf = param[px][py] - 1;
							crsf_param_flag = 2;
						}
						break;
					case 3:
						param[px][py] = !param[px][py];
						crsf_param_buf = param[px][py];
						crsf_param_flag = 4;
						break;
					case 8:
						param[px][py] = !param[px][py];
						beep_enable = param[px][py];
						save_flag = 2;
						break;
					case 9:
						if (UP)
						{
							if (param[px][py] < 2)
								param[px][py]++;
						}
						if (DOWN)
						{
							if (param[px][py])
								param[px][py]--;
						}
						bkg_type = param[px][py];
						save16(26, bkg_type);
						break;
					case 10:
						param[px][py] = !param[px][py];
						lcd_dir = param[px][py];
						save16(27, lcd_dir);
						LCD_dir(lcd_dir, lcd_mirror);
						break;
					case 11:
						param[px][py] = !param[px][py];
						lcd_mirror = param[px][py];
						save16(28, lcd_mirror);
						LCD_dir(lcd_dir, lcd_mirror);
						break;
					}
				}
				else
				{
					switch (py)
					{
					case 0:
						if (UP)
						{
							if (param[px][py] < 8)
								param[px][py]++;
							else
								param[px][py] = 0;
						}
						if (DOWN)
						{
							if (param[px][py])
								param[px][py]--;
							else
								param[px][py] = 8;
						}
						theme = param[px][py];
						theme_change(theme);
						save16(1, theme);
						break;
					case 1:
						param[px][py] = !param[px][py];
						led_enable = param[px][py];
						save16(5, led_enable);
						break;
					case 2:
						if (UP)
						{
							if (param[px][py] < 3600)
								param[px][py] += UP;
							if (param[px][py] >= 3600)
								param[px][py] = 3600;
						}
						if (DOWN)
						{
							if (param[px][py] > 10)
								param[px][py] -= DOWN;
							if (param[px][py] <= 10)
								param[px][py] = 10;
						}
						led_time0 = param[px][py];
						led_time = led_time0;
						save16(6, led_time0);
						break;
					case 3:
						if (UP)
						{
							if (param[px][py] < 48)
								param[px][py]++;
						}
						if (DOWN)
						{
							if (param[px][py])
								param[px][py]--;
						}
						remote_ctrl = param[px][py];
						save16(3, remote_ctrl);
						break;
					case 4:
						if (UP)
						{
							if (param[px][py] < 6)
								param[px][py]++;
							else
								param[px][py] = 0;
						}
						if (DOWN)
						{
							if (param[px][py])
								param[px][py]--;
							else
								param[px][py] = 6;
						}
						boot_menu = param[px][py];
						save16(4, boot_menu);
						break;
					case 6:
						param[px][py] = !param[px][py];
						adc_enable = param[px][py];
						save16(8, adc_enable);
						break;
					case 7:
						if (UP)
						{
							if (param[px][py] < 2)
								param[px][py]++;
							else
								param[px][py] = 0;
						}
						if (DOWN)
						{
							if (param[px][py])
								param[px][py]--;
							else
								param[px][py] = 2;
						}
						input_type = param[px][py];
						for (i = 0; i < 16; i++)
						{
							if (input_type == 0)
								ch_map[i] = i + input_type * 8;
							else
								ch_map[i] = i + input_type * 8 + 2;

							save16(model_addr + i, ch_map[i]);
						}
						save16(22, input_type);
						break;
					case 8:
						param[px][py] = !param[px][py];
						mavlink_enable = param[px][py];
						save16(23, mavlink_enable);
						break;
					case 9:
						if (UP)
							param[px][py] += UP * 10;
						if (DOWN)
						{
							if (param[px][py] > 1000)
								param[px][py] -= DOWN * 10;
						}
						bat_vol = param[px][py] * 100 / bat_adc;
						save_flag = 1;
						save_time = 5;
						break;
					case 10:
						if (UP)
						{
							if (param[px][py] < 4)
								param[px][py]++;
						}
						if (DOWN)
						{
							if (param[px][py])
								param[px][py]--;
						}
						hud_data_type = param[px][py];
						save16(29, hud_data_type);
						break;
					case 11:
						if (UP)
						{
							if (param[px][py] < 1)
								param[px][py]++;
						}
						if (DOWN)
						{
							if (param[px][py])
								param[px][py]--;
						}
						usart2_function = param[px][py];
						save16(30, usart2_function);
						if (usart2_function == 0)
						{
#if MAVLINK
							usart2_init(mavlink_buad * 100); // 串口2初始化为57600
							mavlink_init();					 // 初始化Mavlink
							request_mavlink();				 // 请求MAVLINK数据
#endif
						}
						break;
					case 12:
						if (param[px][py] == 576)
							param[px][py] = 1152;
						else if (param[px][py] == 1152)
							param[px][py] = 576;
						mavlink_buad = param[px][py];
						save16(34, mavlink_buad);

						if (usart2_function == 0)
						{
#if MAVLINK
							usart2_init(mavlink_buad * 100); // 串口2初始化为57600
							mavlink_init();					 // 初始化Mavlink
							request_mavlink();				 // 请求MAVLINK数据
#endif
						}
						break;
					}
				}
				UP = 0;
				DOWN = 0;
			}
			index_bar(setup_count, 5, menu_y, 20, 80);
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
					py = setup_count - 1;
					if (setup_count > setup_y_count)
						menu_y = setup_count - setup_y_count;
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
				if (py >= setup_count)
				{
					py = 0;
					menu_y = 0;
				}
				else if (py >= (menu_y + setup_y_count))
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
				if (px == 1 && py == 5)
					menu3 = 1;
				else if (px == 0 && py == 4)
					message("Disable", 20); // crsf_param_flag=5;
				else if (px == 0 && py == 5)
					message("Disable", 20); // crsf_param_flag=6;
				else if (px == 0 && py == 6)
					message("Disable", 20); // crsf_param_flag=7;
				else if (px == 0 && py == 7)
					message("Disable", 20); // crsf_param_flag=7;
				else
					param_edit = 1;
			}
			index_bar(setup_count, 5, menu_y, 20, 80);
		}
	}
	else if (menu3 == 1)
	{
		if (check_ok("Factory?"))
		{
			factory();
			param[1][0] = theme;
			system_read();
			message("RESET OK", 20);
		}
	}
}

extern u16 adc_min[8];
extern u16 adc_mid[8];
extern u16 adc_max[8];
extern u16 hardware_input[8];

u8 adjust_t = 0;
u8 adjust_flag = 0;
u16 adjust_buf[4];

void adc_adjust()
{
	u8 x1, y1, x2, y2;
	u8 i;
	menu3 = 1;
	if (first_splash)
	{
		first_splash = 0;
		menu3 = 0;
		for (i = 0; i < 4; i++)
		{
			adc_min[i] = 1547;
			adc_max[i] = 2547;
		}
	}
	XUI_str(56, 4, 64, 12, (u8 *)"摇杆校准", 12, 1, PC, BC);
	XUI_rectangle(5, 24, 51, 70, PC);	 // 摇杆方框
	XUI_rectangle(109, 24, 155, 70, PC); // 摇杆方框
	XUI_line(26, 47, 30, 47, SC);		 // 摇杆十字
	XUI_line(28, 45, 28, 49, SC);		 // 摇杆十字
	XUI_line(130, 47, 134, 47, SC);		 // 摇杆十字
	XUI_line(132, 45, 132, 49, SC);		 // 摇杆十字
	x1 = (hardware_value[0] - 1500) * 19 / 500 + 28;
	y1 = (hardware_value[1] - 1500) * 19 / 500 + 47;
	x2 = (hardware_value[3] - 1500) * 19 / 500 + 132;
	y2 = (hardware_value[2] - 1500) * 19 / 500 + 47;
	XUI_circle(x1, y1, 4, PC); // 摇杆圆圈
	XUI_circle(x2, y2, 4, PC); // 摇杆圆圈
	XUI_string8(56, 24, (u8 *)"CH1:", 1, PC, BC);
	XUI_string8(56, 34, (u8 *)"CH2:", 1, PC, BC);
	XUI_string8(56, 44, (u8 *)"CH3:", 1, PC, BC);
	XUI_string8(56, 54, (u8 *)"CH4:", 1, PC, BC);
	XUI_string8(5, 71, (u8 *)"OK to mid,Long OK to save", 1, PC, BC);
	for (i = 0; i < 4; i++)
		XUI_num8(80, 24 + i * 10, hardware_value[i], 4, 1, PC, BC);

	if (adjust_flag)
	{
		adjust_flag--;
		if (adjust_flag == 0)
		{
			for (i = 0; i < 4; i++)
				adc_mid[i] = adjust_buf[i] / 10;
			message("Mid OK", 20);
		}
		else
			for (i = 0; i < 4; i++)
				adjust_buf[i] += hardware_input[i];
	}
	if (OK_release == 1)
	{
		OK_release = 0;
		adjust_flag = 10;
		for (i = 0; i < 4; i++)
			adjust_buf[i] = hardware_input[i];
	}
	if (OK >= 20 && OK <= 40)
	{
		OK = 100;
		menu3 = 0;
#if USE_animation == 0
		ui_menu_flag = 0;
#else
		dir = 15;
#endif
		for (i = 0; i < 4; i++)
		{
			save16(10 + i, adc_min[i]);
			save16(14 + i, adc_mid[i]);
			save16(18 + i, adc_max[i]);
		}
		message("Adjust save OK", 20);
	}
	if (ESC)
	{
		ESC = 0;
		menu3 = 0;
#if USE_animation == 0
		ui_menu_flag = 0;
#else
		dir = 15;
#endif
	}
}
