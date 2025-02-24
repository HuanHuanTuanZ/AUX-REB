// #include "open_tel_mavlink.h"
#include "common.h"
#include "crsf.h"
#include "xuan.h"
#include "gpio.h"

volatile u8 CRSF_package[26] = {0xEE, 24, 0x16, 0x1F, 0xA8, 0x09, 0x08, 0x6A, 0x50, 0x03, 0x10, 0x80, 0x00,
								0x04, 0x20, 0x00, 0x01, 0x08, 0x07, 0x38, 0x00, 0x10, 0x80, 0x00, 0x04, 0x00}; // crsf协议包

#define GPS_ID 0x02
#define CF_VARIO_ID 0x07
#define BATTERY_ID 0x08 //
#define BARO_ALT_ID 0x09
#define LINK_ID 0x14 //
#define CHANNELS_ID 0x16
#define LINK_RX_ID 0x1C
#define LINK_TX_ID 0x1D
#define ATTITUDE_ID 0x1E	//
#define FLIGHT_MODE_ID 0x21 //
#define PING_DEVICES_ID 0x28
#define DEVICE_INFO_ID 0x29 //
#define REQUEST_SETTINGS_ID 0x2A
#define COMMAND_ID 0x32
#define RADIO_ID 0x3A //

const u8 crsf_id[15] =
	{
		GPS_ID, CF_VARIO_ID, BATTERY_ID, BARO_ALT_ID, LINK_ID, CHANNELS_ID, LINK_RX_ID,
		LINK_TX_ID, ATTITUDE_ID, FLIGHT_MODE_ID, PING_DEVICES_ID, DEVICE_INFO_ID,
		REQUEST_SETTINGS_ID, COMMAND_ID, RADIO_ID};

/*crsf address*/
#define CRSF_ADDRESS_RADIO_TRANSMITTER 0xEA
#define CRSF_ADDRESS_CRSF_TRANSMITTER 0xEE

/*crsf frame type*/
#define CRSF_FRAMETYPE_LINK_STATISTICS 0x14
#define CRSF_FRAMETYPE_RC_CHANNELS_PACKED 0x16
#define CRSF_FRAMETYPE_RADIO_ID 0x3A
#define CRSF_FRAMETYPE_PARAMETER_WRITE 0x2D

#define CRSF_INTERVAL 2

void CRSF_send(u8 *package, u8 len);			// 发送CRSF协议包
void CRSF_Package(void);						// CRSF协议遥控数据封包
void CRSF_param_package(u8 dataType, u8 param); // 发送CRSF设置参数封包函数
void CRSF_Proess(void);							// CRSF主进程

u8 crsf_tx = 0;		// 发送状态标志位
void CRSF_Package() // CRSF协议遥控数据封包
{
	u8 i = 0;
	u32 temp[16];								// 通道数据缓存
	volatile u8 *crc_start;						// crc校验起始位置
	u32 bits = 0;								// 未知
	u8 bitsavailable = 0;						// 未知
	volatile u8 *buf = CRSF_package;			// 协议包指针
	*buf++ = CRSF_ADDRESS_CRSF_TRANSMITTER;		// 0号元素，包头设置为0xEE
	*buf++ = 24;								// 1号元素设置为24
	crc_start = buf;							// crc校验从第2号元素开始，
	*buf++ = CRSF_FRAMETYPE_RC_CHANNELS_PACKED; // 2号元素设置为0x16
	for (i = 0; i < 16; i++)
		temp[i] = CH_out[i] * 1.6 - 1408; /////////////////////通道数据获取，并进行比例缩放
	for (i = 0; i < 16; i++)			  // 开始编码，
	{
		bits |= temp[i] << bitsavailable;
		bitsavailable += 11;
		while (bitsavailable >= 8)
		{
			*buf++ = bits;
			bits >>= 8;
			bitsavailable -= 8;
		}
	}
	*buf++ = crc8(crc_start, 23); // CRC8校验
}

uint8_t param_buf[8];						   // 向crsf发送设置参数的数据缓存
void CRSF_param_package(u8 dataType, u8 param) // 发送CRSF设置参数封包函数
{
	uint8_t *crc_strat;
	param_buf[0] = CRSF_ADDRESS_CRSF_TRANSMITTER; // MODULE ADDRESS: 0xEE
	param_buf[1] = 0x06;						  // 1(ID) + 4 + 1(CRC)
	param_buf[2] = CRSF_FRAMETYPE_PARAMETER_WRITE;
	param_buf[3] = CRSF_ADDRESS_CRSF_TRANSMITTER;
	param_buf[4] = CRSF_ADDRESS_RADIO_TRANSMITTER;
	param_buf[5] = dataType;
	param_buf[6] = param;
	crc_strat = &param_buf[2];
	param_buf[7] = crc8(crc_strat, 5); // CRC8校验
	CRSF_send(param_buf, 8);		   // 发送CRSF参数设置包
}

void CRSF_send(u8 *package, u8 len) // 发送CRSF协议包
{
	u8 i = 0;
	sport_dir = 1;
	crsf_tx = 1;
	while (USART_GetFlagStatus(USART3, USART_FLAG_TC) != SET)
		; // 等待发送结束
	for (i = 0; i < len; i++)
	{
		USART_SendData(USART3, package[i]); // 向串口3发送数据
		while (USART_GetFlagStatus(USART3, USART_FLAG_TC) != SET)
			; // 等待发送结束
	}
	crsf_tx = 0;
	sport_dir = 0;
}

u8 crsf_param_flag = 0; // crsf改变参数标志
u8 crsf_param_buf = 0;	// 需要改变的crsf参数缓存

u8 elrs_param = 0;
u8 elrs_value = 0;

// ELRS command
#define ELRS_ADDRESS 0xEE
#define ELRS_BIND_COMMAND 0xFF
#define ELRS_WIFI_COMMAND 0xFE
#define ELRS_PKT_RATE_COMMAND 0x01
#define ELRS_TLM_RATIO_COMMAND 0x02
#define ELRS_SWITCH_MODE_COMMAND 0x03
#define ELRS_MODEL_MATCH_COMMAND 0x04
#define ELRS_POWER_COMMAND 0x06
#define ELRS_BLE_JOYSTIC_COMMAND 17
#define TYPE_SETTINGS_WRITE 0x2D
#define ADDR_RADIO 0xEA //  Radio Transmitter
#define port Serial

extern u8 elrs_rate;
extern u8 elrs_pwr;

void CRSF_out() // CRSF主进程
{
	if (crsf_param_flag) // 参数改变标志
	{
		if (crsf_param_flag == 1)
			CRSF_param_package(ELRS_PKT_RATE_COMMAND, crsf_param_buf); // 改变遥控速率
		else if (crsf_param_flag == 2)
			CRSF_param_package(ELRS_TLM_RATIO_COMMAND, crsf_param_buf); // 改变回传比例
		else if (crsf_param_flag == 3)
			CRSF_param_package(ELRS_POWER_COMMAND, crsf_param_buf); // 改变功率
		else if (crsf_param_flag == 4)
			CRSF_param_package(ELRS_MODEL_MATCH_COMMAND, crsf_param_buf); // 模式匹配
		else if (crsf_param_flag == 5)
			CRSF_param_package(ELRS_BIND_COMMAND, 1); // 进入无线更新模式，保留
		else if (crsf_param_flag == 6)
			CRSF_param_package(ELRS_BLE_JOYSTIC_COMMAND, 1); // 进入无线更新模式，保留
		else if (crsf_param_flag == 7)
			CRSF_param_package(ELRS_WIFI_COMMAND, 1); // 进入无线更新模式，保留

		crsf_param_flag = 0;
	}
	else
	{
		CRSF_Package();					   // CRSF协议封包
		CRSF_send((u8 *)CRSF_package, 26); // 通过串口发射CRSF协议包
	}
}

volatile extern u8 CRSF_TLM_buf[30]; // 发送缓冲,最大USART6_MAX_SEND_LEN字节
extern u8 CRSF_TLM_flag;

int16 crsf_pitch = 0; // 俯仰
int16 crsf_roll = 0;  // 横滚
int16 crsf_yaw = 0;	  // 偏航

u16 crsf_bat = 0;	   // 电压
u16 crsf_cur = 0;	   // 电流
u16 crsf_capacity = 0; // 电池容量
u8 crsf_remaining = 0; // 剩余电量

u8 crsf_flight_mode = 0; // 飞行模式

int crsf_latitude = 0;		 // 经度
int crsf_longitude = 0;		 // 纬度
int16 crsf_ground_speed = 0; // 地速
int16 crsf_heading = 0;		 // 朝向
int16 crsf_altitude = 0;	 // 卫星海拔
u8 crsf_satellites = 0;		 // 卫星数

int16 crsf_vertical_speed = 0; // 垂直速度

int16 crsf_baro_altitude = 0; // 气压计海拔

u8 crsf_rx_rssi_perc = 0;
u8 crsf_tx_rf_power = 0;

u8 crsf_tx_rssi_perc = 0;
u8 crsf_rx_rf_power = 0;
u8 crsf_tx_fps = 0;

u8 crsf_tx_rssi = 0;
u8 crsf_rx_rssi = 0;
u8 crsf_tx_lq = 0;
u8 crsf_rx_lq = 0;
u8 elrs_rate = 0;
u8 elrs_pwr = 0;

int crsf_update_interval = 0;
int crsf_offset = 0;

void crsf_clear()
{
	crsf_pitch = 0;			 // 俯仰
	crsf_roll = 0;			 // 横滚
	crsf_yaw = 0;			 // 偏航
	crsf_bat = 0;			 // 电压
	crsf_cur = 0;			 // 电流
	crsf_capacity = 0;		 // 电池容量
	crsf_remaining = 0;		 // 剩余电量
	crsf_flight_mode = 0;	 // 飞行模式
	crsf_latitude = 0;		 // 经度
	crsf_longitude = 0;		 // 纬度
	crsf_ground_speed = 0;	 // 地速
	crsf_heading = 0;		 // 朝向
	crsf_altitude = 0;		 // 卫星海拔
	crsf_satellites = 0;	 // 卫星数
	crsf_vertical_speed = 0; // 垂直速度
	crsf_baro_altitude = 0;	 // 气压计海拔
	crsf_rx_rssi_perc = 0;
	crsf_tx_rf_power = 0;
	crsf_tx_rssi_perc = 0;
	crsf_rx_rf_power = 0;
	crsf_tx_fps = 0;
	crsf_tx_rssi = 0;
	crsf_rx_rssi = 0;
	crsf_tx_lq = 0;
	crsf_rx_lq = 0;
	elrs_rate = 0;
	elrs_pwr = 0;
	tx_signal = 0;
	rx_signal = 0;
}

void crsf_depackage() // CRSF回传数据解码
{
	if (crc8(&CRSF_TLM_buf[2], CRSF_TLM_buf[1] - 1) == CRSF_TLM_buf[CRSF_TLM_buf[1] + 1]) // CRC校验
	{
		u8 id = CRSF_TLM_buf[2];
		// if(id!=LINK_ID)tlm_temp++;
		switch (id)
		{
		case LINK_ID:					   // 连接信息
			tx_signal = CRSF_TLM_buf[5];   // 获取CRSF发射信号
			rx_signal = CRSF_TLM_buf[11];  // 获取CRSF回传信号
			crsf_tx_lq = CRSF_TLM_buf[5];  // 获取CRSF发射信号
			crsf_rx_lq = CRSF_TLM_buf[11]; // 获取CRSF回传信号
			crsf_tx_rssi = CRSF_TLM_buf[3];
			crsf_rx_rssi = CRSF_TLM_buf[10];
			if (CRSF_TLM_buf[9] == 1)
				elrs_pwr = 1;
			else if (CRSF_TLM_buf[9] == 2)
				elrs_pwr = 2;
			else if (CRSF_TLM_buf[9] == 8)
				elrs_pwr = 3;
			else if (CRSF_TLM_buf[9] == 3)
				elrs_pwr = 4;
			else if (CRSF_TLM_buf[9] == 7)
				elrs_pwr = 5;

			if (CRSF_TLM_buf[8] == 12)
				elrs_rate = 2;
			else if (CRSF_TLM_buf[8] == 11)
				elrs_rate = 3;
			else if (CRSF_TLM_buf[8] == 10)
				elrs_rate = 4;
			else if (CRSF_TLM_buf[8] == 9)
				elrs_rate = 5;
			else if (CRSF_TLM_buf[8] == 8)
				elrs_rate = 6;
			else if (CRSF_TLM_buf[8] == 7)
				elrs_rate = 7;
			else if (CRSF_TLM_buf[8] == 5)
				elrs_rate = 8;
			else if (CRSF_TLM_buf[8] == 4)
				elrs_rate = 9;
			else if (CRSF_TLM_buf[8] == 2)
				elrs_rate = 10;
			tele_flag = 2;
			break;
		case ATTITUDE_ID: // 姿态信息
			crsf_pitch = CRSF_TLM_buf[3] << 8 | CRSF_TLM_buf[4];
			crsf_roll = CRSF_TLM_buf[5] << 8 | CRSF_TLM_buf[6];
			crsf_yaw = CRSF_TLM_buf[7] << 8 | CRSF_TLM_buf[8];
			/////////////////////////////////////////////////////////////
			tele_flag = 2;
			//					attitude.pitch=-crsf_pitch*3.1416f/32768;
			//					attitude.roll=crsf_roll*3.1416f/32768;
			//					attitude.yaw=crsf_yaw*3.1416f/32768;
			break;
		case BATTERY_ID: // 电池信息
			crsf_bat = CRSF_TLM_buf[3] << 8 | CRSF_TLM_buf[4];
			crsf_cur = CRSF_TLM_buf[5] << 8 | CRSF_TLM_buf[6];
			crsf_capacity = CRSF_TLM_buf[7] << 16 | CRSF_TLM_buf[8] << 8 | CRSF_TLM_buf[9];
			crsf_remaining = CRSF_TLM_buf[10];
			///////////////////////////////////////////////
			tele_flag = 2;
			//					rx_sys_status.voltage_battery=crsf_bat;
			//					rx_sys_status.current_battery=crsf_cur;
			//					rx_sys_status.battery_remaining=crsf_remaining;
			break;
		case FLIGHT_MODE_ID: // 模式信息
			crsf_flight_mode = CRSF_TLM_buf[3];
			tele_flag = 2;
			break;
		case GPS_ID: // GPS信息
			crsf_latitude = CRSF_TLM_buf[3] << 24 | CRSF_TLM_buf[4] << 16 | CRSF_TLM_buf[5] << 8 | CRSF_TLM_buf[6];
			crsf_longitude = CRSF_TLM_buf[7] << 24 | CRSF_TLM_buf[8] << 16 | CRSF_TLM_buf[9] << 8 | CRSF_TLM_buf[10];
			crsf_ground_speed = CRSF_TLM_buf[11] << 8 | CRSF_TLM_buf[12];
			crsf_heading = CRSF_TLM_buf[13] << 8 | CRSF_TLM_buf[14];
			crsf_altitude = (CRSF_TLM_buf[15] << 8 | CRSF_TLM_buf[16]) - 1000;
			crsf_satellites = CRSF_TLM_buf[17];
			/////////////////////////////////////////////////////////////
			tele_flag = 2;
			//					GPS_raw.satellites_visible=crsf_satellites;
			//					GPS_raw.lon=crsf_longitude;
			//					GPS_raw.lat=crsf_latitude;
			//					radio_status.rssi=crsf_tx_rssi;
			//					rx_vfr_hud.groundspeed=crsf_ground_speed;
			//					GPS_raw.alt=crsf_altitude;
			break;
		case CF_VARIO_ID: // 垂直速度
			crsf_vertical_speed = CRSF_TLM_buf[3] << 8 | CRSF_TLM_buf[4];
			//////////////////////////////////////////////
			tele_flag = 2;
			//					rx_vfr_hud.climb=crsf_vertical_speed;
			break;
		case BARO_ALT_ID: // 气压计高度
			crsf_baro_altitude = CRSF_TLM_buf[3] << 8 | CRSF_TLM_buf[4];
			if (crsf_baro_altitude & 0x8000)
			{
				// Altitude in meters
				crsf_baro_altitude &= ~(0x8000);
				crsf_baro_altitude *= 100; // cm
			}
			else
			{
				// Altitude in decimeters + 10000dm
				crsf_baro_altitude -= 10000;
				crsf_baro_altitude *= 10;
			}
			/////////////////////////////////////////////
			tele_flag = 2;
			//					position.relative_alt=crsf_baro_altitude;
			break;
		case LINK_RX_ID: // 接收信息
			crsf_rx_rssi_perc = CRSF_TLM_buf[4];
			crsf_tx_rf_power = CRSF_TLM_buf[7];
			break;
		case LINK_TX_ID: // 发射信息
			crsf_tx_rssi_perc = CRSF_TLM_buf[4];
			crsf_rx_rf_power = CRSF_TLM_buf[7];
			crsf_tx_fps = CRSF_TLM_buf[8];
			break;
		case RADIO_ID:
			crsf_update_interval = CRSF_TLM_buf[6] << 24 | CRSF_TLM_buf[7] << 16 | CRSF_TLM_buf[8] << 8 | CRSF_TLM_buf[9];
			crsf_offset = CRSF_TLM_buf[10] << 24 | CRSF_TLM_buf[11] << 16 | CRSF_TLM_buf[12] << 8 | CRSF_TLM_buf[13];
			break;
		default:
			break;
		}
	}
}

#define USART3_REC_LEN 30
u16 USART3_RX_STA = 0;
u8 CRSF_start = 0;	  // 接受CRSF回传信息开始标志
u8 CRSF_TLM_flag = 0; // 接收到TLM消息标志

volatile u8 CRSF_TLM_buf[USART3_REC_LEN];
u8 crsf_len = 0;
u8 elrs_tlm = 0;

#if CRSF_DEBUG
volatile u8 CRSF_ID[15] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
volatile u8 CRSF_rate[15] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
volatile u8 CRSF_rate_temp[15] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
#endif

void USART3_IRQHandler(void) // 串口3中断服务程序
{
	u8 Res;
	if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) // 接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		Res = USART_ReceiveData(USART3); //(USART1->DR);	//读取接收到的数据
		if (crsf_tx == 0)				 // 如果空闲
		{
			if (CRSF_start == 3) // 数据接收
			{
				CRSF_TLM_buf[USART3_RX_STA++] = Res;
				if (USART3_RX_STA == (crsf_len + 2)) // 接收完成
				{
					CRSF_start = 0;
					CRSF_TLM_flag = 3;
					crsf_depackage();
				}
			}
			else if (CRSF_start == 2) // 获取CRSF回传ID
			{
				u8 i = 0;
				CRSF_TLM_buf[USART3_RX_STA++] = Res;
				for (i = 0; i < 15; i++)
				{
					if (Res == crsf_id[i]) // 找到ID
					{
#if CRSF_DEBUG
						CRSF_rate_temp[i]++;
#endif
						CRSF_start = 3;
						break;
					}
				}
				if (i >= 15)
					CRSF_start = 0; // 未找到ID
			}
			else if (CRSF_start == 1) // 记录包长度
			{
				CRSF_TLM_buf[USART3_RX_STA++] = Res;
				CRSF_start = 2;
				crsf_len = Res;
			}
			if (Res == 0xEA) // 开始接收
			{
				USART3_RX_STA = 0;
				CRSF_TLM_buf[USART3_RX_STA++] = Res;
				CRSF_start = 1;
			}
		}
		else
			CRSF_start = 0;
	}
}
