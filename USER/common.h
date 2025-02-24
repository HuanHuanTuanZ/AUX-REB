#ifndef __COMMON_H
#define __COMMON_H

#include "stm32f10x.h"
#include "delay.h"
#include "sys.h"
#include "xuan.h"

#if USB
#include "usb_hw.h"
extern BYTE USB_Configuration;
void USB_out(void);
#endif

#define u8 unsigned char
#define u16 unsigned short int
#define int16 short int
#define u32 unsigned int

#define pi 3.141592f

#define model_size 225
#define model_addr model_current *model_size + 100

extern int16 x_center;
extern int16 y_center;
extern int16 x_min;
extern int16 y_min;
extern int16 x_max;
extern int16 y_max;

extern u8 hud_type;
extern u8 hud_type2;
extern u8 hud_type3;
extern u8 beep_enable;
extern u8 save_flag;
extern u8 save_time;
extern u8 dir; // 上1 下2 左3 右4
extern int16 pixel;
extern u8 input_type;
extern u16 adc_min[8];
extern u16 adc_mid[8];
extern u16 adc_max[8];
extern u16 hardware_input[8];
extern u8 adc_enable;
extern u8 ESC_release, OK_release;
extern u8 led_time;
extern u8 led_time0;
extern u8 led_enable;
extern int16 bat_adc;
extern u8 message_time;
extern u8 ppm_connected;
extern u8 mavlink_connected;
extern u8 sbus_connected;
extern u8 elrs_connected;
extern u8 elrs_back_connected;
extern u8 remote_ctrl;
extern u8 boot_menu; // 启动默认界面
extern u8 model_current;
extern u8 bkg_type;
extern u8 ui_1hz, ui_10hz, ui_20hz, ui_100hz;
extern u8 tsk_1hz, tsk_10hz, tsk_20hz, tsk_100hz;
extern u16 mavlink_buad;

extern const unsigned char download_icon[];
extern const u8 ui_battery_icon[];
extern u16 hardware_value[10];
extern u32 hardware_id;
extern u8 menu3;
extern char model_name[16];
extern u8 ch_map[16];
extern int16 ch_min[16];
extern int16 ch_mid[16];
extern int16 ch_max[16];
extern u8 ch_rev[16];
extern u16 CH_input[16];
extern u8 admin;
extern u16 CH_out[16];
extern u8 output_map[16];
extern u8 output_max[16];
extern u8 output_min[16];
extern u8 crsf_param_flag; // crsf改变参数标志
extern u8 crsf_param_buf;  // 需要改变的crsf参数缓存
extern u16 mix_value[16];
extern u8 mixer_type[mixer_count];
extern u8 mixer_in1[mixer_count];
extern u8 mixer_in2[mixer_count];
extern int16 mixer_rate[mixer_count];

extern u8 tx_signal;
extern u8 rx_signal;

extern u32 time_10ms;
extern u8 time_use1;
extern u32 time_10ms2;
extern u8 time_use2;
extern u32 time_10ms3;
extern u8 time_use3;

extern u8 CRSF_TLM_flag;
extern u8 TIM2CH1_CAPTURE_STA;	// 输入捕获状态
extern u16 TIM2CH1_CAPTURE_VAL; // 输入捕获值
extern u16 ppm_value[8];		// ppm输入值
//extern u8 ppm_mode;				// 1是输入，0是输出
extern u8 ppm_status;			// ppm输入状态
extern u8 UP, DOWN, LEFT, RIGHT, ESC, OK;
extern u8 first_splash;
extern u8 PC, RC, FC, SC, BC;
extern u8 ui_flag[4];
extern u8 ui_current;
extern u8 ui_menu_edit;
extern u8 ui_menu_flag;
extern u8 ui_select_flag;
extern u8 sbus_buf[25];
extern u16 sbus_value[16];
extern u8 sbus_status;
extern u8 tele_flag;
extern u8 mavlink_flag;
extern u16 battery;
extern u8 fps;
extern int16 x_offset;
extern int16 y_offset;
extern u32 time_100ms;
extern u8 menu_y;
extern u8 py, px;
extern u8 menu_len;
extern u8 param_edit;
extern int16 param[2][16];
extern u8 theme;

/////////////CRSF
extern int16 crsf_pitch;		  // 俯仰
extern int16 crsf_roll;			  // 横滚
extern int16 crsf_yaw;			  // 偏航
extern u16 crsf_bat;			  // 电压
extern u16 crsf_cur;			  // 电流
extern u16 crsf_capacity;		  // 电池容量
extern u8 crsf_remaining;		  // 剩余电量
extern u8 crsf_flight_mode;		  // 飞行模式
extern int crsf_latitude;		  // 经度
extern int crsf_longitude;		  // 纬度
extern int16 crsf_ground_speed;	  // 地速
extern int16 crsf_heading;		  // 朝向
extern int16 crsf_altitude;		  // 卫星海拔
extern u8 crsf_satellites;		  // 卫星数
extern int16 crsf_vertical_speed; // 垂直速度
extern int16 crsf_baro_altitude;  // 气压计海拔
extern u8 crsf_rx_rssi_perc;
extern u8 crsf_tx_rf_power;
extern u8 crsf_tx_rssi_perc;
extern u8 crsf_rx_rf_power;
extern u8 crsf_tx_fps;
extern u8 crsf_tx_rssi;
extern u8 crsf_rx_rssi;
extern u8 crsf_tx_lq;
extern u8 crsf_rx_lq;
extern u8 elrs_rate;
extern u8 elrs_pwr;
extern u8 elrs_tlm;
extern int crsf_update_interval;
extern int crsf_offset;
extern u8 tlm;

extern u8 mavlink_enable;
extern int16 bat_vol;

extern u8 lcd_dir;	  // 屏幕翻转
extern u8 lcd_mirror; // 屏幕镜像

extern int16 hud_roll;	// 横滚角
extern int16 hud_pitch; // 俯仰角
extern int16 hud_yaw;
extern int16 hud_alt;
extern int16 hud_groundspeed;
extern int16 hud_airspeed;
extern u8 hud_gps;
extern int16 hud_bat;
extern int16 hud_cur;
extern u8 hud_signal;
extern int hud_lantitude;
extern int hud_longitude;
extern u8 hud_data_type;
extern u8 gyro_connected;
extern int16 Ax, Ay, Az;

extern u8 usart2_function;

#if CRSF_DEBUG
volatile extern u8 CRSF_ID[15];
volatile extern u8 CRSF_rate[15];
volatile extern u8 CRSF_rate_temp[15];
#endif

#if MAVLINK_DEBUG
extern u8 msgid[30];
extern u8 msg_rate[30];
extern u8 msg_rate_temp[30];
extern u8 mavlink_count;
#endif

extern const unsigned char bkg[25600];

///////系统功能
void save16(u16 addr, u16 v);
void save32(u16 addr, u32 v);
u16 read16(u16 addr);
u32 read32(u16 addr);
void system_read(void);
void factory(void);
u8 len(int t);
void theme_change(u8 seq);
void model_name_save(int16 addr);
void model_name_read(int16 addr);
void model_read(void);
void model_reset(void);
void message(char *p, u8 time);
void message_reflash(void);
void link_status_check(void);
u8 check_ok(char *p);
/////////硬件输入
void input(void);
void menu_check(void);
int abs(int v);
void clock(void);
void output(void);
void mix(void);
void logic_operation(void);

/////////通信信号

//void SBUS_out(void);
void CRSF_out(void); // CRSF主进程
void USB_out(void);
//void ppm_enable(void);	// 开启PPM信号输出
//void ppm_disable(void); // 关闭PPM信号输出
void ppm_Cap_Init(u16 arr, u16 psc);

/////////CRSF
void crsf_clear(void);

//////////MAVLINK
void request_mavlink(void);
void mavlink_init(void);
void index_bar(u8 count, u8 max, u8 menu0, u8 y0, u8 y1);

///////////界面
void ui_change(void);
void XUI_setup_menu(void);
void statusbar(void);

void XUI_desk1(void);
void XUI_desk2(void);
void XUI_desk3(void);
void XUI_desk4(void);
void XUI_desk5(void);

void XUI_data1(void);
void XUI_data2(void);
void XUI_data3(void);
void crsf_test(void);
void mavlink_test(void);

void background(void);

void XUI_tool1(void);
void XUI_tool2(void);
void XUI_tool3(void);

void input_setup(void);
void output_setup(void);
void mix_setup(void);
void logic_setup(void);
void system_setup(void);
void version_info(void);
void hud(void);
void hud2(void);
void hud3(void);
void triangle(void);
void logo(void);
void hud_data_prepare(void);

void model_setup(void);
void adc_adjust(void);
void system_status(void);

void XUI_ground(int16 roll, int16 pitch, u8 c1, u8 c2);
void XUI_ground2(int16 roll, int16 pitch, u8 c1, u8 c2);

extern u8 logic_condition[logic_count];
extern int16 logic_a[logic_count];
extern int16 logic_x_value[logic_count];
extern int16 logic_x[logic_count];
extern int16 logic_last_time[logic_count];
extern u32 logic_current_time[logic_count];
extern u8 logic_flag[logic_count];
extern u8 logic_status[logic_count];
extern u8 logic_status_temp[logic_count];
extern u8 logic_status_last[logic_count];
extern u8 logic_mode[logic_count];

void beep_queue_operation(void);
void beep_queue_regedit(u16 freq, u16 time);

void send_gps_aat(void);
void send_position_aat(void);
void send_heartbeat_aat(void);
void update(void); // MAVLINK解析函数
void request_mavlink(void);
void mavlink_data1(void);
void mavlink_data2(void);
void mavlink_data3(void);
void mavlink_data4(void);

void beep(u16 freq, u16 time);
void beep_positive(void);
void beep_negative(void);

void sbus_init(u32 bound); // 串口2初始化

#endif
