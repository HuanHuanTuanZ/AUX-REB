
//功能裁剪
#define Crystal         8  //晶振频率
#define CPU_SPEED       72 //CPU速度

#define MAVLINK         1   //MAVLINK协议使能
#define MAVLINK_DEBUG   0   //是否使用Mavlink协议回传ID统计功能--保持关闭

#if CPU_SPEED == 72
	#define USB      			1  	//USB游戏摇杆使能，仅当CPU为72Mhz的时候，USB才能正常工作
#else 
	#define USB      			0  	//USB游戏摇杆禁止，其他CPU频率
#endif


#define aux_version     36  //软件版本控制

#define logic_count     4   //逻辑总数 -- 请勿修改
#define mixer_count     4   //混控总数 -- 请勿修改

#define USE_animation   0   //是否使用UI切换动画
#define CRSF_DEBUG      1   //是否使用CRSF协议回传ID统计功能
#define DMA             0   //定义屏幕刷新是否使用DMA，屏幕缓存占用翻一倍，12800->25600
#define BKG             0   //是否使用图片作为背景





