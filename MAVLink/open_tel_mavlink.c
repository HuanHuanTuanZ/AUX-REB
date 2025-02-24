#include "xuan.h"

#if MAVLINK

#include "open_tel_mavlink.h" //  qq:624668529
#include "mavlink_usart_fifo.h"
#include "define.h"
#include "stdint.h"
#include "gpio.h"
#include "delay.h"
#define Pi 3.1415926

typedef uint8_t bool;

int8_t control_mode = STABILIZE;

mavlink_channel_t chan;
mavlink_heartbeat_t heartbeat;
mavlink_attitude_t attitude;
// mavlink_mission_ack_t  								 		mission_ack;
// mavlink_mission_item_t   							 		mission_item;
// mavlink_mission_item_t 								 		guided_target;
// mavlink_mission_count_t   						 		mission_count;
// mavlink_mission_request_t  						 		mission_request;
// mavlink_optical_flow_t   						   		opt_flow;
// mavlink_rc_channels_raw_t   					 		rc_input;
mavlink_global_position_int_t position;
// mavlink_set_mode_t         						 		msg_set_mode;
// mavlink_mission_item_reached_t				 		my_mission_item_reached;
mavlink_gps_raw_int_t GPS_raw;
// mavlink_set_mode_t										 		send_set_mode;
// mavlink_command_long_t								 		rx_cmd,tx_cmd;
mavlink_request_data_stream_t tx_request_data_stream;
mavlink_vfr_hud_t rx_vfr_hud;
// mavlink_rc_channels_override_t         		rx_rc_override,tx_rc_override;
// mavlink_attitude_setpoint_external_t	 		tx_attitude_target;
// mavlink_battery_status_t               		battery_status;
// mavlink_power_status_t                 		power_status;
// mavlink_statustext_t									 		rx_text,tx_text;
mavlink_sys_status_t rx_sys_status;
// mavlink_radio_status_t        				 		radio_status;
// mavlink_rc_channels_t                  		rc;
// mavlink_servo_output_raw_t             		servo_raw;
// mavlink_mission_current_t  				 		    mission_current_no;
// mavlink_mission_request_list_t            mission_request_list;
// mavlink_mission_request_t                 mission_request_flag;

// mavlink_system_time_t                     sys_time;
// mavlink_raw_imu_t                         raw_imu;
// mavlink_scaled_pressure_t                 scaled_pressure;
// mavlink_mission_current_t                 current_mission;
// mavlink_nav_controller_output_t 					nav_controller_output;
// mavlink_scaled_imu2_t											scaled_imu2;
// mavlink_param_value_t											param_value;
////End Add By BigW
//// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-
// this costs us 51 bytes, but means that low priority
// messages don't block the CPU

// static mavlink_statustext_t pending_status;

// true when we have received at least 1 MAVLink packet
// static bool mavlink_active;
// check if a message will fit in the payload space available
#define CHECK_PAYLOAD_SIZE(id)                     \
    if (payload_space < MAVLINK_MSG_ID_##id##_LEN) \
    return false
void handleMessage(mavlink_message_t *msg);
/*
 *  !!NOTE!!
 *
 *  the use of NOINLINE separate functions for each message type avoids
 *  a compiler bug in gcc that would cause it to use far more stack
 *  space than is needed. Without the NOINLINE we use the sum of the
 *  stack needed for each message type. Please be careful to follow the
 *  pattern below when adding any new messages
 */
static NOINLINE void send_heartbeat(mavlink_channel_t chan)
{
    uint8_t base_mode = MAV_MODE_FLAG_CUSTOM_MODE_ENABLED;
    uint8_t system_status = MAV_STATE_ACTIVE;
    uint32_t custom_mode = control_mode;
    // work out the base_mode. This value is not very useful
    // for APM, but we calculate it as best we can so a generic
    // MAVLink enabled ground station can work out something about
    // what the MAV is up to. The actual bit values are highly
    // ambiguous for most of the APM flight modes. In practice, you
    // only get useful information from the custom_mode, which maps to
    // the APM flight mode and has a well defined meaning in the
    // ArduPlane documentation
    base_mode = MAV_MODE_FLAG_STABILIZE_ENABLED;
    switch (control_mode)
    {
    case AUTO:
    case RTL:
    case LOITER:
    case GUIDED:
    case CIRCLE:
        base_mode |= MAV_MODE_FLAG_GUIDED_ENABLED;
        // note that MAV_MODE_FLAG_AUTO_ENABLED does not match what
        // APM does in any mode, as that is defined as "system finds its own goal
        // positions", which APM does not currently do
        break;
    }
    // all modes except INITIALISING have some form of manual
    // override if stick mixing is enabled
    base_mode |= MAV_MODE_FLAG_MANUAL_INPUT_ENABLED;
#if HIL_MODE != HIL_MODE_DISABLED
    base_mode |= MAV_MODE_FLAG_HIL_ENABLED;
#endif
    // we are armed if we are not initialising
    if (0)
    { // motors.armed()) {
        base_mode |= MAV_MODE_FLAG_SAFETY_ARMED;
    }
    // indicate we have set a custom mode
    base_mode |= MAV_MODE_FLAG_CUSTOM_MODE_ENABLED;
    mavlink_msg_heartbeat_send(
        chan,
        MAV_TYPE_QUADROTOR,
        MAV_AUTOPILOT_ARDUPILOTMEGA,
        base_mode,
        custom_mode,
        system_status);
}

static void NOINLINE send_requst(mavlink_channel_t chan)
{
    // Vector3f omega_I = ahrs.get_gyro_drift();

    mavlink_msg_request_data_stream_send(
        chan,
        mavlink_system.sysid,  // requst_sys_id,//omega_I.x,
        mavlink_system.compid, // requst_commond_id,
        tx_request_data_stream.req_stream_id,
        tx_request_data_stream.req_message_rate,
        tx_request_data_stream.start_stop); // ahrs.get_error_yaw());
}

// static void NOINLINE send_mission_item_home(mavlink_channel_t chan)
//{
//	mavlink_msg_mission_item_send(
//		chan,
//		mavlink_system.sysid,
//		mavlink_system.compid,
//		0,//航点序号
//		MAV_FRAME_GLOBAL,//坐标系，地面站发送时只有
//		16,//MAV_CMD_NAV_WAYPOINT
//		0,//不设为当前航点
//		0,//自动开始下个航点
//		0.0,//驻留时间，秒
//	  0.0,//航点半径
//		0.0,//顺时针
//		0.0,//
//		guided_target.x,//	经度
//		guided_target.y,//纬度
//		guided_target.z	//高度
//		);
// }

// static void NOINLINE send_mission_item_waypoint(mavlink_channel_t chan)
//{
//	mavlink_msg_mission_item_send(
//		chan,
//		mavlink_system.sysid,
//		mavlink_system.compid,
//		guided_target.seq,//航点序号
//		MAV_FRAME_GLOBAL_RELATIVE_ALT,
//	//坐标系，地面站发送时只有home点是0(MAV_FRAME_GLOBAL)
//	//其余航点均为3(MAV_FRAME_GLOBAL_RELATIVE_ALT)
//		guided_target.command,//MAV_CMD_NAV_WAYPOINT
//		guided_target.current,//设为当前航点 current
//		guided_target.autocontinue,//不自动开始下个航点
//		0,//驻留时间，秒
//	  0,//航点半径
//		0,//顺时针
//		0,//指向设定
//	//地面站发送时普通航点的这四个参数均为0
//		guided_target.x,//	纬度
//		guided_target.y,//  经度
//		guided_target.z	//高度
//		);
// }

// static void NOINLINE send_mission_count(mavlink_channel_t chan)
//{
//	mavlink_msg_mission_count_send(
//			chan,
//			mavlink_system.sysid,
//			mavlink_system.compid,
//			mission_count.count
//	);
// }

// static void NOINLINE send_mission_ack(mavlink_channel_t chan)
//{
//	mavlink_msg_mission_ack_send(
//		chan,
//		mavlink_system.sysid,
//		mavlink_system.compid,
//		0
//	);
// }

// static void NOINLINE set_mode(mavlink_channel_t chan)
//{
//	mavlink_msg_set_mode_send(
//	chan,
//	mavlink_system.sysid,
//	send_set_mode.base_mode,
//	send_set_mode.custom_mode
//	);
// }

// static void NOINLINE send_cmd(mavlink_channel_t chan)
//{
//	mavlink_msg_command_long_send(
//	chan,
//	mavlink_system.sysid,
//	mavlink_system.compid,
//	tx_cmd.command,
//	tx_cmd.confirmation,
//	tx_cmd.param1,
//	tx_cmd.param2,
//	tx_cmd.param3,
//	tx_cmd.param4,
//	tx_cmd.param5,
//	tx_cmd.param6,
//	tx_cmd.param7
//	);
// }

static void NOINLINE send_gps(mavlink_channel_t chan)
{
    mavlink_msg_gps_raw_int_send(
        chan,                        // chan
        0,                           // time_usec
        GPS_raw.fix_type,            // fix_type
        GPS_raw.lat,                 // lat
        GPS_raw.lon,                 // lon
        GPS_raw.alt,                 // alt
        GPS_raw.eph,                 // eph
        GPS_raw.epv,                 // epv
        UINT16_MAX,                  // vel
        UINT16_MAX,                  // cog
        GPS_raw.satellites_visible); // satellites_visible
}

static void NOINLINE send_position(mavlink_channel_t chan)
{
    mavlink_msg_global_position_int_send(
        chan,                  // chan
        0,                     ///< Timestamp (milliseconds since system boot)
        position.lat,          ///< Latitude, expressed as * 1E7
        position.lon,          ///< Longitude, expressed as * 1E7
        position.alt,          ///< Altitude in meters, expressed as * 1000 (millimeters), above MSL
        position.relative_alt, ///< Altitude above ground in meters, expressed as * 1000 (millimeters)
        0,                     ///< Ground X Speed (Latitude), expressed as m/s * 100
        0,                     ///< Ground Y Speed (Longitude), expressed as m/s * 100
        0,                     ///< Ground Z Speed (Altitude), expressed as m/s * 100
        position.hdg);         ///< Compass heading in degrees * 100, 0.0..359.99 degrees. If unknown, set to: UINT16_MAX
}

// static void NOINLINE send_mission_request_list(mavlink_channel_t chan)////////////////杩斿洖浠诲姟鍒楄〃
//{
//	mavlink_msg_mission_request_list_send(chan,mission_request_list.target_component,mission_request_list.target_system);
// }

// static void NOINLINE send_mission_request(mavlink_channel_t chan)////////////////杩斿洖浠诲姟鍒楄〃
//{
//	mavlink_msg_mission_request_send(chan,mission_request.target_system,mission_request.target_component,mission_request.seq);
// }

// static void NOINLINE send_mission_current(mavlink_channel_t chan)//////////////////////////鎵ц鎸囧畾搴忓彿鐨勪换鍔?
//{
//	mavlink_msg_mission_current_send(chan,mission_current_no.seq);
// }

// are we still delaying telemetry to try to avoid Xbee bricking?
static bool telemetry_delayed(mavlink_channel_t chan)
{
    return false;
}
// try to send a message, return false if it won't fit in the serial tx buffer
static bool mavlink_try_send_message(mavlink_channel_t chan, enum ap_message id, uint16_t packet_drops)
{
    int16_t payload_space = serial_free();
    if (telemetry_delayed(chan))
    {
        return false;
    }
    switch (id)
    {
    case MSG_HEARTBEAT:
        CHECK_PAYLOAD_SIZE(HEARTBEAT);
        send_heartbeat(chan);
        break;

    case MSG_GPS_RAW:
        CHECK_PAYLOAD_SIZE(GPS_RAW_INT);
        send_gps(chan);
        break;

    case MSG_POSITION:
        CHECK_PAYLOAD_SIZE(GLOBAL_POSITION_INT);
        send_position(chan);
        break;

    case MSG_ID_REQUEST_DATA_STREAM:
        CHECK_PAYLOAD_SIZE(REQUEST_DATA_STREAM);
        send_requst(chan);
        break;

        //			case MSG_ID_MISSION_CURRENT:
        //				CHECK_PAYLOAD_SIZE(MISSION_CURRENT);/////////////////////////////////////////////////////////////////
        //				send_mission_current(chan);
        //				break;
        //			case MSG_ID_MISSION_ITEM_HOME:
        //				CHECK_PAYLOAD_SIZE(MISSION_ITEM);
        //				send_mission_item_home(chan);
        //				break;
        //			case MSG_ID_MISSION_ITEM_WAYPOINT:
        //				CHECK_PAYLOAD_SIZE(MISSION_ITEM);
        //				send_mission_item_waypoint(chan);
        //				break;
        //			case MSG_ID_MISSION_COUNT:
        //				CHECK_PAYLOAD_SIZE(MISSION_COUNT);
        //				send_mission_count(chan);
        //				break;
        //			case MSG_ID_MISSION_REQUEST_LIST:
        //				CHECK_PAYLOAD_SIZE(MISSION_REQUEST_LIST);///////////////////////////////////////////鑾峰彇浠诲姟娓呭崟
        //				send_mission_request_list(chan);
        //				break;
        //			case MSG_ID_MISSION_REQUEST:
        //				CHECK_PAYLOAD_SIZE(MISSION_REQUEST);
        //				send_mission_request(chan);
        //				break;
        //			case MSG_ID_MISSION_ACK:
        //				CHECK_PAYLOAD_SIZE(MISSION_ACK);
        //				send_mission_ack(chan);
        //				break;
        //			case MSG_ID_SET_MODE:
        //				CHECK_PAYLOAD_SIZE(SET_MODE);
        //				set_mode(chan);
        //				break;
        //			case MSG_ID_COMMAND_LONG:
        //				CHECK_PAYLOAD_SIZE(COMMAND_LONG);
        //				send_cmd(chan);
        //				break;
    default:
        break;
    }
    return true;
}

#define MAX_DEFERRED_MESSAGES MSG_RETRY_DEFERRED
static struct mavlink_queue
{
    enum ap_message deferred_messages[MAX_DEFERRED_MESSAGES];
    uint8_t next_deferred_message;
    uint8_t num_deferred_messages;
} mavlink_queue[2];

// send a message using mavlink
void mavlink_send_message(mavlink_channel_t chan, enum ap_message id, uint16_t packet_drops)
{
    uint8_t i, nextid;
    struct mavlink_queue *q = &mavlink_queue[(uint8_t)chan];
    // see if we can send the deferred messages, if any
    while (q->num_deferred_messages != 0)
    {
        if (!mavlink_try_send_message(chan,
                                      q->deferred_messages[q->next_deferred_message],
                                      packet_drops))
        {
            break;
        }
        q->next_deferred_message++;
        if (q->next_deferred_message == MAX_DEFERRED_MESSAGES)
        {
            q->next_deferred_message = 0;
        }
        q->num_deferred_messages--;
    }
    if (id == MSG_RETRY_DEFERRED)
    {
        return;
    }
    // this message id might already be deferred
    for (i = 0, nextid = q->next_deferred_message; i < q->num_deferred_messages; i++)
    {
        if (q->deferred_messages[nextid] == id)
        {
            // its already deferred, discard
            return;
        }
        nextid++;
        if (nextid == MAX_DEFERRED_MESSAGES)
        {
            nextid = 0;
        }
    }
    if (q->num_deferred_messages != 0 ||
        !mavlink_try_send_message(chan, id, packet_drops))
    {
        // can't send it now, so defer it
        if (q->num_deferred_messages == MAX_DEFERRED_MESSAGES)
        {
            // the defer buffer is full, discard
            return;
        }
        nextid = q->next_deferred_message + q->num_deferred_messages;
        if (nextid >= MAX_DEFERRED_MESSAGES)
        {
            nextid -= MAX_DEFERRED_MESSAGES;
        }
        q->deferred_messages[nextid] = id;
        q->num_deferred_messages++;
    }
}

// void mavlink_send_text(mavlink_channel_t chan, enum gcs_severity severity, char *str)
//{
//	if (telemetry_delayed(chan))
//	{
//		return;
//	}
//	if (severity == SEVERITY_LOW)
//	{
//		// send via the deferred queuing system
//		pending_status.severity = (uint8_t)severity;
//		mav_array_memcpy((char *)pending_status.text, str, sizeof(pending_status.text));
//		mavlink_send_message(chan, MSG_STATUSTEXT, 0);
//	}
//	else
//	{
//		// send immediately
//		mavlink_msg_statustext_send(
//				chan,
//				severity,
//				str);
//	}
// }

/*
#0   HEARTBEAT;
#1	 SYS_STATUS				         --
#2	 SYS_TIME						       --
#22  PARAM_VALUE				      --
#24	 GPS_RAW_INT						  --
#27	 RAW_INT						       --
#29  SCALED_PRESSURE			    --
#30  ATTITUDE                   --
#33  GLOBAL_POSITION_INT        --
#35  RC_CHANNELS_RAW          --
#36  SERVO_OUTPUT_RAW          --
#39  MISSION_ITEM
#40  MISSION_REQUEST
#42  MISSION_CURRENT		      --
#44  MISSION_COUNT
#47  MISSION_ACK
#62  NAV_CONTROLLER_OUTPUT		--
#65  RC_CHANNELS	             --
#74  VFR_HUD	                --
#77  COMMAND_ACK
#116 SCALED_IMU2
#125 POWER_SATTUS		          --
#253 STATUSTEXT			          --
#136 MAVLINK_MSG_ID_TERRAIN_REPORT  --
#147  MAVLINK_MSG_ID_BATTERY_STATUS  --
*/

#include "common.h"

// 解包函数,可按需要仿照如下格式添加case和对应的decode函数

#if MAVLINK_DEBUG
u8 msgid[30] = {0};
u8 msg_rate[30];
u8 msg_rate_temp[30];
u8 mavlink_count = 0;
#endif

void handleMessage(mavlink_message_t *msg) // command for telemetry
{

#if MAVLINK_DEBUG
    u8 i;
    for (i = 1; i < 30; i++)
    {
        if (msg->msgid == msgid[i])
            break;
        else if (msgid[i] == 0)
        {
            msgid[i] = msg->msgid;
            break;
        }
    }
    for (i = 0; i < 30; i++)
    {
        if (msg->msgid == 0 && i)
            break;
        if (msg->msgid == msgid[i])
            msg_rate_temp[i]++;
    }
#endif

    switch (msg->msgid)
    {

    case MAVLINK_MSG_ID_HEARTBEAT:
    {                                                  // 0
        mavlink_msg_heartbeat_decode(msg, &heartbeat); // 解析心跳包
        break;
    }
    case MAVLINK_MSG_ID_ATTITUDE:
    { //
        mavlink_msg_attitude_decode(msg, &attitude);
        break;
    }
    case MAVLINK_MSG_ID_SYS_STATUS:
    { // 绯荤粺鐘舵�?							#1 xxxxxxxxxxxxxx
        mavlink_msg_sys_status_decode(msg, &rx_sys_status);
        break;
    }
        //				case MAVLINK_MSG_ID_GPS_RAW_INT:{              //RAW鐨勬牸寮忕殑GPS鏁版嵁		#24xxxxxxxxxxxxxxxx
        //					mavlink_msg_gps_raw_int_decode(msg,&GPS_raw);
        //					break;
        //				}
        //				case MAVLINK_MSG_ID_RC_CHANNELS_RAW: {
        //					  mavlink_msg_rc_channels_raw_decode(msg, &rc_input);
        //					  break;
        //				}
        //				case MAVLINK_MSG_ID_SYS_STATUS:{
        //						mavlink_msg_sys_status_decode(msg,&rx_sys_status);
        //						break;
        //				}
        //				case MAVLINK_MSG_ID_MISSION_ACK: {
        //						mavlink_msg_mission_ack_decode(msg,&mission_ack);
        //						break;
        //				}
        //				case MAVLINK_MSG_ID_MISSION_REQUEST:{
        //						mavlink_msg_mission_request_decode(msg,&mission_request);
        //						mission_received=1;
        //					break;
        //				}
    case MAVLINK_MSG_ID_VFR_HUD:
    {
        mavlink_msg_vfr_hud_decode(msg, &rx_vfr_hud);
        break;
    }
        //				default:
        //						break;
    }
}

void update(void) // MAVLINK解析函数
{
    mavlink_message_t msg;
    mavlink_status_t status;
    status.packet_rx_drop_count = 0;
    while (serial_available())
    {
        uint8_t c = serial_read_ch();
        if (mavlink_parse_char(chan, c, &msg, &status))
        {
            handleMessage(&msg);
#if MAVLINK_DEBUG
            mavlink_count++;
#endif
            mavlink_flag = 2;
        }
    }
}

#endif
