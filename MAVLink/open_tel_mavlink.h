#ifndef __OPEN_TEL_MAVLINK_H//??:????  qq:624668529
#define __OPEN_TEL_MAVLINK_H
//#include "./minimal/minimal/minimal.h"
#include "define.h"
#include "mavlink_avoid_errors.h"
#include "stdint.h"
#include "timer.h"

extern mavlink_command_long_t							 			rx_cmd,tx_cmd;
extern mavlink_request_data_stream_t						tx_request_data_stream;
extern mavlink_set_mode_t    										msg_set_mode,send_set_mode;
extern mavlink_mission_request_t  							mission_request;
extern mavlink_global_position_int_t						position;
extern mavlink_mission_ack_t  									mission_ack;
extern mavlink_mission_item_reached_t						my_mission_item_reached;
extern mavlink_mission_item_t 									guided_target;
extern mavlink_mission_item_t   							 	mission_item;
extern mavlink_attitude_t       						    attitude;
extern mavlink_rc_channels_raw_t   						  rc_input;
extern mavlink_gps_raw_int_t       							GPS_raw;											
extern mavlink_vfr_hud_t												rx_vfr_hud;
extern mavlink_heartbeat_t                    	heartbeat;
extern mavlink_attitude_setpoint_external_t	   	tx_attitude_target;
extern mavlink_battery_status_t                	battery_status;
extern mavlink_power_status_t                  	power_status;
extern mavlink_sys_status_t             	     	rx_sys_status;
extern mavlink_statustext_t											rx_text;
extern mavlink_radio_status_t    								radio_status;    
extern mavlink_rc_channels_t                    rc;
extern mavlink_servo_output_raw_t               servo_raw;
extern mavlink_mission_set_current_t  					mission_current;
extern mavlink_mission_count_t   						 	  mission_count;
extern fifo_t 																	uart_rx_fifo, uart_tx_fifo;
extern fifo_t																		uart_rx_fifo, uart_tx_fifo;
extern mavlink_system_time_t                  	sys_time;
extern mavlink_raw_imu_t                        raw_imu;
extern mavlink_scaled_pressure_t                scaled_pressure;
extern mavlink_mission_current_t                rrent_mission;
extern mavlink_mission_current_t                current_mission;
extern mavlink_nav_controller_output_t 					nav_controller_output;
extern mavlink_scaled_imu2_t										scaled_imu2;
extern mavlink_param_value_t										param_value;
extern mavlink_rc_channels_scaled_t             rc_channel_scaled;
extern mavlink_rc_channels_override_t         	rx_rc_override,tx_rc_override;
extern mavlink_param_request_read_t             param_request_read;
extern mavlink_param_set_t                      param_set_value;

extern u8 mav_rc_activite;
extern u8 flag_guided_target_reached;
//extern u32 time_100ms_count,last_mav_update_rc_time_count;
extern u8 update_10hz;
//extern u8 mission_current_number;

extern u8 mission_flag;
extern u8 is_enable_connect;
extern u8 tele_flag;
extern u8 mission_receive_waypoint_flag;            
void mavlink_send_message(mavlink_channel_t chan, enum ap_message id, uint16_t packet_drops);
void update(void);
void handleMessage(mavlink_message_t* msg);

#endif /*__OPENTEL_MAVLINK_H*/
