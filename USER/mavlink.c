#include "xuan.h"

#if MAVLINK

#include "open_tel_mavlink.h"
#include "common.h"
#include "XUI.h"
#include "ips.h"
/*

heartbeat
attitude
sys_status
battery_status
gps_raw_int
global_position_int
radio_status
rc_channel
vfr_hud
*/

void send_gps_aat()
{
	GPS_raw.lat = crsf_latitude;
	GPS_raw.lon = crsf_longitude;
	GPS_raw.alt = crsf_altitude * 1000;
	GPS_raw.fix_type = 3;
	GPS_raw.satellites_visible = crsf_satellites;
	GPS_raw.eph = 10;
	GPS_raw.epv = 10;
	mavlink_send_message(MAVLINK_COMM_0, MSG_GPS_RAW, 0); // 发送心跳
}

void send_position_aat()
{
	position.lat = crsf_latitude;
	position.lon = crsf_longitude;
	position.alt = crsf_altitude * 1000;
	position.relative_alt = crsf_altitude * 1000;
	position.hdg = crsf_yaw;
	mavlink_send_message(MAVLINK_COMM_0, MSG_POSITION, 0);
}

void send_heartbeat_aat()
{
	mavlink_send_message(MAVLINK_COMM_0, MSG_HEARTBEAT, 0); // 发送心跳
}

void request_mavlink(void)
{
	tx_request_data_stream.req_stream_id = 0; //
	tx_request_data_stream.req_message_rate = 2;
	tx_request_data_stream.start_stop = 1;
	mavlink_send_message(MAVLINK_COMM_0, MSG_ID_REQUEST_DATA_STREAM, 0);

	tx_request_data_stream.req_stream_id = 10; //
	tx_request_data_stream.req_message_rate = 10;
	tx_request_data_stream.start_stop = 1;
	mavlink_send_message(MAVLINK_COMM_0, MSG_ID_REQUEST_DATA_STREAM, 0);
}

#endif

#include "mpu6050.h"
#include "common.h"
#include "XUI.h"
#include "ips.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void mavlink_data1()
{
	statusbar();

	XUI_str(48, 2, 64, 16, (u8 *)"ATTITUDE", 16, 1, PC, BC);

#if MAVLINK
	XUI_str(0, 20, 100, 12, (u8 *)"Roll:", 12, 1, PC, BC);
	XUI_str(0, 32, 100, 12, (u8 *)"Pitch:", 12, 1, PC, BC);
	XUI_str(0, 44, 100, 12, (u8 *)"Yaw:", 12, 1, PC, BC);

	XUI_str(80, 20, 100, 12, (u8 *)"R_spd:", 12, 1, PC, BC);
	XUI_str(80, 32, 100, 12, (u8 *)"P_spd:", 12, 1, PC, BC);
	XUI_str(80, 44, 100, 12, (u8 *)"Y_spd:", 12, 1, PC, BC);

	XUI_num(40, 20, attitude.roll * 180 / pi, 5, 12, 1, PC, BC);
	XUI_num(40, 32, attitude.pitch * 180 / pi, 5, 12, 1, PC, BC);
	XUI_num(40, 44, attitude.yaw * 180 / pi, 5, 12, 1, PC, BC);

	XUI_num(120, 20, attitude.rollspeed * 1000, 5, 12, 1, PC, BC);
	XUI_num(120, 32, attitude.pitchspeed * 1000, 5, 12, 1, PC, BC);
	XUI_num(120, 44, attitude.yawspeed * 1000, 5, 12, 1, PC, BC);
#else
	XUI_str(62, 34, 100, 12, (u8 *)"已禁用", 12, 1, PC, BC);

#endif
}

void mavlink_data2()
{
	statusbar();
	XUI_str(52, 2, 64, 16, (u8 *)"VFR_HUD", 16, 1, PC, BC);
#if MAVLINK
	XUI_str(0, 20, 100, 12, (u8 *)"AIR_spd:", 12, 1, PC, BC);
	XUI_str(0, 32, 100, 12, (u8 *)"GND_spd:", 12, 1, PC, BC);
	XUI_str(0, 44, 100, 12, (u8 *)"Alt:", 12, 1, PC, BC);

	XUI_str(80, 20, 100, 12, (u8 *)"Climd:", 12, 1, PC, BC);
	XUI_str(80, 32, 100, 12, (u8 *)"Head:", 12, 1, PC, BC);
	XUI_str(80, 44, 100, 12, (u8 *)"Throt:", 12, 1, PC, BC);

	XUI_num(40, 20, rx_vfr_hud.airspeed * 1000, 5, 12, 1, PC, BC);
	XUI_num(40, 32, rx_vfr_hud.groundspeed * 1000, 5, 12, 1, PC, BC);
	XUI_num(40, 44, rx_vfr_hud.alt * 100, 5, 12, 1, PC, BC);

	XUI_num(120, 20, rx_vfr_hud.climb * 1000, 5, 12, 1, PC, BC);
	XUI_num(120, 32, rx_vfr_hud.heading, 5, 12, 1, PC, BC);
	XUI_num(120, 44, rx_vfr_hud.throttle, 5, 12, 1, PC, BC);
#else
	XUI_str(62, 34, 100, 12, (u8 *)"已禁用", 12, 1, PC, BC);

#endif
}

void mavlink_data3()
{
	statusbar();
	XUI_str(52, 2, 64, 16, (u8 *)"SYS_STA", 16, 1, PC, BC);
#if MAVLINK
	XUI_str(0, 20, 100, 12, (u8 *)"Load:", 12, 1, PC, BC);
	XUI_str(0, 32, 100, 12, (u8 *)"Batt:", 12, 1, PC, BC);
	XUI_str(0, 44, 100, 12, (u8 *)"Curr:", 12, 1, PC, BC);

	XUI_str(80, 20, 100, 12, (u8 *)"Drop:", 12, 1, PC, BC);
	XUI_str(80, 32, 100, 12, (u8 *)"Error:", 12, 1, PC, BC);
	XUI_str(80, 44, 100, 12, (u8 *)"Remain:", 12, 1, PC, BC);

	XUI_num(40, 20, rx_sys_status.load, 5, 12, 1, PC, BC);
	XUI_num(40, 32, rx_sys_status.voltage_battery, 5, 12, 1, PC, BC);
	XUI_num(40, 44, rx_sys_status.current_battery, 5, 12, 1, PC, BC);

	XUI_num(120, 20, rx_sys_status.drop_rate_comm, 5, 12, 1, PC, BC);
	XUI_num(120, 32, rx_sys_status.errors_comm, 5, 12, 1, PC, BC);
	XUI_num(120, 44, rx_sys_status.battery_remaining, 5, 12, 1, PC, BC);
#else
	XUI_str(62, 34, 100, 12, (u8 *)"已禁用", 12, 1, PC, BC);

#endif
}

void mavlink_data4()
{
	statusbar();
	XUI_str(60, 2, 64, 16, (u8 *)"HEART", 16, 1, PC, BC);
#if MAVLINK
	XUI_str(0, 20, 100, 12, (u8 *)"Custom_m:", 12, 1, PC, BC);
	XUI_str(0, 32, 100, 12, (u8 *)"Type:", 12, 1, PC, BC);
	XUI_str(0, 44, 100, 12, (u8 *)"Autopilot:", 12, 1, PC, BC);

	XUI_str(80, 20, 100, 12, (u8 *)"Base_mod:", 12, 1, PC, BC);
	XUI_str(80, 32, 100, 12, (u8 *)"Sys_sta:", 12, 1, PC, BC);
	XUI_str(80, 44, 100, 12, (u8 *)"Version:", 12, 1, PC, BC);

	XUI_num(40, 20, heartbeat.custom_mode, 5, 12, 1, PC, BC);
	XUI_num(40, 32, heartbeat.type, 5, 12, 1, PC, BC);
	XUI_num(40, 44, heartbeat.autopilot, 5, 12, 1, PC, BC);

	XUI_num(120, 20, heartbeat.base_mode, 5, 12, 1, PC, BC);
	XUI_num(120, 32, heartbeat.system_status, 5, 12, 1, PC, BC);
	XUI_num(120, 44, heartbeat.mavlink_version, 5, 12, 1, PC, BC);
#else
	XUI_str(62, 34, 100, 12, (u8 *)"已禁用", 12, 1, PC, BC);

#endif
}

/*
#0   HEARTBEAT;
--	#1	 SYS_STATUS				         --
#2	 SYS_TIME						       --
#22  PARAM_VALUE				      --
#24	 GPS_RAW_INT						  --
#27	 RAW_INT						       --
#29  SCALED_PRESSURE			    --
--	#30  ATTITUDE                   --
#33  GLOBAL_POSITION_INT        --
--	#35  RC_CHANNELS_RAW          --
#36  SERVO_OUTPUT_RAW          --
#39  MISSION_ITEM
#40  MISSION_REQUEST
#42  MISSION_CURRENT		      --
#44  MISSION_COUNT
#47  MISSION_ACK
#62  NAV_CONTROLLER_OUTPUT		--
#65  RC_CHANNELS	             --
--	#74  VFR_HUD	                --
#77  COMMAND_ACK
#116 SCALED_IMU2
#125 POWER_SATTUS		          --
#253 STATUSTEXT			          --
#136 MAVLINK_MSG_ID_TERRAIN_REPORT  --
#147  MAVLINK_MSG_ID_BATTERY_STATUS  --
*/

#if MAVLINK_DEBUG
extern u8 msgid[30];
extern u8 msg_rate[30];
extern u8 msg_rate_temp[30];
extern u8 mavlink_count;

#endif

void mavlink_test()
{
#if MAVLINK_DEBUG
	u8 i = 0;
	statusbar();
	XUI_str(56, 2, 72, 16, (u8 *)"MAV_ID", 16, 1, PC, BC);
	XUI_str(0 + 6, 20, 100, 12, (u8 *)"ID", 12, 1, PC, BC);
	XUI_str(40 + 6, 20, 100, 12, (u8 *)"ID", 12, 1, PC, BC);
	XUI_str(80 + 6, 20, 100, 12, (u8 *)"ID", 12, 1, PC, BC);
	XUI_str(120 + 6, 20, 100, 12, (u8 *)"ID", 12, 1, PC, BC);
	XUI_str(20 + 6, 20, 100, 12, (u8 *)"Hz", 12, 1, PC, BC);
	XUI_str(60 + 6, 20, 100, 12, (u8 *)"Hz", 12, 1, PC, BC);
	XUI_str(100 + 6, 20, 100, 12, (u8 *)"Hz", 12, 1, PC, BC);
	XUI_str(140 + 6, 20, 100, 12, (u8 *)"Hz", 12, 1, PC, BC);

	for (i = 0; i < 5; i++)
	{
		XUI_num8(0, 32 + 8 * i, msgid[i], 3, 1, PC, 0);
		XUI_num8(40, 32 + 8 * i, msgid[i + 5], 3, 1, PC, 0);
		XUI_num8(80, 32 + 8 * i, msgid[i + 10], 3, 1, PC, 0);
		XUI_num8(120, 32 + 8 * i, msgid[i + 15], 3, 1, PC, 0);
		XUI_num8(20, 32 + 8 * i, msg_rate[i], 2, 1, msg_rate[i] ? RED8 : PC, 0);
		XUI_num8(60, 32 + 8 * i, msg_rate[i + 5], 2, 1, msg_rate[i + 5] ? RED8 : PC, 0);
		XUI_num8(100, 32 + 8 * i, msg_rate[i + 10], 2, 1, msg_rate[i + 10] ? RED8 : PC, 0);
		XUI_num8(140, 32 + 8 * i, msg_rate[i + 15], 2, 1, msg_rate[i + 15] ? RED8 : PC, 0);
	}
	// XUI_num(20,0,mavlink_count,5,12,0,RED8,0);
#else
	statusbar();
	XUI_str(52, 2, 72, 16, (u8 *)"MAVLINK", 16, 1, PC, BC);
	XUI_str(62, 34, 100, 12, (u8 *)"已禁用", 12, 1, PC, BC);
#endif
}

void hud_data_prepare()
{
	if (hud_data_type == 0) // 自动
	{
		if (elrs_back_connected)
		{
			if (crsf_yaw > 0)
				hud_yaw = crsf_yaw * 180 / 31415;
			else
			{
				hud_yaw = (31415 + crsf_yaw) * 180 / 31415 + 180;
				if (hud_yaw > 359)
					hud_yaw = 359;
			}
			hud_roll = -crsf_roll * 180 / 31415;
			hud_pitch = -crsf_pitch * 180 / 31415;
			hud_alt = crsf_altitude;
			hud_groundspeed = crsf_ground_speed;
			hud_gps = crsf_satellites;
			hud_bat = crsf_bat;
			hud_cur = crsf_cur;
		}
#if MAVLINK
		else if (mavlink_connected)
		{

			hud_yaw = attitude.yaw * 180 / pi;
			hud_roll = -attitude.roll * 180 / pi;
			hud_pitch = attitude.pitch * 180 / pi;
			hud_alt = rx_vfr_hud.alt * 10;
			hud_groundspeed = rx_vfr_hud.groundspeed;
			hud_gps = 32;
			hud_bat = rx_sys_status.voltage_battery;
			hud_cur = rx_sys_status.current_battery;
		}
#endif
		else if (gyro_connected)
		{
			hud_roll = Ax;
			hud_pitch = Ay;
			hud_yaw = 0;
		}
		else
		{
			hud_yaw += (CH_out[3] - 1500) / 50;
			if (hud_yaw < 0)
				hud_yaw += 360;
			else if (hud_yaw > 360)
				hud_yaw -= 360;
			hud_alt += (CH_out[2] - 1500) / 50;
			hud_roll = -(CH_out[0] - 1500) * 180 / 500;
			hud_pitch = -(CH_out[1] - 1500) * 90 / 500;
			hud_groundspeed = (CH_out[0] + CH_out[1] - 2000) / 10;
		}
	}
	else if (hud_data_type == 1) // 来自ELRS数据
	{
		if (crsf_yaw > 0)
			hud_yaw = crsf_yaw * 180 / 31415;
		else
		{
			hud_yaw = (31415 + crsf_yaw) * 180 / 31415 + 180;
			if (hud_yaw > 359)
				hud_yaw = 359;
		}
		hud_roll = -crsf_roll * 180 / 31415;
		hud_pitch = -crsf_pitch * 180 / 31415;
		hud_alt = crsf_altitude;
		hud_groundspeed = crsf_ground_speed;
	}
#if MAVLINK
	else if (hud_data_type == 2) // 来自MAVLINK数据
	{
		hud_yaw = attitude.yaw * 180 / pi;
		hud_roll = attitude.roll * 180 / pi;
		hud_pitch = attitude.pitch * 180 / pi;
		hud_alt = rx_vfr_hud.alt;
		hud_groundspeed = rx_vfr_hud.groundspeed;
	}
#endif
	else if (hud_data_type == 3) // 来自姿态仪数据
	{
		hud_roll = Ax;
		hud_pitch = Ay;
		hud_yaw = 0;
	}
	else if (hud_data_type == 4) // 来自通道数据
	{
		hud_yaw += (CH_out[3] - 1500) / 50;
		if (hud_yaw < 0)
			hud_yaw += 360;
		else if (hud_yaw > 360)
			hud_yaw -= 360;
		hud_alt += (CH_out[2] - 1500) / 50;
		hud_roll = -(CH_out[0] - 1500) * 180 / 500;
		hud_pitch = -(CH_out[1] - 1500) * 90 / 500;
		hud_groundspeed = (CH_out[0] + CH_out[1] - 2000) / 10;
	}
}
