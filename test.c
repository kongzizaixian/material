-m                                  
20                                  char* pattern
boat_160x160_ARGB8888.bin           char* pic1_name,
desert_160x160_ARGB8888.bin         char* pic2_name,
dog_160x160_ARGB8888		    char* pic3_name,
elephant_160x160_ARGB8888	    char* pic1_name,
sea_160x160_ARGB8888		    char* pic1_name,
temple_160x160_ARGB8888             char* pic1_name,
4:5:2:3:1:0			    char* ch_list
0:1:2:3:4:5			    char* ovly_map
0:0:120:120_20:160:160:160_40:40:160:160_60:60:160:160_80:80:160:160_100:100:160:160  char* dst_rect
1:1:0:0:0:0			    char* rotation
0				    char* comp_rot
0:0:720:1280			    char* ovly_rect


{
    /* 
     *  "(ch1->ovly2 +ch2->ovly2 ) =>ldi"
     */
    
	/* 叠加摸式OVERLAY_COMP_TYPE_UINT32*/
	OVERLAY_COMP_TYPE_ONLINE,     comp_mode;
	
	/*配置叠加通道资源CH_RES*/
	{		

       /***********************************
         ch_num       : 3;
         ch_en        : 1;
         has_block_rot  : 1;
         has_720p_rot   : 1;
         has_1080p_rot  : 1;
         has_scl1     : 1;
         has_scl3     : 1;
         has_ovly1    : 1;
         has_ovly2    : 1;
         has_ovly3    : 1;
         reserved     : 20;
		*********************************/    compose_ch_res[ADE_COMPOSE_CH_MAX];
			/*通道1*/
			{OVERLAY_PIPE_ADE_CH1,1,0,0,0,0,0,0,1,0,0},
			/*通道2*/
			{OVERLAY_PIPE_ADE_CH2,1,0,0,0,0,0,0,1,0,0},
			/*通道3*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道4*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道5*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道6*/
			{0,0,0,0,0,0,0,0,0,0,0}
	},

	/************************************
        uint32_t    ch_en     : 1;
        uint32_t    has_ldi   : 1;
        uint32_t    has_720p_rot    : 1;
        uint32_t    has_1080p_rot   : 1;
        uint32_t    has_scl2  : 1;
        uint32_t    has_scl3  : 1;
        uint32_t    has_ovly2 : 1;
        uint32_t    has_ovly3 : 1;
        uint32_t    has_wdma2 : 1;
        uint32_t    has_wdma3 : 1;
        uint32_t    reserved  : 22;
	 ************************************/
	/*配置显示通道DISP_CH_RES资源*/
	{0,0,0,0,0,0,0,0,0,0,0},              disp_ch_res;
	
	 /*配置叠加模块资源*/
	/*OVLY1_RES*/
	{0,0,0,0,0,0},				ovly1_res;
	/*OVLY2_RES*/
	{OVERLAY_OVLY_2,1,0,0,0,0,0,0,1,0,0},   ovly2_res;
	/*OVLY3_RES*/
	{0,0,0,0,0,0,0,0,0,0},			ovly3_res;
	
	/*ROT_RES*/
	{0,0,0,0,0,0,0,0,0,0},			rot_res;
	
	/*SCL2_RES*/
	{0,0,0,0,0,0,0,0,0},			scl2_res;
	
	/* ADE_MEM_SHARE_EN */
	0
},
