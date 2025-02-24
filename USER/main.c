#include "xuan.h"

#include "common.h"
#include "ips.h"
// #include "lcd.h"
#include "XUI.h"
#include "usart3.h"
#include "timer.h"
#include "adc.h"
#include "gpio.h"
#include "usart.h"
#include "bkg.h"
#include "mpu6050.h"

/////////////////////////////////系统参数///////////////////////////////////
u8 ui_1hz = 0, ui_10hz = 0, ui_20hz = 0, ui_100hz = 0;	   // 界面刷新频率标志
u8 tsk_1hz = 0, tsk_10hz = 0, tsk_20hz = 0, tsk_100hz = 0; // 任务处理频率标志

u8 time3 = 0;				// 界面定时器3计数器
u8 time1 = 0;				// 任务定时器1计数器
u32 second = 0;				// 开机总时间秒数
u16 battery = 5000;			// 电池电压-mV
u32 time_10ms = 0;			// 计时器1的10ms计数
u8 time_use1 = 0;			// 计时器1使用标志
u32 time_10ms2 = 0;			// 计时器2的10ms计数
u8 time_use2 = 0;			// 计时器2使用标志
u32 time_10ms3 = 0;			// 计时器3的10ms计数
u8 time_use3 = 0;			// 计时器3使用标志
int16 param[2][16];			// 设置界面通用参数缓存
u32 time_100ms = 0;			// 定时器100ms计数器
u32 hardware_id = 0;		// 硬件ID
u8 ppm_connected = 0;		// PPM连接标志
u8 sbus_connected = 0;		// SBUS连接标志
u8 elrs_connected = 0;		// ELRS高频头连接标志
u8 elrs_back_connected = 0; // ELRS回传连接标志
u8 led_time = 10;			// 屏幕背光计数器
u8 led_time0 = 10;			// 屏幕背光熄屏时间
u8 led_enable = 0;			// 屏幕自动背光使能
u8 admin = 0;				// 注册标志
u8 adc_enable = 0;			// ADC输入使能开关
u8 input_type = 0;			// 主输入信号类型
u8 beep_flag = 0;			// 蜂鸣器声音持续时间计数器
u8 usb_flag = 0;			// USB插入标志
u8 out_mode_temp = 0;		// 信号输入历史记录
u8 beep_enable = 1;			// 蜂鸣器使能
int16 bat_vol = 680;		// 电池电压校准参数

///////////////////////////////界面UI参数///////////////////////////////////
u8 UP, DOWN, LEFT, RIGHT, ESC, OK; // 菜单按键按下标志
u8 ESC_release, OK_release;		   // 菜单按键释放标志
u8 fps = 0;						   // 界面刷新帧率
u8 fps_temp = 0;				   // 界面刷新帧率计数器
u8 first_splash = 0;			   // UI首次刷新标志
u8 ui_flag[4] = {0, 0, 0};		   // 桌面UI标志
u8 ui_current = 0;				   // 桌面当前UI标志
u8 ui_menu_edit = 0;			   // 设置界面标志
u8 ui_menu_flag = 0;			   // 设置界面当前UI标志
u8 ui_select_flag = 0;			   // 设置界面UI选中标志
u8 PC = WHITE8;					   // 前景色，字体颜色
u8 BC = BLACK8;					   // 背景色
u8 RC = GRAY8;					   // 条形Bar，状态栏颜色
u8 SC = BLUE8;					   // 项目选中颜色
u8 FC = RED8;					   // 项目聚焦颜色
u8 theme = 0;					   // 界面主题
u8 save_flag = 0;				   // 参数保存通用标志
u8 save_time = 0;				   // 参数保存延时
u8 bkg_type = 0;				   // UI背景类型
u8 lcd_dir = 0;					   // 屏幕翻转
u8 lcd_mirror = 0;				   // 屏幕镜像
u8 usart2_function = 0;			   // 串口2功能
u16 mavlink_buad = 1152;		   // Mavlink波特率

//////////////////////////////////遥控参数///////////////////////////////////
u8 out_mode = 1; // 输出模式
u8 ch_map[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
int16 ch_min[16] = {100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100};
int16 ch_mid[16] = {
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
};
int16 ch_max[16] = {-100, -100, -100, -100, -100, -100, -100, -100, -100, -100, -100, -100, -100, -100, -100, -100};
u8 ch_rev[16] = {
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
};
u16 CH_input[16] = {1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500}; // 通道输入值

u16 CH_out[16] = {1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500}; // 通道输出值
u8 output_map[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
u8 output_max[16] = {100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100};
u8 output_min[16] = {100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100};

/////////////////////////////////回传参数///////////////////////////////////
u8 tele_flag = 0; // 数传连接计数器

int16 hud_roll = 0;	 // 横滚角
int16 hud_pitch = 0; // 俯仰角
int16 hud_yaw = 0;
int16 hud_alt = 0;
int16 hud_groundspeed = 0;
int16 hud_airspeed = 0;
u8 hud_gps = 0;
int16 hud_bat = 0;
int16 hud_cur = 0;
u8 hud_signal = 0;
int hud_lantitude;
int hud_longitude;

u8 hud_data_type = 0;
u8 gyro_connected = 0;

u8 tx_signal = 0; // 发射信号
u8 rx_signal = 0; // 接收信号
u8 mavlink_flag = 0;
u8 mavlink_connected = 0;

int16 Ax, Ay, Az;
////////////////////////////////MAVLINK参数///////////////////////////////////
u8 mavlink_enable = 1; // MAVLINK输出使能

u8 ips_type = 0; // 0:0.96   1:1.8

/*
PPM输入      ->  PA0捕获     4Mhz捕获

SBUS输入     ->  串口1->RX   1000000bps
SBUS输出     ->  串口3->TX   1000000bps  与CRSF输出共用引脚，但不会同时输出两个信号，所以不冲突

MAVLINK输出  ->  串口2->TX   57600bps
MAVLINK输入  ->  串口2->RX   57600bps

CRSF输出     ->  串口3->TX   4000000bps
CRSF输入     ->  串口3->RX   4000000bps

ADC1         ->  PA1
ADC2         ->  PA4
ADC3         ->  PA5
ADC4         ->  PA6
ADC5         ->  PA7
ADC6         ->  PB0
BAT_ADC      ->  PB1

*/

int main(void)
{
	SystemInit();				   // 系统初始化72M
	delay_init();				   // 延时函数初始化
	delay_ms(100);				   // 上电延时，确保电源稳定
	Gpio_Init();				   // 初始化输出引脚
	BEEP_Init(999, CPU_SPEED - 1); // 蜂鸣器初始化
	LCD_BLK_Clr();									// 打开背光
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(100000);								// 串口1初始化为CRSF
	Adc_Init();										// ADC初始化
	if (read16(0) != aux_version)
		factory(); // 首次运行初始化
	usart2_function = read16(30);

	if (usart2_function == 0) // 串口功能为MAVLINK
	{
#if MAVLINK
		usart2_init(mavlink_buad * 100); // 串口2初始化
		mavlink_init();					 // 初始化Mavlink
		request_mavlink();				 // 请求MAVLINK数据
#endif
	}
	else if (usart2_function == 1) // 串口功能为MPU6050
	{
		MPU6050_Init();
	}

#if USB
	USB_Init();		   // 初始化USB
	USB_Connect(TRUE); // 使能USB
#endif
	//////////////////////////////////////////////////////////////////////

	lcd_dir = read16(27);		// 获取屏幕方向设置
	lcd_mirror = read16(28);	// 获取屏幕镜像设置
	LCD_Init();					// LCD初始化
	LCD_Fill(0, 0, 159, 79, 0); // 清楚屏幕

#if DMA
	XUI_dma_init(); // 屏幕DMA初始化
#endif

	bat_adc = Get_Adc(9); // 获取电池电压值
	/////////////////////////////////////////////////////////////////////////
	out_mode = 1;
	ppm_mode = 1;	   // PPM输入模式
	if (ppm_mode == 1) // PPM输入模式
	{
		ppm_Cap_Init(0xffff, CPU_SPEED - 1); // 以1Mhz的频率计数
		if (out_mode == 3)
			out_mode = 1;
	}
	if (out_mode == 1)
		usart3_init(400000); // CRSF输出
	else if (out_mode == 2)
		sbus_init(100000);	// SBUS输出
	else if (out_mode == 3) // PPM输出
	{
		ppm_enable();
		TIM2_Int_Init(999, CPU_SPEED - 1); // 定时器1ms中断
	}

	TIM3_Int_Init(999, CPU_SPEED * 10 - 1); // 定时器10ms中断
	TIM1_Int_Init(999, CPU_SPEED * 10 - 1); // 定时器1ms中断

	TIM_Cmd(TIM3, (FunctionalState)1); // 打开任务定时器
	TIM_Cmd(TIM1, (FunctionalState)1); // 打开任务定时器

	beep_enable = read16(25); // 检测蜂鸣器使能状态
	// 注册蜂鸣器开机声音，反向注册
	beep_queue_regedit(1400, 10);
	beep_queue_regedit(0, 4);
	beep_queue_regedit(1200, 10);
	beep_queue_regedit(0, 4);
	beep_queue_regedit(1000, 10);

	logo(); // 开机LOGO
	// 开机默认界面
	if (boot_menu == 0)
	{
	}
	else if (boot_menu == 1)
	{
		ui_current = 0;
		ui_flag[ui_current] = 1;
	}
	else if (boot_menu == 2)
	{
		ui_current = 1;
		ui_flag[ui_current] = 3;
	}
	else if (boot_menu == 3)
	{
		ui_current = 1;
		ui_flag[ui_current] = 0;
	}
	else if (boot_menu == 4)
	{
		ui_current = 1;
		ui_flag[ui_current] = 1;
	}
	else if (boot_menu == 5)
	{
		ui_current = 2;
		ui_flag[ui_current] = 0;
	}
	else if (boot_menu == 6)
	{
		ui_menu_edit = 1;
		ui_menu_flag = 0;
	}

	// 主循环
	while (1)
	{
		background();
		ui_change();		   // 改变UI界面
		if (ui_menu_edit == 0) // 当前为桌面UI
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
					//						case 3:clock();break;
					//						case 4:triangle();break;
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
		else /// 当前为设置界面UI
		{
			statusbar();
			switch (ui_menu_flag)
			{
			case 0:
				XUI_setup_menu();
				break;
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
		message_reflash(); // 刷新提示信息
		XUI_reflash(); // 刷新屏幕
		fps_temp++;	   // 屏幕刷新计数器+1

#if USB
		if (USB_Configuration)
		{
			if (usb_flag == 0)
			{
				message("USB已连接", 20);
				out_mode_temp = out_mode;
				out_mode = 4;
			}
			usb_flag = 10;
		}
		else if (usb_flag)
			usb_flag--;
		else if (out_mode == 4)
		{
			out_mode = out_mode_temp;
			message("USB已断开", 20);
		}
#endif
		if (tsk_100hz)
		{
			tsk_100hz = 0;
#if MAVLINK
			if (mavlink_enable && usart2_function == 0)
			{
				update();
			}
#endif
		}
		if (tsk_10hz)
		{
			tsk_10hz = 0;
#if MAVLINK
			if (mavlink_enable)
			{
				if (mavlink_connected && usart2_function == 0)
				{
					send_gps_aat();
					send_position_aat();
				}
			}
#endif
		}
		if (tsk_1hz)
		{
			tsk_1hz = 0;
#if MAVLINK
			if (mavlink_enable)
			{
				if (mavlink_connected)
					send_heartbeat_aat();
			}
#endif
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*---------------------------------UI界面定时器-----------------------------------------------------------------------*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TIM3_IRQHandler(void) // TIM3    20ms
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //??TIM3?????"??
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update); //??TIMx??????
		tsk_100hz = 1;
		ui_100hz = 1;
		beep_queue_operation();
		time3++;
		if (time3 % 5 == 0)
		{
			tsk_20hz = 1;
			ui_20hz = 1;
			menu_check(); // 菜单按键检测
			if (message_time)
				message_time--;
			link_status_check();
			hud_data_prepare();

			if (usart2_function == 1)
			{
				if (gyro_connected == 1)
				{
					Ax = GetData(ACCEL_XOUT_H) * 180 / 32768;
					Ay = GetData(ACCEL_YOUT_H) * 180 / 32768;
					Az = GetData(ACCEL_ZOUT_H) * 180 / 32768;
					if (Az < 0)
					{
						if (Ax >= 0)
							Ax = 180 - Ax;
						else
							Ax = -180 - Ax;
					}
					if (Ax == 0 && Ay == 0 && Az == 0)
					{
						gyro_connected = 0;
						message("Mpu6050已断开", 20);
						beep_negative();
					}
				}
			}
		}
		if (time3 % 10 == 0)
		{
			tsk_10hz = 1;
			ui_10hz = 1;
			time_100ms++;
		}
		if (time3 == 100)
		{
			u8 i;
			time3 = 0;
			fps = fps_temp;
			fps_temp = 0;
			second++;
			ui_1hz = 1;
			tsk_1hz = 1;
			if (led_time && led_enable)
				led_time--;
			else if (led_enable)
				LCD_BLK_Clr();

			if (tele_flag)
				tele_flag--; // 数传连接标志
			if (mavlink_flag)
				mavlink_flag--; // 数传连接标志
#if CRSF_DEBUG
			for (i = 0; i < 15; i++)
			{
				CRSF_rate[i] = CRSF_rate_temp[i];
				CRSF_rate_temp[i] = 0;
			}
#endif

#if MAVLINK_DEBUG
			for (i = 0; i < 30; i++)
			{
				msg_rate[i] = msg_rate_temp[i];
				msg_rate_temp[i] = 0;
			}
#endif
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*---------------------------------主任务定时器-----------------------------------------------------------------------*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TIM1_UP_IRQHandler(void) // 0.5MS查询一次
{
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET) // 0.5中断
	{
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update); // 清除中断标志
		time1++;
		input();		   // 获取通道输入+通道运算+按键输入
		mix();			   // 混控运算
		logic_operation(); // 逻辑开关运算
		output();		   // 输出运算
		CRSF_out();		   // CRSF输出
		/*
		switch (out_mode)
		{
		case 1:
			CRSF_out();
			break;
		case 2:
			SBUS_out();
			break;
		case 3:
			break; // PPM输出
		case 4:
			USB_out();
			break;
		default:
			break;
		}
		*/
		if (time_use1) // 计时器计数
			time_10ms += 3;
		if (time_use2)
			time_10ms2 += 3;
		if (time_use3)
			time_10ms3 += 3;

		if (time1 == 100)
		{
			time1 = 0;
		}
	}
}
