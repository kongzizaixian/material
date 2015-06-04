#include "v8r1_commandfile_test.h"
#include <linux/ion.h>
//#include <linux/ion_iommu_heap.h>
//#include <linux/hisi_ion.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

extern int fb_test_open_fb(void);
extern int fb_test_open_ade(void);
extern int fb_test_close_fb(void);
extern int fb_test_close_ade(void);
extern int get_pic_size_by_filename(char *pic_filename , uint32_t* width, uint32_t* height);

//int   g_ov_blending_mode = (0xff << 16) | HWC_BLENDING_COVERAGE;
int   g_ov_blending_mode = (0xff << 16) | HWC_BLENDING_PREMULT;
int   g_ov_fb_fd;
int   g_ov_ade_fd;
int* ion_vir_addr = NULL;
int   g_ion_fd = 0;
struct ion_handle     *g_ion_hnd;
extern struct fb_var_screeninfo g_fb_var_screen;
extern struct fb_fix_screeninfo g_fb_fix_screen;
extern  struct OVERLAY_IMAGE_ST  g_overlay_dst;

int   g_ion_offline_vaddr = 0;
int   g_ion_offline_paddr = 0;

int   g_ion_wifi_vaddr = 0;
int   g_ion_wifi_paddr = 0;

int   g_blk_rot_dst_format = 0;

//char  pic_data[1024*1024];


#define STR_MAX_LEN  (100)

#ifndef ALIGN_UP
#define ALIGN_UP(x, align) (((x) + ((align)-1)) & ~((align)-1))
#endif
#ifndef ALIGN_DOWN
#define ALIGN_DOWN(x, align)  ((x) & ~((align)-1))
#endif

struct overlay_resource_info *pg_ov_res = NULL;
int sec_ovly_count = 0;
int ovly2_ch_count = 0;

struct overlay_resource_info pattern[] = {
//	{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},
/*pattern 5<-------->0*/
{
    
    /* 
     *  "(ch1->ovly2 +ch2->ovly2 ) =>ldi"
     */
    
	/* 叠加摸式OVERLAY_COMP_TYPE_UINT32*/
	OVERLAY_COMP_TYPE_ONLINE,
	
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
		*********************************/
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
	{0,0,0,0,0,0,0,0,0,0,0},
	
	 /*配置叠加模块资源*/
	/*OVLY1_RES*/
	{0,0,0,0,0,0},
	/*OVLY2_RES*/
	{OVERLAY_OVLY_2,1,0,0,0,0,0,0,1,0,0},
	/*OVLY3_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*ROT_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*SCL2_RES*/
	{0,0,0,0,0,0,0,0,0},
	
	/* ADE_MEM_SHARE_EN */
	0
},


/*pattern 6<-------->1*/
{
    /*       
     "(ch1->scl1->ovly3 + ch3->ovly2) =>ROT=>LDI"     
     */
     
	/* 叠加摸式OVERLAY_COMP_TYPE_UINT32*/
	OVERLAY_COMP_TYPE_ONLINE,
	
	/*配置叠加通道资源CH_RES*/
	{		
			/*通道1*/
			{OVERLAY_PIPE_ADE_CH1,1,0,0,0,1,0,0,0,1,0},
			//{OVERLAY_PIPE_ADE_CH1,1,0,0,0,1,0,0,1,0,0},
			/*通道2*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道3*/
			{OVERLAY_PIPE_ADE_CH3,1,0,0,0,0,0,0,1,0,0},			
			/*通道4*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道5*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道6*/
			{0,0,0,0,0,0,0,0,0,0,0}
	},

	/*配置显示通道DISP_CH_RES资源*/
	{0,0,0,0,0,0,0,0,0,0,0},
	
	 /*配置叠加模块资源*/
	/*OVLY1_RES*/
	{0,0,0,0,0,0},
	/*OVLY2_RES*/
	{OVERLAY_OVLY_2,1,0,0,0,1,0,0,0,0,0}, 
	//{OVERLAY_OVLY_2,1,0,0,0,0,0,0,1,0,0},   //ldi
	/*OVLY3_RES*/
	{OVERLAY_OVLY_3,1,0,0,0,0,0,1,0,0},
	//{0,0,0,0,0,0,0,0,0,0},
	
	/*ROT_RES*/
	{1,0,0,0,0,0,0,1,0,0},
	//{0,0,0,0,0,0,0,0,0,0},
	
	/*SCL2_RES*/
	{0,0,0,0,0,0,0,0,0},
	
	/* ADE_MEM_SHARE_EN */
	0
},
/*pattern 7<-------->2*/
{
    /* 
     "(ch1->ovly3->rot->ovly2 +ch4->ovly2) => LDI"          
     */
     
	/* 叠加摸式OVERLAY_COMP_TYPE_UINT32*/
	OVERLAY_COMP_TYPE_ONLINE,
	
	/*配置叠加通道资源CH_RES*/
	{		
			/*通道1*/
			{OVERLAY_PIPE_ADE_CH1,1,0,0,0,0,0,0,0,1,0},
			/*通道2*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道3*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道4*/
			{OVERLAY_PIPE_ADE_CH4,1,0,0,0,0,0,0,1,0,0},
			/*通道5*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道6*/
			{0,0,0,0,0,0,0,0,0,0,0}
	},

	/*配置显示通道DISP_CH_RES资源*/
	{0,0,0,0,0,0,0,0,0,0,0},
	
	 /*配置叠加模块资源*/
	/*OVLY1_RES*/
	{0,0,0,0,0,0},
	/*OVLY2_RES*/
	{OVERLAY_OVLY_2,1,0,0,0,0,0,0,1,0,0},
	/*OVLY3_RES*/
	//{OVERLAY_OVLY_3,1,0,1,0,0,0,0,0,0}, //rot
	{OVERLAY_OVLY_3,1,0,0,0,0,0,1,0,0},   //ovly2
	
	/*ROT_RES*/
	//{1,0,0,0,0,0,1,0,0,0},
	{0,0,0,0,0,0,0,0,0,0},
	
	/*SCL2_RES*/
	{0,0,0,0,0,0,0,0,0},
	
	/* ADE_MEM_SHARE_EN */
	0
},

/*pattern 8<-------->3*/
{
    /*      
     "(ch1->scl1->ovly2 + ch5->ovly2) =>ROT=>SCL2=>LDI"           
     */
	/* 叠加摸式OVERLAY_COMP_TYPE_UINT32*/
	OVERLAY_COMP_TYPE_ONLINE,
	
	/*配置叠加通道资源CH_RES*/
	{		
			/*通道1*/
			{OVERLAY_PIPE_ADE_CH1,1,0,0,0,1,0,0,1,0,0},
			/*通道2*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道3*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道4*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道5*/
			{OVERLAY_PIPE_ADE_CH5,1,0,0,0,0,0,0,1,0,0},
			/*通道6*/
			{0,0,0,0,0,0,0,0,0,0,0}
	},

	/*配置显示通道DISP_CH_RES资源*/
	{0,0,0,0,0,0,0,0,0,0,0},
	
	 /*配置叠加模块资源*/
	/*OVLY1_RES*/
	{0,0,0,0,0,0},
	/*OVLY2_RES*/
	//{OVERLAY_OVLY_2,1,0,0,0,1,0,1,0,0,0},
	{OVERLAY_OVLY_2,1,0,0,0,0,0,1,0,0,0},
	/*OVLY3_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*ROT_RES*/
	//{1,0,0,0,1,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0},
	
	/*SCL2_RES*/
	{1,0,0,0,0,0,1,0,0},
	
	/* ADE_MEM_SHARE_EN */
	0
},
/*pattern 9<-------->4*/
{
    /* 
     "(ch5->scl3->ovly2 + ch6->ovly2) =>ROT=>LDI"                
     */
     
	/* 叠加摸式OVERLAY_COMP_TYPE_UINT32*/
	OVERLAY_COMP_TYPE_ONLINE,
	
	/*配置叠加通道资源CH_RES*/
	{		
			/*通道1*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道2*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道3*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道4*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道5*/
			{OVERLAY_PIPE_ADE_CH5,1,0,0,0,0,1,0,1,0,0},
			/*通道6*/
			{OVERLAY_PIPE_ADE_CH6,1,0,0,0,0,0,0,1,0,0}
	},

	/*配置显示通道DISP_CH_RES资源*/
	{0,0,0,0,0,0,0,0,0,0,0},
	
	 /*配置叠加模块资源*/
	/*OVLY1_RES*/
	{0,0,0,0,0,0},
	/*OVLY2_RES*/
	//{OVERLAY_OVLY_2,1,0,0,0,1,0,0,0,0,0}, //rot
	{OVERLAY_OVLY_2,1,0,0,0,0,0,0,1,0,0}, //ldi
	/*OVLY3_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*ROT_RES*/
	//{1,0,0,0,0,0,0,1,0,0},
	{0,0,0,0,0,0,0,0,0,0},
	
	/*SCL2_RES*/
	{0,0,0,0,0,0,0,0,0},
	
	/* ADE_MEM_SHARE_EN */
	0
	
},

/*pattern 10<-------->5*/
{
    /*      
     "(ch5->rot->ovly3 + ch6->rot->ovly3) =>SCL2=>LDI"                     
     */
     
	/* 叠加摸式OVERLAY_COMP_TYPE_UINT32*/
	OVERLAY_COMP_TYPE_ONLINE,
	
	/*配置叠加通道资源CH_RES*/
	{		
			/*通道1*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道2*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道3*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道4*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道5*/
			{OVERLAY_PIPE_ADE_CH5,1,0,1,0,0,0,0,0,1,0},
			/*通道6*/
			{OVERLAY_PIPE_ADE_CH6,1,0,1,0,0,0,0,0,1,0}
	},

	/*配置显示通道DISP_CH_RES资源*/
	{0,0,0,0,0,0,0,0,0,0,0},
	
	 /*配置叠加模块资源*/
	/*OVLY1_RES*/
	{0,0,0,0,0,0},
	/*OVLY2_RES*/
	{0,0,0,0,0,0,0,0,0,0,0},
	/*OVLY3_RES*/
	{OVERLAY_OVLY_3,1,0,0,0,1,0,0,0,0},
	
	/*ROT_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*SCL2_RES*/
	{1,0,0,0,0,0,1,0,0},

	/* ADE_MEM_SHARE_EN */
	0
},
/*pattern 11<-------->6*/
{
    /*           
     "(ch5->rot->scl3->ctran1->ovly2 + ch6->rot->scl1->ovly2) =>LDI"                        
     */
     
	/* 叠加摸式OVERLAY_COMP_TYPE_UINT32*/
	OVERLAY_COMP_TYPE_ONLINE,
	
	/*配置叠加通道资源CH_RES*/
	{		
			/*通道1*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道2*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道3*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道4*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道5*/
			{OVERLAY_PIPE_ADE_CH5,1,0,1,0,0,1,0,1,0,0},
			/*通道6*/
			{OVERLAY_PIPE_ADE_CH6,1,0,1,0,1,0,0,1,0,0}
	},

	/*配置显示通道DISP_CH_RES资源*/
	{0,0,0,0,0,0,0,0,0,0,0},
	
	 /*配置叠加模块资源*/
	/*OVLY1_RES*/
	{0,0,0,0,0,0},
	/*OVLY2_RES*/
	{OVERLAY_OVLY_2,1,0,0,0,0,0,0,1,0,0},
	/*OVLY3_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*ROT_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*SCL2_RES*/
	{0,0,0,0,0,0,0,0,0},

	/* ADE_MEM_SHARE_EN */
	0
},
/*pattern 12<-------->7*/
{
    /*        
     "(ch2->ovly2 + ch3->ovly2 + ch4->ovly2) => LDI"                            
     */
     
	/* 叠加摸式OVERLAY_COMP_TYPE_UINT32*/
	OVERLAY_COMP_TYPE_ONLINE,
	
	/*配置叠加通道资源CH_RES*/
	{		
			/*通道1*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道2*/
			{OVERLAY_PIPE_ADE_CH2,1,0,0,0,0,0,0,1,0,0},
			/*通道3*/
			{OVERLAY_PIPE_ADE_CH3,1,0,0,0,0,0,0,1,0,0},
			/*通道4*/
			{OVERLAY_PIPE_ADE_CH4,1,0,0,0,0,0,0,1,0,0},
			/*通道5*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道6*/
			{0,0,0,0,0,0,0,0,0,0,0}
	},

	/*配置显示通道DISP_CH_RES资源*/
	{0,0,0,0,0,0,0,0,0,0,0},
	
	 /*配置叠加模块资源*/
	/*OVLY1_RES*/
	{0,0,0,0,0,0},
	/*OVLY2_RES*/
	{OVERLAY_OVLY_2,1,0,0,0,0,0,0,1,0,0},
	/*OVLY3_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*ROT_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*SCL2_RES*/
	{0,0,0,0,0,0,0,0,0},

	/* ADE_MEM_SHARE_EN */
	0
},
/*pattern 13<-------->8*/
{
    /*      
     "(ch1->scl1->ovly2 + ch5->scl3->rot->ctran1->ovly2 + ch6->rot->ctran2->ovly3->scl2->ovly2) =>LDI"                                 
     */
     
	/* 叠加摸式OVERLAY_COMP_TYPE_UINT32*/
	OVERLAY_COMP_TYPE_ONLINE,
	
	/*配置叠加通道资源CH_RES*/
	{		
			/*通道1*/
			{OVERLAY_PIPE_ADE_CH1,1,0,0,0,1,0,0,1,0,0},
			/*通道2*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道3*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道4*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道5*/
			{OVERLAY_PIPE_ADE_CH5,1,0,1,0,0,1,0,1,0,0},
			/*通道6*/
			{OVERLAY_PIPE_ADE_CH6,1,0,1,0,0,0,0,0,1,0}
	},

	/*配置显示通道DISP_CH_RES资源*/
	{0,0,0,0,0,0,0,0,0,0,0},
	
	 /*配置叠加模块资源*/
	/*OVLY1_RES*/
	{0,0,0,0,0,0},

	/*OVLY2_RES*/
	/* {OVERLAY_OVLY_2,1,0,0,0,1,0,0,0,0,0},*/
	{OVERLAY_OVLY_2,1,0,0,0,0,0,0,1,0,0},
	
	/*OVLY3_RES*/
	{OVERLAY_OVLY_3,1,0,0,0,1,0,0,0,0},
	
	/*ROT_RES*/
	//{1,0,0,0,0,0,0,1,0,0},
	{0,0,0,0,0,0,0,0,0,0},	
	
	/*SCL2_RES*/
	{1,0,0,0,0,1,0,0,0},
	
	/* ADE_MEM_SHARE_EN */
	0
},
/*pattern 14<-------->9*/
{
    /*      
     "(ch1->ovly2 + ch2->ovly2 + ch3->ovly2 +ch4->ovly2) => LDI"
     */
     
	/* 叠加摸式OVERLAY_COMP_TYPE_UINT32*/
	OVERLAY_COMP_TYPE_ONLINE,
	
	/*配置叠加通道资源CH_RES*/
	{		
			/*通道1*/
			{OVERLAY_PIPE_ADE_CH1,1,0,0,0,0,0,0,1,0,0},
			/*通道2*/
			{OVERLAY_PIPE_ADE_CH2,1,0,0,0,0,0,0,1,0,0},
			/*通道3*/
			{OVERLAY_PIPE_ADE_CH3,1,0,0,0,0,0,0,1,0,0},
			/*通道4*/
			{OVERLAY_PIPE_ADE_CH4,1,0,0,0,0,0,0,1,0,0},
			/*通道5*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道6*/
			{0,0,0,0,0,0,0,0,0,0,0}
	},

	/*配置显示通道DISP_CH_RES资源*/
	{0,0,0,0,0,0,0,0,0,0,0},
	
	 /*配置叠加模块资源*/
	/*OVLY1_RES*/
	{0,0,0,0,0,0},
	/*OVLY2_RES*/
	{2,1,0,0,0,0,0,0,1,0,0},
	/*OVLY3_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*ROT_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*SCL2_RES*/
	{0,0,0,0,0,0,0,0,0},
	
	/* ADE_MEM_SHARE_EN */
	0
},
/*pattern 15<-------->10*/
{
    /*      
     "(ch1->scl1->ovly2 + ch4->ovly3->ROT->ovly2 +ch5->scl3->rot->ctran1->ovly2 + ch6->ctran2->ovly2)=>SCL2=>LDI"     
     */
     
	/* 叠加摸式OVERLAY_COMP_TYPE_UINT32*/
	OVERLAY_COMP_TYPE_ONLINE,
	
	/*配置叠加通道资源CH_RES*/
	{		
			/*通道1*/
			{OVERLAY_PIPE_ADE_CH1,1,0,0,0,1,0,0,1,0,0},
			/*通道2*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道3*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道4*/
			{OVERLAY_PIPE_ADE_CH4,1,0,0,0,0,0,0,0,1,0},
			/*通道5*/
			{OVERLAY_PIPE_ADE_CH5,1,0,1,0,0,1,0,1,0,0},
			/*通道6*/
			{OVERLAY_PIPE_ADE_CH6,1,0,0,0,0,0,0,1,0,0}
	},

	/*配置显示通道DISP_CH_RES资源*/
	{0,0,0,0,0,0,0,0,0,0,0},
	
	 /*配置叠加模块资源*/
	/*OVLY1_RES*/
	{0,0,0,0,0,0},
	/*OVLY2_RES*/
	{OVERLAY_OVLY_2,1,0,0,0,0,0,1,0,0,0},
	/*OVLY3_RES*/
	//{OVERLAY_OVLY_3,1,0,1,0,0,0,1,0,0},
	{OVERLAY_OVLY_3,1,0,0,0,0,0,1,0,0},
	
	/*ROT_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*SCL2_RES*/
	{1,0,0,0,0,0,1,0,0},
	
	/* ADE_MEM_SHARE_EN */
	0
},
/*pattern 16<-------->11*/
{
    /*      
     "(ch1->ovly3 + ch2->ovly3 +  ch3->ovly3 + ch4->ovly3 + ch5->ovly3) => LDI"
     */
     
	/* 叠加摸式OVERLAY_COMP_TYPE_UINT32*/
	OVERLAY_COMP_TYPE_ONLINE,
	
	/*配置叠加通道资源CH_RES*/
	{		
			/*通道1*/
			{OVERLAY_PIPE_ADE_CH1,1,0,0,0,0,0,0,1,0,0},
			/*通道2*/
			{OVERLAY_PIPE_ADE_CH2,1,0,0,0,0,0,0,1,0,0},
			/*通道3*/
			{OVERLAY_PIPE_ADE_CH3,1,0,0,0,0,0,0,1,0,0},
			/*通道4*/
			{OVERLAY_PIPE_ADE_CH4,1,0,0,0,0,0,0,1,0,0},
			/*通道5*/
			{OVERLAY_PIPE_ADE_CH5,1,0,0,0,0,0,0,1,0,0},
			/*通道6*/
			{0,0,0,0,0,0,0,0,0,0,0}
	},

	/*配置显示通道DISP_CH_RES资源*/
	{0,0,0,0,0,0,0,0,0,0,0},
	
	 /*配置叠加模块资源*/
	/*OVLY1_RES*/
	{0,0,0,0,0,0},
	/*OVLY2_RES*/
	{OVERLAY_OVLY_2,1,0,0,0,0,0,0,1,0,0},
	/*OVLY3_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*ROT_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*SCL2_RES*/
	{0,0,0,0,0,0,0,0,0},
	
	/* ADE_MEM_SHARE_EN */
	0
},
/*pattern 17<-------->12*/
{
    /*      
     "(ch1->ovly2 + ch2->ovly2 + ch3->ovly2 +ch5->scl3->rot->ovly2 + ch6->scl1->ctran2->ovly2)=>>SCL2=>LDI"     
     */
     
	/* 叠加摸式OVERLAY_COMP_TYPE_UINT32*/
	OVERLAY_COMP_TYPE_ONLINE,
	
	/*配置叠加通道资源CH_RES*/
	{		
			/*通道1*/
			{OVERLAY_PIPE_ADE_CH1,1,0,0,0,0,0,0,1,0,0},
			/*通道2*/
			{OVERLAY_PIPE_ADE_CH2,1,0,0,0,0,0,0,1,0,0},
			/*通道3*/
			{OVERLAY_PIPE_ADE_CH3,1,0,0,0,0,0,0,1,0,0},
			/*通道4*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道5*/
			{OVERLAY_PIPE_ADE_CH5,1,0,1,0,0,1,0,1,0,0},
			/*通道6*/
			{OVERLAY_PIPE_ADE_CH6,1,0,0,0,1,0,0,1,0,0}
	},

	/*配置显示通道DISP_CH_RES资源*/
	{0,0,0,0,0,0,0,0,0,0,0},
	
	 /*配置叠加模块资源*/
	/*OVLY1_RES*/
	{0,0,0,0,0,0},
	/*OVLY2_RES*/
	//{OVERLAY_OVLY_2,1,0,0,0,1,0,1,0,0,0},
	{OVERLAY_OVLY_2,1,0,0,0,0,0,1,0,0,0}, //ovly2->scl2->ldi
	//{OVERLAY_OVLY_2,1,0,0,0,0,0,0,1,0,0}, //ovly2->ldi

	/*OVLY3_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*ROT_RES*/
	//{1,0,0,0,1,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0},
	
	/*SCL2_RES*/
	{1,0,0,0,0,0,1,0,0},
	//{0,0,0,0,0,0,0,0,0}	

	/* ADE_MEM_SHARE_EN */
	0
},
/*pattern 18<-------->13*/
{
    /*      
     "(ch1->ovly2 + ch2->ovly2 + ch3->ovly2 + ch4->ovly3->scl2->ovly2+ch5->scl3->ovly2 +ch6->scl1->ctran2->ovly2)=>LDI"
     */
     
	/* 叠加摸式OVERLAY_COMP_TYPE_UINT32*/
	OVERLAY_COMP_TYPE_ONLINE,
	
	/*配置叠加通道资源CH_RES*/
	{		
			/*通道1*/
			{OVERLAY_PIPE_ADE_CH1,1,0,0,0,0,0,0,1,0,0},
			/*通道2*/
			{OVERLAY_PIPE_ADE_CH2,1,0,0,0,0,0,0,1,0,0},
			/*通道3*/
			{OVERLAY_PIPE_ADE_CH3,1,0,0,0,0,0,0,1,0,0},
			/*通道4*/
			{OVERLAY_PIPE_ADE_CH4,1,0,0,0,0,0,0,0,1,0},
			/*通道5*/
			{OVERLAY_PIPE_ADE_CH5,1,0,0,0,0,1,0,1,0,0},
			/*通道6*/
			{OVERLAY_PIPE_ADE_CH6,1,0,0,0,1,0,0,1,0,0}
	},

	/*配置显示通道DISP_CH_RES资源*/
	{0,0,0,0,0,0,0,0,0,0,0},
	
	 /*配置叠加模块资源*/
	/*OVLY1_RES*/
	{0,0,0,0,0,0},
	/*OVLY2_RES*/
	{OVERLAY_OVLY_2,1,0,0,0,0,0,0,1,0,0},
	/*OVLY3_RES*/
	{OVERLAY_OVLY_3,1,0,0,0,1,0,0,0,0},
	
	/*ROT_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*SCL2_RES*/
	{1,0,0,0,0,1,0,0,0},
	
	/* ADE_MEM_SHARE_EN */
	0
},
/*pattern 19<-------->14*/
{
    /*      
     "(ch1->ovly2 + ch2->ovly2 + ch3->ovly2 + ch4->ovly2+ch5->ovly2 +ch6->ovly2)=>LDI"
     */
     
	/* 叠加摸式OVERLAY_COMP_TYPE_UINT32*/
	OVERLAY_COMP_TYPE_ONLINE,
	
	/*配置叠加通道资源CH_RES*/
	{		
			/*通道1*/
			{OVERLAY_PIPE_ADE_CH1,1,0,0,0,0,0,0,1,0,0},
			/*通道2*/
			{OVERLAY_PIPE_ADE_CH2,1,0,0,0,0,0,0,1,0,0},
			/*通道3*/
			{OVERLAY_PIPE_ADE_CH3,1,0,0,0,0,0,0,1,0,0},
			/*通道4*/
			{OVERLAY_PIPE_ADE_CH4,1,0,0,0,0,0,0,1,0,0},
			/*通道5*/
			{OVERLAY_PIPE_ADE_CH5,1,0,0,0,0,0,0,1,0,0},
			/*通道6*/
			{OVERLAY_PIPE_ADE_CH6,1,0,0,0,0,0,0,1,0,0}
	},

	/*配置显示通道DISP_CH_RES资源*/
	{0,0,0,0,0,0,0,0,0,0,0},
	
	 /*配置叠加模块资源*/
	/*OVLY1_RES*/
	{0,0,0,0,0,0},
	/*OVLY2_RES*/
	{OVERLAY_OVLY_2,1,0,0,0,0,0,0,1,0,0},
	/*OVLY3_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*ROT_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*SCL2_RES*/
	{0,0,0,0,0,0,0,0,0},

	/* ADE_MEM_SHARE_EN */
	0
},
/*离线模式*/
/*pattern 20<-------->15, testcase 20*/
{   
    /*      
     disp=>WDMA3     
     */
    
	/* 叠加摸式OVERLAY_COMP_TYPE_UINT32*/
	OVERLAY_COMP_TYPE_OFFLINE,
	
	/*配置叠加通道资源CH_RES*/
	{		
			/*通道1*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道2*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道3*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道4*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道5*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道6*/
			{0,0,0,0,0,0,0,0,0,0,0}
	},

	/*配置显示通道DISP_CH_RES资源*/
	{1,0,0,0,0,0,0,0,0,1,0},
	
	 /*配置叠加模块资源*/
	/*OVLY1_RES*/
	{0,0,0,0,0,0},
	/*OVLY2_RES*/
	{0,0,0,0,0,0,0,0,0,0,0},
	/*OVLY3_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*ROT_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*SCL2_RES*/
	{0,0,0,0,0,0,0,0,0},

	/* ADE_MEM_SHARE_EN */
	0
},
#if 1
/*pattern 22<-------->16 , testcase 22*/
{
    /*    
    ch1(block_rot)->ovly1=>wdma1=>DISP_RDMA=>LDI
    */
    
	/* 叠加摸式OVERLAY_COMP_TYPE_UINT32*/
	OVERLAY_COMP_TYPE_OFFLINE,
	
	/*配置叠加通道资源CH_RES*/
	{		
			/*通道1*/
			{OVERLAY_PIPE_ADE_CH1,1,1,0,0,0,0,1,0,0,0},
			/*通道2*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道3*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道4*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道5*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道6*/
			{0,0,0,0,0,0,0,0,0,0,0}
	},

	/*配置显示通道DISP_CH_RES资源*/
	{1,1,0,0,0,0,0,0,0,0,0},
	
	 /*配置叠加模块资源*/
	/*OVLY1_RES*/
	{OVERLAY_OVLY_1,1,1,0,0,0},
	/*OVLY2_RES*/
	{0,0,0,0,0,0,0,0,0,0,0},
	/*OVLY3_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*ROT_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*SCL2_RES*/
	{0,0,0,0,0,0,0,0,0},

	/* ADE_MEM_SHARE_EN */
	0
},
/*pattern 25<-------->17*/
{
    /*    
    "(ch1->ovly1 +  ch2->ovly1)=>WDMA1=>DISP=>LDI +=>WDMA3"    
    */
    
	/* 叠加摸式OVERLAY_COMP_TYPE_UINT32*/
	OVERLAY_COMP_TYPE_OFFLINE,
	
	/*配置叠加通道资源CH_RES*/
	{		
			/*通道1*/
			{OVERLAY_PIPE_ADE_CH1,1,0,0,0,0,0,1,0,0,0},
			/*通道2*/
			{OVERLAY_PIPE_ADE_CH2,1,0,0,0,0,0,1,0,0,0},
			/*通道3*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道4*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道5*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道6*/
			{0,0,0,0,0,0,0,0,0,0,0}
	},

	/*配置显示通道DISP_CH_RES资源*/
	{1,1,0,0,0,0,0,0,0,0,0},
	
	 /*配置叠加模块资源*/
	/*OVLY1_RES*/
	{OVERLAY_OVLY_1,1,1,0,0,0},
	/*OVLY2_RES*/
	{0,0,0,0,0,0,0,0,0,0,0},
	/*OVLY3_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*ROT_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*SCL2_RES*/
	{0,0,0,0,0,0,0,0,0},

	/* ADE_MEM_SHARE_EN */
	0
},
/*pattern 26<-------->18*/
{
    /*    
    "(ch1->scl1->ovly1 +ch3->ovly3->rot->scl2->ovly1)=>ctran3=>WDMA1=>DISP=>LDI"        
    */
    
	/* 叠加摸式OVERLAY_COMP_TYPE_UINT32*/
	OVERLAY_COMP_TYPE_OFFLINE,
	
	/*配置叠加通道资源CH_RES*/
	{		
			/*通道1*/
			{OVERLAY_PIPE_ADE_CH1,1,0,0,0,1,0,1,0,0,0},
			/*通道2*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道3*/
			{OVERLAY_PIPE_ADE_CH3,1,0,0,0,0,0,0,0,1,0},
			/*通道4*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道5*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道6*/
			{0,0,0,0,0,0,0,0,0,0,0}
	},

	/*配置显示通道DISP_CH_RES资源*/
	{1,1,0,0,0,0,0,0,0,0,0},
	
	 /*配置叠加模块资源*/
	/*OVLY1_RES*/
	{OVERLAY_OVLY_1,1,1,0,0,0},
	/*OVLY2_RES*/
	{0,0,0,0,0,0,0,0,0,0,0},
	/*OVLY3_RES*/
	{OVERLAY_OVLY_3,1,0,1,0,0,0,0,1,0},
	
	/*ROT_RES*/
	{1,0,0,0,1,0,0,0,1,0},
	
	/*SCL2_RES*/
	{1,0,0,0,1,0,0,1,0},

	/* ADE_MEM_SHARE_EN */
	0
},
/*pattern 27<-------->19 */
{
    /*        
    "(ch3->ovly3->scl2->ovly2 + ch4->ovly2 ) =>ROT=>WDMA3"
    */
	/* 叠加摸式OVERLAY_COMP_TYPE_UINT32*/
	OVERLAY_COMP_TYPE_OFFLINE,
	
	/*配置叠加通道资源CH_RES*/
	{		
			/*通道1*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道2*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道3*/
			{OVERLAY_PIPE_ADE_CH3,1,0,0,0,0,0,0,0,1,0},
			/*通道4*/
			{OVERLAY_PIPE_ADE_CH4,1,0,0,0,0,0,0,1,0,0},
			/*通道5*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道6*/
			{0,0,0,0,0,0,0,0,0,0,0}
	},

	/*配置显示通道DISP_CH_RES资源*/
	{1,1,0,0,0,0,0,0,0,0,0},
	
	 /*配置叠加模块资源*/
	/*OVLY1_RES*/
	{0,0,0,0,0,0},
	/*OVLY2_RES*/
	{OVERLAY_OVLY_2,1,0,1,0,0,0,0,0,1,0},
	/*OVLY3_RES*/
	{OVERLAY_OVLY_3,1,0,0,0,1,0,0,1,0},
	
	/*ROT_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*SCL2_RES*/
	{1,0,0,0,0,1,0,1,0},

	/* ADE_MEM_SHARE_EN */
	0
},
/*pattern 28<-------->20*/
{
    /*    
    "(ch5->rot->scl1->ctran1->OVLY1 + ch6->rot->ctran2->OVLY1)=>CTRAN3=>WDMA1=>DISP=>LDI &&(ch5->rot->scl1->ctran1->OVLY1 + ch6->rot->ctran2->OVLY1) =>CTRAN4=>SCL3=>WDMA3"
    */
	/* 叠加摸式OVERLAY_COMP_TYPE_UINT32*/
	OVERLAY_COMP_TYPE_OFFLINE,
	
	/*配置叠加通道资源CH_RES*/
	{		
			/*通道1*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道2*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道3*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道4*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道5*/
			{OVERLAY_PIPE_ADE_CH5,1,0,1,0,1,0,1,0,0,0},
			/*通道6*/
			{OVERLAY_PIPE_ADE_CH6,1,0,1,0,0,0,1,0,0,0}
	},

	/*配置显示通道DISP_CH_RES资源*/
	{1,1,0,0,0,0,0,0,0,0,0},
	
	 /*配置叠加模块资源*/
	/*OVLY1_RES*/
	{OVERLAY_OVLY_1,1,1,1,1,0},
	/*OVLY2_RES*/
	{0,0,0,0,0,0,0,0,0,0,0},
	/*OVLY3_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*ROT_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*SCL2_RES*/
	{0,0,0,0,0,0,0,0,0},

	/* ADE_MEM_SHARE_EN */
	0
},
/*pattern 29<-------->21*/
{
    /*
    "(ch1->ovly1 + ch2->ovly1 + ch3->ovly1)=>WDMA1=>DISP=>LDI"
    */
	/* 叠加摸式OVERLAY_COMP_TYPE_UINT32*/
	OVERLAY_COMP_TYPE_OFFLINE,
	
	/*配置叠加通道资源CH_RES*/
	{		
			/*通道1*/
			{OVERLAY_PIPE_ADE_CH1,1,0,0,0,0,0,1,0,0,0},
			/*通道2*/
			{OVERLAY_PIPE_ADE_CH2,1,0,0,0,0,0,1,0,0,0},
			/*通道3*/
			{OVERLAY_PIPE_ADE_CH3,1,0,0,0,0,0,1,0,0,0},
			/*通道4*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道5*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道6*/
			{0,0,0,0,0,0,0,0,0,0,0}
	},

	/*配置显示通道DISP_CH_RES资源*/
	{1,1,0,0,0,0,0,0,0,0,0},
	
	 /*配置叠加模块资源*/
	/*OVLY1_RES*/
	{OVERLAY_OVLY_1,1,1,0,0,0},
	/*OVLY2_RES*/
	{0,0,0,0,0,0,0,0,0,0,0},
	/*OVLY3_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*ROT_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*SCL2_RES*/
	{0,0,0,0,0,0,0,0,0},

	/* ADE_MEM_SHARE_EN */
	0
},
/*pattern 30<-------->22*/
{
    /*    
    "(ch2->ovly1 + ch3->ovly1 + ch4->ovly1) =>CTRAN3=>WDMA1=>DISP=>ROT=>SCL2=>CTRAN6=>LDI"    
    */
    
	/* 叠加摸式OVERLAY_COMP_TYPE_UINT32*/
	OVERLAY_COMP_TYPE_OFFLINE,
	
	/*配置叠加通道资源CH_RES*/
	{		
			/*通道1*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道2*/
			{OVERLAY_PIPE_ADE_CH2,1,0,0,0,0,0,1,0,0,0},
			/*通道3*/
			{OVERLAY_PIPE_ADE_CH3,1,0,0,0,0,0,1,0,0,0},
			/*通道4*/
			{OVERLAY_PIPE_ADE_CH4,1,0,0,0,0,0,1,0,0,0},
			/*通道5*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道6*/
			{0,0,0,0,0,0,0,0,0,0,0}
	},

	/*配置显示通道DISP_CH_RES资源*/
	{1,0,0,0,1,0,0,0,0,0,0},
	
	 /*配置叠加模块资源*/
	/*OVLY1_RES*/
	{OVERLAY_OVLY_1,1,1,0,0,0},
	/*OVLY2_RES*/
	{0,0,0,0,0,0,0,0,0,0,0},
	/*OVLY3_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*ROT_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*SCL2_RES*/
	{1,0,0,0,0,0,1,0,0},

	/* ADE_MEM_SHARE_EN */
	0
},
/*pattern 31<-------->23*/
{
    /*    
    "(ch4->ovly3->scl2->ovly1 + ch5->rot->scl3->ovly1 + ch6->scl1->ovly1) =>WDMA1=>DISP=>LDI"
    */
	/* 叠加摸式OVERLAY_COMP_TYPE_UINT32*/
	OVERLAY_COMP_TYPE_OFFLINE,
	
	/*配置叠加通道资源CH_RES*/
	{		
			/*通道1*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道2*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道3*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道4*/
			{OVERLAY_PIPE_ADE_CH4,1,0,0,0,0,0,0,0,1,0},
			/*通道5*/
			{OVERLAY_PIPE_ADE_CH5,1,0,1,0,0,1,1,0,0,0},
			/*通道6*/
			{OVERLAY_PIPE_ADE_CH6,1,0,0,0,1,0,1,0,0,0}
	},

	/*配置显示通道DISP_CH_RES资源*/
	{1,1,0,0,0,0,0,0,0,0,0},
	
	 /*配置叠加模块资源*/
	/*OVLY1_RES*/
	{OVERLAY_OVLY_1,1,1,0,0,0},
	
	/*OVLY2_RES*/
	{0,0,0,0,0,0,0,0,0,0,0},
	
	/*OVLY3_RES*/
	{OVERLAY_OVLY_3,1,0,0,0,1,0,0,0,0},
	
	/*ROT_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*SCL2_RES*/
	{1,0,0,0,1,0,0,1,0},

	/* ADE_MEM_SHARE_EN */
	0
},
/*pattern 32<-------->24*/
{
    /*    
    "(ch1->ovly1 + ch2->ovly1 + ch3->ovly1 +ch4->ovly1)=>WDMA1=>DISP=>LDI"
    */
	/* 叠加摸式OVERLAY_COMP_TYPE_UINT32*/
	OVERLAY_COMP_TYPE_OFFLINE,
	
	/*配置叠加通道资源CH_RES*/
	{		
			/*通道1*/
			{OVERLAY_PIPE_ADE_CH1,1,0,0,0,0,0,1,0,0,0},
			/*通道2*/
			{OVERLAY_PIPE_ADE_CH2,1,0,0,0,0,0,1,0,0,0},
			/*通道3*/
			{OVERLAY_PIPE_ADE_CH3,1,0,0,0,0,0,1,0,0,0},
			/*通道4*/
			{OVERLAY_PIPE_ADE_CH4,1,0,0,0,0,0,1,0,0,0},
			/*通道5*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道6*/
			{0,0,0,0,0,0,0,0,0,0,0}
	},

	/*配置显示通道DISP_CH_RES资源*/
	{1,1,0,0,0,0,0,0,0,0,0},
	
	 /*配置叠加模块资源*/
	/*OVLY1_RES*/
	{OVERLAY_OVLY_1,1,1,0,0,0},
	/*OVLY2_RES*/
	{0,0,0,0,0,0,0,0,0,0,0},
	/*OVLY3_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*ROT_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*SCL2_RES*/
	{0,0,0,0,0,0,0,0,0},

	/* ADE_MEM_SHARE_EN */
	0
},
/*pattern 33<-------->25*/
{
    /* 
    "(ch3->ovly1 + ch4->ovly1 + ch5->rot->ovly1 +ch6->rot->scl1->ovly1) =>WDMA1=>DISP=>LDI 
    */
	/* 叠加摸式OVERLAY_COMP_TYPE_UINT32*/
	OVERLAY_COMP_TYPE_OFFLINE,
	
	/*配置叠加通道资源CH_RES*/
	{		
			/*通道1*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道2*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道3*/
			{OVERLAY_PIPE_ADE_CH3,1,0,0,0,0,0,1,0,0,0},
			/*通道4*/
			{OVERLAY_PIPE_ADE_CH4,1,0,0,0,0,0,1,0,0,0},
			/*通道5*/
			{OVERLAY_PIPE_ADE_CH5,1,0,1,0,0,0,1,0,0,0},
			/*通道6*/
			{OVERLAY_PIPE_ADE_CH6,1,0,1,0,1,0,1,0,0,0}
	},

	/*配置显示通道DISP_CH_RES资源*/
	{1,1,0,0,0,0,0,0,0,0,0},
	
	 /*配置叠加模块资源*/
	/*OVLY1_RES*/
	{OVERLAY_OVLY_1,1,1,0,0,0},
	/*OVLY2_RES*/
	{0,0,0,0,0,0,0,0,0,0,0},
	/*OVLY3_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*ROT_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*SCL2_RES*/
	{0,0,0,0,0,0,0,0,0},

	/* ADE_MEM_SHARE_EN */
	0
},
/*pattern 34<-------->26*/
{
    /*    
    "(ch1->ovly3 +ch2->ovly3 + ch3->ovly3 +ch4->ovly3 +ch5->ovly3) =>ovly1=>WDMA1=>DISP=>LDI
    */
	/* 叠加摸式OVERLAY_COMP_TYPE_UINT32*/
	OVERLAY_COMP_TYPE_OFFLINE,
	
	/*配置叠加通道资源CH_RES*/
	{		
			/*通道1*/
			{OVERLAY_PIPE_ADE_CH1,1,0,0,0,0,0,0,0,1,0},
			/*通道2*/
			{OVERLAY_PIPE_ADE_CH2,1,0,0,0,0,0,0,0,1,0},
			/*通道3*/
			{OVERLAY_PIPE_ADE_CH3,1,0,0,0,0,0,0,0,1,0},
			/*通道4*/
			{OVERLAY_PIPE_ADE_CH4,1,0,0,0,0,0,0,0,1,0},
			/*通道5*/
			{OVERLAY_PIPE_ADE_CH5,1,0,0,0,0,0,0,0,1,0},
			/*通道6*/
			{0,0,0,0,0,0,0,0,0,0,0}
	},

	/*配置显示通道DISP_CH_RES资源*/
	{1,1,0,0,0,0,0,0,0,0,0},
	
	 /*配置叠加模块资源*/
	/*OVLY1_RES*/
	{OVERLAY_OVLY_1,1,1,0,0,0},
	/*OVLY2_RES*/
	{0,0,0,0,0,0,0,0,0,0,0},
	/*OVLY3_RES*/
	{OVERLAY_OVLY_3,1,0,0,0,0,1,0,1,0},
	
	/*ROT_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*SCL2_RES*/
	{0,0,0,0,0,0,0,0,0},

	/* ADE_MEM_SHARE_EN */
	0
},
/*pattern 35<-------->27*/
{
    /*    
    
    "(ch2->ovly1 + ch3->ovly1 +ch4->ovly1 +ch5->ovly1 +ch6->ovly1) =>WDMA1=>DISP=>LDI 
    &&(ch2->ovly1 + ch3->ovly1 +ch4->ovly1 +ch5->ovly1 +ch6->ovly1)  =>SCL3=>WDMA3"
     
    */
	/* 叠加摸式OVERLAY_COMP_TYPE_UINT32*/
	OVERLAY_COMP_TYPE_OFFLINE,
	
	/*配置叠加通道资源CH_RES*/
	{		
			/*通道1*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道2*/
			{OVERLAY_PIPE_ADE_CH2,1,0,0,0,0,0,1,0,0,0},
			/*通道3*/
			{OVERLAY_PIPE_ADE_CH3,1,0,0,0,0,0,1,0,0,0},
			/*通道4*/
			{OVERLAY_PIPE_ADE_CH4,1,0,0,0,0,0,1,0,0,0},
			/*通道5*/
			{OVERLAY_PIPE_ADE_CH5,1,0,0,0,0,0,1,0,0,0},
			/*通道6*/
			{OVERLAY_PIPE_ADE_CH6,1,0,0,0,0,0,1,0,0,0}
	},

	/*配置显示通道DISP_CH_RES资源*/
	{1,1,0,0,0,0,0,0,0,0,0},
	
	 /*配置叠加模块资源*/
	/*OVLY1_RES*/
	{OVERLAY_OVLY_1,1,1,1,1,0},
	/*OVLY2_RES*/
	{0,0,0,0,0,0,0,0,0,0,0},
	/*OVLY3_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*ROT_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*SCL2_RES*/
	{0,0,0,0,0,0,0,0,0},

	/* ADE_MEM_SHARE_EN */
	0
},
/*pattern 36<-------->28*/
{
    /*
    "(ch1->ovly1 + ch2->ovly1 + ch3->ovly1 +ch4->ovly1 +ch5->ovly1 +ch6->ovly1) =>WDMA1=>DISP=>LDI 
    */
	/* 叠加摸式OVERLAY_COMP_TYPE_UINT32*/
	OVERLAY_COMP_TYPE_OFFLINE,
	
	/*配置叠加通道资源CH_RES*/
	{		
			/*通道1*/
			{OVERLAY_PIPE_ADE_CH1,1,0,0,0,0,0,1,0,0,0},
			/*通道2*/
			{OVERLAY_PIPE_ADE_CH2,1,0,0,0,0,0,1,0,0,0},
			/*通道3*/
			{OVERLAY_PIPE_ADE_CH3,1,0,0,0,0,0,1,0,0,0},
			/*通道4*/
			{OVERLAY_PIPE_ADE_CH4,1,0,0,0,0,0,1,0,0,0},
			/*通道5*/
			{OVERLAY_PIPE_ADE_CH5,1,0,0,0,0,0,1,0,0,0},
			/*通道6*/
			{OVERLAY_PIPE_ADE_CH6,1,0,0,0,0,0,1,0,0,0}
	},

	/*配置显示通道DISP_CH_RES资源*/
	{1,1,0,0,0,0,0,0,0,0,0},
	
	 /*配置叠加模块资源*/
	/*OVLY1_RES*/
	{OVERLAY_OVLY_1,1,1,0,0,0},
	/*OVLY2_RES*/
	{0,0,0,0,0,0,0,0,0,0,0},
	/*OVLY3_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*ROT_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*SCL2_RES*/
	{0,0,0,0,0,0,0,0,0},

	/* ADE_MEM_SHARE_EN */
	0
},

#endif
/*pattern 38<-------->29*/
{
    /*    
    "(ch6->rot->scl1->ctran->ovly1->wdma1 + ch5->rot->scl3->ovly2)=>LDI"
    */
	/* 叠加摸式OVERLAY_COMP_TYPE_UINT32*/
	OVERLAY_COMP_TYPE_HYBRID,
	
	/*配置叠加通道资源CH_RES*/
	{		
			/*通道1*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道2*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道3*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道4*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道5*/
			{OVERLAY_PIPE_ADE_CH5,1,0,1,0,0,1,0,1,0,0},
			/*通道6*/
			{OVERLAY_PIPE_ADE_CH6,1,0,1,0,1,0,1,0,0,0}
	},

	/*配置显示通道DISP_CH_RES资源*/
	{0,0,0,0,0,0,0,0,0,0,0},
	
	 /*配置叠加模块资源*/
	/*OVLY1_RES*/
	{OVERLAY_OVLY_1,1,1,0,0,0},
	/*OVLY2_RES*/
	{OVERLAY_OVLY_2,1,0,0,0,0,0,0,1,0,0},
	/*OVLY3_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*ROT_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*SCL2_RES*/
	{0,0,0,0,0,0,0,0,0},

	/* ADE_MEM_SHARE_EN */
	0
},
/*pattern 39<-------->30*/
{
    
    /* 
     * "ch5->rot->scl3->ovly1->wdma1 + ch4->ovly2 + ch6->rot->scl1->ovly2)=>LDI"
    */
	/* 叠加摸式OVERLAY_COMP_TYPE_UINT32*/
	OVERLAY_COMP_TYPE_HYBRID,
	
	/*配置叠加通道资源CH_RES*/
	{		
			/*通道1*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道2*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道3*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道4*/
			{OVERLAY_PIPE_ADE_CH4,1,0,0,0,0,0,0,1,0,0},
			/*通道5*/
			{OVERLAY_PIPE_ADE_CH5,1,0,1,0,0,1,1,0,0,0},
			/*通道6*/
			{OVERLAY_PIPE_ADE_CH6,1,0,1,0,1,0,0,1,0,0}
	},

	/*配置显示通道DISP_CH_RES资源*/
	{0,0,0,0,0,0,0,0,0,0,0},
	
	 /*配置叠加模块资源*/
	/*OVLY1_RES*/
	{OVERLAY_OVLY_1,1,1,0,0,0},
	/*OVLY2_RES*/
	{OVERLAY_OVLY_2,1,0,0,0,0,0,0,1,0,0},
	/*OVLY3_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*ROT_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*SCL2_RES*/
	{0,0,0,0,0,0,0,0,0},

	/* ADE_MEM_SHARE_EN */
	0
},
/*pattern 40<-------->31*/
{
    /*     
     "((ch5->rot->scl3->ovly1 + ch3->ovly1)=>wdma1 + ch4->ovly2 + ch6->rot->scl1->ovly2)=>LDI
     */
	/* 叠加摸式OVERLAY_COMP_TYPE_UINT32*/
	OVERLAY_COMP_TYPE_HYBRID,
	
	/*配置叠加通道资源CH_RES*/
	{		
			/*通道1*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道2*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道3*/
			{OVERLAY_PIPE_ADE_CH3,1,0,0,0,0,0,1,0,0,0},
			/*通道4*/
			{OVERLAY_PIPE_ADE_CH4,1,0,0,0,0,0,0,1,0,0},
			/*通道5*/
			{OVERLAY_PIPE_ADE_CH5,1,0,1,0,0,1,1,0,0,0},
			/*通道6*/
			{OVERLAY_PIPE_ADE_CH6,1,0,1,0,1,0,0,1,0,0}
	},

	/*配置显示通道DISP_CH_RES资源*/
	{0,0,0,0,0,0,0,0,0,0,0},
	
	 /*配置叠加模块资源*/
	/*OVLY1_RES*/
	{OVERLAY_OVLY_1,1,1,0,0,0},
	/*OVLY2_RES*/
	{OVERLAY_OVLY_2,1,0,0,0,0,0,0,1,0,0},
	/*OVLY3_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*ROT_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*SCL2_RES*/
	{0,0,0,0,0,0,0,0,0},

	/* ADE_MEM_SHARE_EN */
	0
},
/*pattern 41<-------->32*/
{
    /*
    "((ch4->ovly1 + ch5->rot->scl3->ovly1 + ch2->ovly1)=>wdma1 + ch3->ovly2 + ch6->scl1->ovly2)=>LDI"
    */
	/* 叠加摸式OVERLAY_COMP_TYPE_UINT32*/
	OVERLAY_COMP_TYPE_HYBRID,
	
	/*配置叠加通道资源CH_RES*/
	{		
			/*通道1*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道2*/
			{OVERLAY_PIPE_ADE_CH2,1,0,0,0,0,0,1,0,0,0},
			/*通道3*/
			{OVERLAY_PIPE_ADE_CH3,1,0,0,0,0,0,0,1,0,0},
			/*通道4*/
			{OVERLAY_PIPE_ADE_CH4,1,0,0,0,0,0,1,0,0,0},
			/*通道5*/
			{OVERLAY_PIPE_ADE_CH5,1,0,1,0,0,1,1,0,0,0},
			/*通道6*/
			{OVERLAY_PIPE_ADE_CH6,1,0,0,0,1,0,0,1,0,0}
	},

	/*配置显示通道DISP_CH_RES资源*/
	{0,0,0,0,0,0,0,0,0,0,0},
	
	 /*配置叠加模块资源*/
	/*OVLY1_RES*/
	{OVERLAY_OVLY_1,1,1,0,0,0},
	/*OVLY2_RES*/
	{OVERLAY_OVLY_2,1,0,0,0,0,0,0,1,0,0},
	/*OVLY3_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*ROT_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*SCL2_RES*/
	{0,0,0,0,0,0,0,0,0},

	/* ADE_MEM_SHARE_EN */
	0
},
/*pattern 42<-------->33*/
{
    /*     
    "((ch5->rot->scl3->ovly1 + ch6->scl1->ctran->ovly1)=>WDMA1 + (ch4->ovly2 + ch3->ovly2 + ch2->ovly2 + ch1->ovly2))=>SCL2=>LDI"
    */
	/* 叠加摸式OVERLAY_COMP_TYPE_UINT32*/
	OVERLAY_COMP_TYPE_HYBRID,
	
	/*配置叠加通道资源CH_RES*/
	{		
			/*通道1*/
			{OVERLAY_PIPE_ADE_CH1,1,0,0,0,0,0,0,1,0,0},
			/*通道2*/
			{OVERLAY_PIPE_ADE_CH2,1,0,0,0,0,0,0,1,0,0},
			/*通道3*/
			{OVERLAY_PIPE_ADE_CH3,1,0,0,0,0,0,0,1,0,0},
			/*通道4*/
			{OVERLAY_PIPE_ADE_CH4,1,0,0,0,0,0,0,1,0,0},
			/*通道5*/
			{OVERLAY_PIPE_ADE_CH5,1,0,1,0,0,1,1,0,0,0},
			/*通道6*/
			{OVERLAY_PIPE_ADE_CH6,1,0,0,0,1,0,1,0,0,0}
	},

	/*配置显示通道DISP_CH_RES资源*/
	{0,0,0,0,0,0,0,0,0,0,0},
	
	 /*配置叠加模块资源*/
	/*OVLY1_RES*/
	{OVERLAY_OVLY_1,1,1,0,0,0},
	/*OVLY2_RES*/
	{OVERLAY_OVLY_2,1,0,0,0,0,0,1,0,0,0},
	/*OVLY3_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*ROT_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*SCL2_RES*/
	{1,0,0,0,0,0,1,0,0},

	/* ADE_MEM_SHARE_EN */
	0
},
/* pattern 34 */
/*z00196871 add for FPGA test on 2013-12-18*/
{
	/* 叠加摸式OVERLAY_COMP_TYPE_UINT32*/
	OVERLAY_COMP_TYPE_ONLINE,
	
	/*配置叠加通道资源CH_RES*/
	{		
			/*通道1*/
			{OVERLAY_PIPE_ADE_CH1,1,0,0,0,0,0,0,1,0,0},
			/*通道2*/
			{OVERLAY_PIPE_ADE_CH2,1,0,0,0,0,0,0,1,0,0},
			/*通道3*/
			{OVERLAY_PIPE_ADE_CH3,1,0,0,0,0,0,0,1,0,0},
			/*通道4*/
			{OVERLAY_PIPE_ADE_CH4,1,0,0,0,0,0,0,0,1,0},
			/*通道5*/
			{OVERLAY_PIPE_ADE_CH5,1,0,0,0,0,1,0,1,0,0},
			/*通道6*/
			{OVERLAY_PIPE_ADE_CH6,1,0,0,1,1,0,0,1,0,0}
	},

	/*配置显示通道DISP_CH_RES资源*/
	{0,0,0,0,0,0,0,0,0,0,0},
	
	 /*配置叠加模块资源*/
	/*OVLY1_RES*/
	{0,0,0,0,0,0},
	/*OVLY2_RES*/
	{OVERLAY_OVLY_2,1,0,0,0,0,0,0,1,0,0},
	/*OVLY3_RES*/
	{OVERLAY_OVLY_3,1,0,0,0,1,0,0,0,0},
	
	/*ROT_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*SCL2_RES*/
	{1,0,0,0,0,1,0,0,0},

	/* ADE_MEM_SHARE_EN */
	0
},

/* pattern 35 */
/*z00206003 add for FPGA test on 2014-02-13*/
{
	/* 叠加摸式OVERLAY_COMP_TYPE_UINT32*/
	OVERLAY_COMP_TYPE_ONLINE,
	
	/*配置叠加通道资源CH_RES*/
	{		
			/*通道1*/
			{OVERLAY_PIPE_ADE_CH1,1,0,0,0,0,0,0,1,0,0},
			/*通道2*/
			{OVERLAY_PIPE_ADE_CH2,1,0,0,0,0,0,0,1,0,0},
			/*通道3*/
			{OVERLAY_PIPE_ADE_CH3,1,0,0,0,0,0,0,1,0,0},
			/*通道4*/
			{OVERLAY_PIPE_ADE_CH4,1,0,0,0,0,0,0,0,1,0},
			/*通道5*/
			{OVERLAY_PIPE_ADE_CH5,1,0,1,0,0,1,0,1,0,0},
			/*通道6*/
			{OVERLAY_PIPE_ADE_CH6,1,0,1,0,1,0,0,1,0,0}
	},

	/*配置显示通道DISP_CH_RES资源*/
	{0,0,0,0,0,0,0,0,0,0,0},
	
	 /*配置叠加模块资源*/
	/*OVLY1_RES*/
	{0,0,0,0,0,0},
	/*OVLY2_RES*/
	{OVERLAY_OVLY_2,1,0,0,0,0,0,0,1,0,0},
	/*OVLY3_RES*/
	{OVERLAY_OVLY_3,1,0,0,0,1,0,0,0,0},
	
	/*ROT_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*SCL2_RES*/
	{1,0,0,0,0,1,0,0,0},

	/* ADE_MEM_SHARE_EN */
	0
},

/* pattern 36 */
/*z00206003 add for FPGA test on 2014-02-13*/
{
	/* 叠加摸式OVERLAY_COMP_TYPE_UINT32*/
	OVERLAY_COMP_TYPE_ONLINE,
	
	/*配置叠加通道资源CH_RES*/
	{		
			/*通道1*/
			{OVERLAY_PIPE_ADE_CH1,1,0,0,0,0,0,0,1,0,0},
			/*通道2*/
			{OVERLAY_PIPE_ADE_CH2,1,0,0,0,0,0,0,1,0,0},
			/*通道3*/
			{OVERLAY_PIPE_ADE_CH3,1,0,0,0,0,0,0,1,0,0},
			/*通道4*/
			{OVERLAY_PIPE_ADE_CH4,1,0,0,0,0,0,0,0,1,0},
			/*通道5*/
			{OVERLAY_PIPE_ADE_CH5,1,0,0,0,0,1,0,1,0,0},
			/*通道6*/
			{OVERLAY_PIPE_ADE_CH6,1,0,1,0,1,0,0,1,0,0}
	},

	/*配置显示通道DISP_CH_RES资源*/
	{0,0,0,0,0,0,0,0,0,0,0},
	
	 /*配置叠加模块资源*/
	/*OVLY1_RES*/
	{0,0,0,0,0,0},
	/*OVLY2_RES*/
	{OVERLAY_OVLY_2,1,0,0,0,0,0,0,1,0,0},
	/*OVLY3_RES*/
	{OVERLAY_OVLY_3,1,0,0,0,1,0,0,0,0},
	
	/*ROT_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*SCL2_RES*/
	{1,0,0,0,0,1,0,0,0},
	
	/* ADE_MEM_SHARE_EN */
	0
},


/* pattern 37 */
{
	/* 叠加摸式OVERLAY_COMP_TYPE_UINT32*/
	OVERLAY_COMP_TYPE_ONLINE,
	
	/*配置叠加通道资源CH_RES*/
	{		
			/*通道1*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道2*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道3*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道4*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道5*/
			{0,0,0,0,0,0,0,0,0,0,0},
			/*通道6*/
			{0,0,0,0,0,0,0,0,0,0,0},
	},

	/*配置显示通道DISP_CH_RES资源*/
	{0,0,0,0,0,0,0,0,0,0,0},
	
	 /*配置叠加模块资源*/
	/*OVLY1_RES*/
	{0,0,0,0,0,0},
	/*OVLY2_RES*/
	{0, 0,0,0,0,0,0,0, 0,0,0},
	/*OVLY3_RES*/
	{ 0, 0,0,0,0,0 ,0,0,0,0},
	
	/*ROT_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*SCL2_RES*/
	{0,0,0,0,0,0,0,0,0},

	/* ADE_MEM_SHARE_EN */
	0
},

/* pattern 38 */
{
    /*    
    "(ch1->ovly1 + ch2->ovly1 + ch3->ovly1 +ch4->scl2->ovly1 + ch5->ROT->scl3->ovly1 + ch6->ROT->scl1->ovly1)=>WDMA1=>DISP=>LDI"
    */
	/* 叠加摸式OVERLAY_COMP_TYPE_UINT32*/
	OVERLAY_COMP_TYPE_OFFLINE,
	
	/*配置叠加通道资源CH_RES*/
	{		
			/*通道1*/
			{OVERLAY_PIPE_ADE_CH1,1,0,0,0,0,0,1,0,0,0},
			/*通道2*/
			{OVERLAY_PIPE_ADE_CH2,1,0,0,0,0,0,1,0,0,0},
			/*通道3*/
			{OVERLAY_PIPE_ADE_CH3,1,0,0,0,0,0,1,0,0,0},
			/*通道4*/
			{OVERLAY_PIPE_ADE_CH4,1,0,0,0,0,0,0,0,1,0}, /*ovly3*/
			/*通道5*/
			{OVERLAY_PIPE_ADE_CH5,1,0,1,0,0,1,1,0,0,0},
			/*通道6*/
			{OVERLAY_PIPE_ADE_CH6,1,0,1,0,1,0,1,0,0,0},
	},

	/*配置显示通道DISP_CH_RES资源*/
	{1,1,0,0,0,0,0,0,0,0,0},
	
	 /*配置叠加模块资源*/
	/*OVLY1_RES*/
	{OVERLAY_OVLY_1,1,1,0,0,0},
	/*OVLY2_RES*/
	{0,0,0,0,0,0,0,0,0,0,0},
	/*OVLY3_RES*/
	{OVERLAY_OVLY_3,1,0,0,0,1,0,0,0,0}, /*ovly3->scl2*/
	
	/*ROT_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*SCL2_RES*/
	{1,0,0,0,1,0,0,1,0},

	/* ADE_MEM_SHARE_EN */
	0
},
/*================caogouchen=====begin=======*/
/*pattern 39*/
{
    /*    
    "(ch1->ovly1 + ch2->ovly1 + ch3->ovly1 +ch4->scl2->ovly1 + ch5->scl3->ovly1 + ch6->ROT->scl1->ovly1)=>WDMA1=>DISP=>LDI"
    */
	/* 叠加摸式OVERLAY_COMP_TYPE_UINT32*/
	OVERLAY_COMP_TYPE_OFFLINE,
	
	/*配置叠加通道资源CH_RES*/
	{		
			/*通道1*/
			{OVERLAY_PIPE_ADE_CH1,1,0,0,0,0,0,1,0,0,0},
			/*通道2*/
			{OVERLAY_PIPE_ADE_CH2,1,0,0,0,0,0,1,0,0,0},
			/*通道3*/
			{OVERLAY_PIPE_ADE_CH3,1,0,0,0,0,0,1,0,0,0},
			/*通道4*/
			{OVERLAY_PIPE_ADE_CH4,1,0,0,0,0,0,0,0,1,0}, /*ovly3*/
			/*通道5*/
			{OVERLAY_PIPE_ADE_CH5,1,0,0,0,0,1,1,0,0,0},
			/*通道6*/
			{OVERLAY_PIPE_ADE_CH6,1,0,1,0,1,0,1,0,0,0},
	},

	/*配置显示通道DISP_CH_RES资源*/
	{1,1,0,0,0,0,0,0,0,0,0},
	
	 /*配置叠加模块资源*/
	/*OVLY1_RES*/
	{OVERLAY_OVLY_1,1,1,0,0,0},
	
	/*OVLY2_RES*/
	{0,0,0,0,0,0,0,0,0,0,0},
	/*OVLY3_RES*/
	//{OVERLAY_OVLY_3,1,0,0,0,1,0,0,0,0}, /*ovly3->scl2*/ ok
	{OVERLAY_OVLY_3,1,0,0,0,1,0,0,1,0}, /*ovly3->scl2*/
	
	/*ROT_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*SCL2_RES*/
	//{1,0,0,0,1,0,0,0,0} //ok
	{1,0,0,0,1,0,0,1,0},

	/* ADE_MEM_SHARE_EN */
	0
},

/*pattern 40*/
{
    /*    
    "(ch1->ovly1 + ch2->ovly1 + ch3->ovly1 +ch4->scl2->ovly1 + ch5->scl3->ovly1 + ch6->ROT->scl1->ovly1)=>WDMA1=>DISP=>LDI"
    */
	/* 叠加摸式OVERLAY_COMP_TYPE_UINT32*/
	OVERLAY_COMP_TYPE_OFFLINE,
	
	/*配置叠加通道资源CH_RES*/
	{		
			/*通道1*/
			{OVERLAY_PIPE_ADE_CH1,1,0,0,0,0,0,1,0,0,0},
			/*通道2*/
			{OVERLAY_PIPE_ADE_CH2,1,0,0,0,0,0,1,0,0,0},
			/*通道3*/
			{OVERLAY_PIPE_ADE_CH3,1,0,0,0,0,0,1,0,0,0},
			/*通道4*/
			{OVERLAY_PIPE_ADE_CH4,1,0,0,0,0,0,0,0,1,0}, /*ovly3*/
			/*通道5*/
			{OVERLAY_PIPE_ADE_CH5,1,0,0,0,0,1,1,0,0,0},
			/*通道6*/
			{OVERLAY_PIPE_ADE_CH6,1,1,0,0,1,0,1,0,0,0},
	},

	/*配置显示通道DISP_CH_RES资源*/
	{1,1,0,0,0,0,0,0,0,0,0},
	
	 /*配置叠加模块资源*/
	/*OVLY1_RES*/
	{OVERLAY_OVLY_1,1,1,0,0,0},
	/*OVLY2_RES*/
	{0,0,0,0,0,0,0,0,0,0,0},
	/*OVLY3_RES*/
	{OVERLAY_OVLY_3,1,0,0,0,1,0,0,0,0}, /*ovly3->scl2*/
	
	/*ROT_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*SCL2_RES*/
	{1,0,0,0,1,0,0,1,0},

	/* ADE_MEM_SHARE_EN */
	0
},
/*pattern 41*/
{
    /*    
    "(ch1->ovly1 + ch2->ovly1 + ch3->ovly1 +ch4->ovly1 + ch5->ROT->scl2->ovly1 + ch6->ROT->scl1->ovly1)=>WDMA1=>DISP=>LDI"
    */
	/* 叠加摸式OVERLAY_COMP_TYPE_UINT32*/
	OVERLAY_COMP_TYPE_OFFLINE,
	
	/*配置叠加通道资源CH_RES*/
	{		
			/*通道1*/
			{OVERLAY_PIPE_ADE_CH1,1,0,0,0,0,0,1,0,0,0},
			/*通道2*/
			{OVERLAY_PIPE_ADE_CH2,1,0,0,0,0,0,1,0,0,0},
			/*通道3*/
			{OVERLAY_PIPE_ADE_CH3,1,0,0,0,0,0,1,0,0,0},
			/*通道4*/
			{OVERLAY_PIPE_ADE_CH4,1,0,0,0,0,0,1,0,0,0}, 
			/*通道5*/
			{OVERLAY_PIPE_ADE_CH5,1,0,1,0,0,0,0,0,1,0},/*ovly3->scl2*/
			/*通道6*/
			{OVERLAY_PIPE_ADE_CH6,1,0,1,0,1,0,1,0,0,0},
	},

	/*配置显示通道DISP_CH_RES资源*/
	{1,1,0,0,0,0,0,0,0,0,0},
	
	 /*配置叠加模块资源*/
	/*OVLY1_RES*/
	{OVERLAY_OVLY_1,1,1,0,0,0},
	/*OVLY2_RES*/
	{0,0,0,0,0,0,0,0,0,0,0},
	/*OVLY3_RES*/
	{OVERLAY_OVLY_3,1,0,0,0,1,0,0,0,0}, /*ovly3->scl2*/
	
	/*ROT_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*SCL2_RES*/
	{1,0,0,0,1,0,0,1,0},
	
	/* ADE_MEM_SHARE_EN */
	0
},
#if 0
/*================caogouchen============*/
/*pattern 42*/
{
    /*    
    "(ch1->ovly1 + ch2->ovly1 + ch3->ovly1 +ch4->ovly1 + ch5->ROT->scl2->ovly1 + ch6->ROT->scl1->ovly1)=>WDMA1=>DISP=>LDI"
    */
	/* 叠加摸式OVERLAY_COMP_TYPE_UINT32*/
	OVERLAY_COMP_TYPE_OFFLINE,
	
	/*配置叠加通道资源CH_RES*/
	{		
			/*通道1*/
			{OVERLAY_PIPE_ADE_CH1,1,0,0,0,0,0,1,0,0,0},
			/*通道2*/
			{OVERLAY_PIPE_ADE_CH2,1,0,0,0,0,0,1,0,0,0},
			/*通道3*/
			{OVERLAY_PIPE_ADE_CH3,1,0,0,0,0,0,1,0,0,0},
			/*通道4*/
			{OVERLAY_PIPE_ADE_CH4,1,0,0,0,0,0,1,0,0,0}, 
			/*通道5*/
			{OVERLAY_PIPE_ADE_CH5,1,0,1,0,0,0,0,0,1,0},/*ovly3->scl2*/
			/*通道6*/
			{OVERLAY_PIPE_ADE_CH6,1,0,1,0,1,0,1,0,0,0},
	},

	/*配置显示通道DISP_CH_RES资源*/
	{0,0,0,0,0,0,0,0,0,0,0},
	
	 /*配置叠加模块资源*/
	/*OVLY1_RES*/
	{0,0,0,0,0,0},
	/*OVLY2_RES*/
	{0, 0,0,0,0,0,0,0,0 ,0,0},
	/*OVLY3_RES*/
	{ 0, 0,0,0,0,0 ,0,0,0,0},
	
	/*ROT_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*SCL2_RES*/
	{0,0,0,0,0,0,0,0,0},

	/* ADE_MEM_SHARE_EN */
	0
},
#endif
/* pattern 42*/
/*z00206003 add for FPGA test  wifi display on 2014-02-14*/
{
	/* 叠加摸式OVERLAY_COMP_TYPE_UINT32*/
	OVERLAY_COMP_TYPE_ONLINE,
	
	/*配置叠加通道资源CH_RES*/
	{		
			/*通道1*/
			{OVERLAY_PIPE_ADE_CH1,1,0,0,0,0,0,0,1,0,0},
			/*通道2*/
			{OVERLAY_PIPE_ADE_CH2,1,0,0,0,0,0,0,1,0,0},
			/*通道3*/
			{OVERLAY_PIPE_ADE_CH3,1,0,0,0,0,0,0,1,0,0},
			/*通道4*/
			{OVERLAY_PIPE_ADE_CH4,1,0,0,0,0,0,0,1,0,0},
			/*通道5*/
			{OVERLAY_PIPE_ADE_CH5,1,0,1,0,0,0,0,0,1,0},
			/*通道6*/
			{OVERLAY_PIPE_ADE_CH6,1,0,1,0,1,0,0,1,0,0}
	},

	/*配置显示通道DISP_CH_RES资源*/
	{0,0,0,0,0,0,0,0,0,0,0},
	
	 /*配置叠加模块资源*/
	/*OVLY1_RES*/
	{0,0,0,0,0,0},
	/*OVLY2_RES*/
	{OVERLAY_OVLY_2,1,0,1,1,0,0,0,1,0,0},
	/*OVLY3_RES*/
	{OVERLAY_OVLY_3,1,0,0,0,1,0,0,0,0},
	
	/*ROT_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*SCL2_RES*/
	{1,0,0,0,0,1,0,0,0},

	/* ADE_MEM_SHARE_EN */
	0
},

/* pattern 43*/
/*z00206003 add for FPGA test  wifi display on 2014-02-14*/
{
	/* 叠加摸式OVERLAY_COMP_TYPE_UINT32*/
	OVERLAY_COMP_TYPE_ONLINE,
	
	/*配置叠加通道资源CH_RES*/
	{		
			/*通道1*/
			{OVERLAY_PIPE_ADE_CH1,1,0,0,0,0,0,0,1,0,0},
			/*通道2*/
			{OVERLAY_PIPE_ADE_CH2,1,0,0,0,0,0,0,1,0,0},
			/*通道3*/
			{OVERLAY_PIPE_ADE_CH3,1,0,0,0,0,0,0,1,0,0},
			/*通道4*/
			{OVERLAY_PIPE_ADE_CH4,1,0,0,0,0,0,0,1,0,0},
			/*通道5*/
			{OVERLAY_PIPE_ADE_CH5,1,0,0,0,0,0,0,0,1,0},
			/*通道6*/
			{OVERLAY_PIPE_ADE_CH6,1,0,1,0,1,0,0,1,0,0}
	},

	/*配置显示通道DISP_CH_RES资源*/
	{0,0,0,0,0,0,0,0,0,0,0},
	
	 /*配置叠加模块资源*/
	/*OVLY1_RES*/
	{0,0,0,0,0,0},
	/*OVLY2_RES*/
	{OVERLAY_OVLY_2,1,0,1,1,0,0,0,1,0,0},
	/*OVLY3_RES*/
	{OVERLAY_OVLY_3,1,0,0,0,1,0,0,0,0},
	
	/*ROT_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*SCL2_RES*/
	{1,0,0,0,0,1,0,0,0},

	/* ADE_MEM_SHARE_EN */
	0
},

/* pattern 44*/
/*z00206003 add for FPGA test  wifi display on 2014-02-14*/
{
	/* 叠加摸式OVERLAY_COMP_TYPE_UINT32*/
	OVERLAY_COMP_TYPE_HYBRID,
	
	/*配置叠加通道资源CH_RES*/
	{		
			/*通道1*/
			{OVERLAY_PIPE_ADE_CH1,1,0,0,0,0,0,0,1,0,0},
			/*通道2*/
			{OVERLAY_PIPE_ADE_CH2,1,0,0,0,0,0,0,1,0,0},
			/*通道3*/
			{OVERLAY_PIPE_ADE_CH3,1,0,0,0,0,0,0,1,0,0},
			/*通道4*/
			{OVERLAY_PIPE_ADE_CH4,1,0,0,0,0,0,0,1,0,0},
			/*通道5*/
			{OVERLAY_PIPE_ADE_CH5,1,0,1,0,1,0,0,1,0,0},
			/*通道6*/
			{OVERLAY_PIPE_ADE_CH6,1,1,0,0,0,0,0,0,1,0}
	},

	/*配置显示通道DISP_CH_RES资源*/
	{0,0,0,0,0,0,0,0,0,0,0},
	
	 /*配置叠加模块资源*/
	/*OVLY1_RES*/
	{OVERLAY_OVLY_1,1,1,0,0,0},
	/*OVLY2_RES*/
	{OVERLAY_OVLY_2,1,0,1,1,0,0,0,1,0,0},
	/*OVLY3_RES*/
	{OVERLAY_OVLY_3,1,0,0,0,1,0,0,0,0},
	
	/*ROT_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*SCL2_RES*/
	{1,0,0,0,1,0,0,0,0},

	/* ADE_MEM_SHARE_EN */
	0
},
/* pattern 45*/
/*z00196871 add for FPGA test  Hybrid(720P)  Pattern1 on 2014-02-26*/
{
    /*     
     "((ch1(block)->ovly1 + ch2->ovly1)=>wdma1 + ch3->ovly2 + ch4->ovly3->scl2->ovly2+ch5(720P rot)->scl3->ovly2+ch6(720P rot)->scl1->ovly2)=>LDI
     */
	/* 叠加摸式OVERLAY_COMP_TYPE_UINT32*/
	OVERLAY_COMP_TYPE_HYBRID,
	
	/*配置叠加通道资源CH_RES*/
	{		
			/*通道1*/
			{OVERLAY_PIPE_ADE_CH1,1,1,0,0,0,0,1,0,0,0},
			/*通道2*/
			{OVERLAY_PIPE_ADE_CH2,1,0,0,0,0,0,1,0,0,0},
			/*通道3*/
			{OVERLAY_PIPE_ADE_CH3,1,0,0,0,0,0,0,1,0,0},
			/*通道4*/
			{OVERLAY_PIPE_ADE_CH4,1,0,0,0,0,0,0,0,1,0},
			/*通道5*/
			{OVERLAY_PIPE_ADE_CH5,1,0,1,0,0,1,0,1,0,0},
			/*通道6*/
			{OVERLAY_PIPE_ADE_CH6,1,0,1,0,1,0,0,1,0,0}
	},

	/*配置显示通道DISP_CH_RES资源*/
	{0,0,0,0,0,0,0,0,0,0,0},
	
	 /*配置叠加模块资源*/
	/*OVLY1_RES*/
	{1,1,1,0,0,0},
	/*OVLY2_RES*/
	{OVERLAY_OVLY_2,1,0,0,0,0,0,0,1,0,0},
	/*OVLY3_RES*/
	{OVERLAY_OVLY_3,1,0,0,0,1,0,0,0,0},
	
	/*ROT_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*SCL2_RES*/
	{1,0,0,0,0,1,0,0,0},

	/* ADE_MEM_SHARE_EN */
	0
},
/* pattern 46*/
/*z00196871 add for FPGA test  Hybrid(720P)  Pattern2 on 2014-02-27*/
{
    /*     
     "((ch1(block)->ovly1 + ch2->ovly1)=>wdma1 + ch3->ovly2 + ch4->ovly3->scl2->ovly2+ch5->scl3->ovly2+ch6(720P rot)->scl1->ovly2)=>LDI
     */
	/* 叠加摸式OVERLAY_COMP_TYPE_UINT32*/
	OVERLAY_COMP_TYPE_HYBRID,
	
	/*配置叠加通道资源CH_RES*/
	{		
			/*通道1*/
			{OVERLAY_PIPE_ADE_CH1,1,1,0,0,0,0,1,0,0,0},
			/*通道2*/
			{OVERLAY_PIPE_ADE_CH2,1,0,0,0,0,0,1,0,0,0},
			/*通道3*/
			{OVERLAY_PIPE_ADE_CH3,1,0,0,0,0,0,0,1,0,0},
			/*通道4*/
			{OVERLAY_PIPE_ADE_CH4,1,0,0,0,0,0,0,0,1,0},
			/*通道5*/
			{OVERLAY_PIPE_ADE_CH5,1,0,0,0,0,1,0,1,0,0},
			/*通道6*/
			{OVERLAY_PIPE_ADE_CH6,1,0,1,0,1,0,0,1,0,0}
	},

	/*配置显示通道DISP_CH_RES资源*/
	{0,0,0,0,0,0,0,0,0,0,0},
	
	 /*配置叠加模块资源*/
	/*OVLY1_RES*/
	{1,1,1,0,0,0},
	/*OVLY2_RES*/
	{OVERLAY_OVLY_2,1,0,0,0,0,0,0,1,0,0},
	/*OVLY3_RES*/
	{OVERLAY_OVLY_3,1,0,0,0,1,0,0,0,0},
	
	/*ROT_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*SCL2_RES*/
	{1,0,0,0,0,1,0,0,0},

	/* ADE_MEM_SHARE_EN */
	0
},
/* pattern 47*/
/*z00196871 add for FPGA test  Hybrid(720P)  Pattern3 on 2014-02-27*/
{
    /*     
     "((ch1(block)->ovly1 + ch6(720P rot)->scl1->ovly1)=>wdma1 +ch2->ovly2+ ch3->ovly2 + ch4->ovly3->scl2->ovly2+ch5(720P rot)->scl3->ovly2)=>LDI
     */
	/* 叠加摸式OVERLAY_COMP_TYPE_UINT32*/
	OVERLAY_COMP_TYPE_HYBRID,
	
	/*配置叠加通道资源CH_RES*/
	{		
			/*通道1*/
			{OVERLAY_PIPE_ADE_CH1,1,1,0,0,0,0,1,0,0,0},
			/*通道2*/
			{OVERLAY_PIPE_ADE_CH2,1,0,0,0,0,0,0,1,0,0},
			/*通道3*/
			{OVERLAY_PIPE_ADE_CH3,1,0,0,0,0,0,0,1,0,0},
			/*通道4*/
			{OVERLAY_PIPE_ADE_CH4,1,0,0,0,0,0,0,0,1,0},
			/*通道5*/
			{OVERLAY_PIPE_ADE_CH5,1,0,1,0,0,1,0,1,0,0},
			/*通道6*/
			{OVERLAY_PIPE_ADE_CH6,1,0,1,0,1,0,1,0,0,0}
	},

	/*配置显示通道DISP_CH_RES资源*/
	{0,0,0,0,0,0,0,0,0,0,0},
	
	 /*配置叠加模块资源*/
	/*OVLY1_RES*/
	{1,1,1,0,0,0},
	/*OVLY2_RES*/
	{OVERLAY_OVLY_2,1,0,0,0,0,0,0,1,0,0},
	/*OVLY3_RES*/
	{OVERLAY_OVLY_3,1,0,0,0,1,0,0,0,0},
	
	/*ROT_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*SCL2_RES*/
	{1,0,0,0,0,1,0,0,0},

	/* ADE_MEM_SHARE_EN */
	0
},
/* pattern 48*/
/*z00196871 add for FPGA test  Hybrid(720P)  Pattern4 on 2014-02-27*/
{
    /*     
     "((ch1->ovly1 + ch6(blockt)->scl1->ovly1)=>wdma1 +ch2->ovly2+ ch3->ovly2 + ch4->ovly3->scl2->ovly2+ch5(720P rot)->scl3->ovly2)=>LDI
     */
	/* 叠加摸式OVERLAY_COMP_TYPE_UINT32*/
	OVERLAY_COMP_TYPE_HYBRID,
	
	/*配置叠加通道资源CH_RES*/
	{		
			/*通道1*/
			{OVERLAY_PIPE_ADE_CH1,1,0,0,0,0,0,1,0,0,0},
			/*通道2*/
			{OVERLAY_PIPE_ADE_CH2,1,0,0,0,0,0,0,1,0,0},
			/*通道3*/
			{OVERLAY_PIPE_ADE_CH3,1,0,0,0,0,0,0,1,0,0},
			/*通道4*/
			{OVERLAY_PIPE_ADE_CH4,1,0,0,0,0,0,0,0,1,0},
			/*通道5*/
			{OVERLAY_PIPE_ADE_CH5,1,0,1,0,0,1,0,1,0,0},
			/*通道6*/
			{OVERLAY_PIPE_ADE_CH6,1,1,0,0,1,0,1,0,0,0}
	},

	/*配置显示通道DISP_CH_RES资源*/
	{0,0,0,0,0,0,0,0,0,0,0},
	
	 /*配置叠加模块资源*/
	/*OVLY1_RES*/
	{1,1,1,0,0,0},
	/*OVLY2_RES*/
	{OVERLAY_OVLY_2,1,0,0,0,0,0,0,1,0,0},
	/*OVLY3_RES*/
	{OVERLAY_OVLY_3,1,0,0,0,1,0,0,0,0},
	
	/*ROT_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*SCL2_RES*/
	{1,0,0,0,0,1,0,0,0},

	/* ADE_MEM_SHARE_EN */
	0
},
/* pattern 49*/
/*z00196871 add for FPGA test  Hybrid(720P)  Pattern6 on 2014-02-27*/
{
    /*     
     "((ch1->ovly3->scl2->ovly1 + ch6(blockt)->scl1->ovly1)=>wdma1 +ch2->ovly2+ ch3->ovly2 + ch4->ovly2+ch5(720P rot)->scl3->ovly2)=>LDI
     */
	/* 叠加摸式OVERLAY_COMP_TYPE_UINT32*/
	OVERLAY_COMP_TYPE_HYBRID,
	
	/*配置叠加通道资源CH_RES*/
	{		
			/*通道1*/
			{OVERLAY_PIPE_ADE_CH1,1,0,0,0,0,0,0,0,1,0},
			/*通道2*/
			{OVERLAY_PIPE_ADE_CH2,1,0,0,0,0,0,0,1,0,0},
			/*通道3*/
			{OVERLAY_PIPE_ADE_CH3,1,0,0,0,0,0,0,1,0,0},
			/*通道4*/
			{OVERLAY_PIPE_ADE_CH4,1,0,0,0,0,0,0,1,0,0},
			/*通道5*/
			{OVERLAY_PIPE_ADE_CH5,1,0,1,0,0,1,0,1,0,0},
			/*通道6*/
			{OVERLAY_PIPE_ADE_CH6,1,1,0,0,1,0,1,0,0,0}
	},

	/*配置显示通道DISP_CH_RES资源*/
	{0,0,0,0,0,0,0,0,0,0,0},
	
	 /*配置叠加模块资源*/
	/*OVLY1_RES*/
	{1,1,1,0,0,0},
	/*OVLY2_RES*/
	{OVERLAY_OVLY_2,1,0,0,0,0,0,0,1,0,0},
	/*OVLY3_RES*/
	{OVERLAY_OVLY_3,1,0,0,0,1,0,0,0,0},
	
	/*ROT_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*SCL2_RES*/
	{1,0,0,0,1,0,0,0,0},

	/* ADE_MEM_SHARE_EN */
	0
},
/* pattern 50*/
/*z00196871 add for FPGA test  Hybrid(720P)  Pattern8 on 2014-02-27*/
{
    /*     
     "((ch1->ovly1 +ch3->ovly1+ ch4->ovly3->scl2->ovly1+ch6(block)->scl1->ovly1)=>wdma1 +ch2->ovly2+ch5(720P rot)->scl3->ovly2)=>LDI
     */
	/* 叠加摸式OVERLAY_COMP_TYPE_UINT32*/
	OVERLAY_COMP_TYPE_HYBRID,
	
	/*配置叠加通道资源CH_RES*/
	{		
			/*通道1*/
			{OVERLAY_PIPE_ADE_CH1,1,0,0,0,0,0,1,0,0,0},
			/*通道2*/
			{OVERLAY_PIPE_ADE_CH2,1,0,0,0,0,0,0,1,0,0},
			/*通道3*/
			{OVERLAY_PIPE_ADE_CH3,1,0,0,0,0,0,1,0,0,0},
			/*通道4*/
			{OVERLAY_PIPE_ADE_CH4,1,0,0,0,0,0,0,0,1,0},
			/*通道5*/
			{OVERLAY_PIPE_ADE_CH5,1,0,1,0,0,1,0,1,0,0},
			/*通道6*/
			{OVERLAY_PIPE_ADE_CH6,1,1,0,0,1,0,1,0,0,0}
	},

	/*配置显示通道DISP_CH_RES资源*/
	{0,0,0,0,0,0,0,0,0,0,0},
	
	 /*配置叠加模块资源*/
	/*OVLY1_RES*/
	{1,1,1,0,0,0},
	/*OVLY2_RES*/
	{OVERLAY_OVLY_2,1,0,0,0,0,0,0,1,0,0},
	/*OVLY3_RES*/
	{OVERLAY_OVLY_3,1,0,0,0,1,0,0,0,0},
	
	/*ROT_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*SCL2_RES*/
	{1,0,0,0,1,0,0,0,0},

	/* ADE_MEM_SHARE_EN */
	0
},
/*pattern 51*/
/*z00196871 add for FPGA test  offline  Pattern1 on 2014-02-28*/
{
    /*    
    "(ch1->ovly1 + ch2->ovly1 + ch3->ovly1 +ch4->scl2->ovly1 + ch5->scl3->ovly1 + ch6(1080P rot)->scl1->ovly1)=>WDMA1=>DISP=>LDI"
    */
	/* 叠加摸式OVERLAY_COMP_TYPE_UINT32*/
	OVERLAY_COMP_TYPE_OFFLINE,
	
	/*配置叠加通道资源CH_RES*/
	{		
			/*通道1*/
			{OVERLAY_PIPE_ADE_CH1,1,0,0,0,0,0,1,0,0,0},
			/*通道2*/
			{OVERLAY_PIPE_ADE_CH2,1,0,0,0,0,0,1,0,0,0},
			/*通道3*/
			{OVERLAY_PIPE_ADE_CH3,1,0,0,0,0,0,1,0,0,0},
			/*通道4*/
			{OVERLAY_PIPE_ADE_CH4,1,0,0,0,0,0,0,0,1,0}, /*ovly3*/
			/*通道5*/
			{OVERLAY_PIPE_ADE_CH5,1,0,0,0,0,1,1,0,0,0},
			/*通道6*/
			{OVERLAY_PIPE_ADE_CH6,1,0,0,1,1,0,1,0,0,0},
	},

	/*配置显示通道DISP_CH_RES资源*/
	{1,1,0,0,0,0,0,0,0,0,0},
	
	 /*配置叠加模块资源*/
	/*OVLY1_RES*/
	{OVERLAY_OVLY_1,1,1,0,0,0},
	
	/*OVLY2_RES*/
	{0,0,0,0,0,0,0,0,0,0,0},
	/*OVLY3_RES*/
	//{OVERLAY_OVLY_3,1,0,0,0,1,0,0,0,0}, /*ovly3->scl2*/ ok
	{OVERLAY_OVLY_3,1,0,0,0,1,0,0,1,0}, /*ovly3->scl2*/
	
	/*ROT_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*SCL2_RES*/
	//{1,0,0,0,1,0,0,0,0} //ok
	{1,0,0,0,1,0,0,1,0},

	/* ADE_MEM_SHARE_EN */
	0
},
/*pattern low resolution<-------->52*/
{
	/* 叠加摸式OVERLAY_COMP_TYPE_UINT32*/
	OVERLAY_COMP_TYPE_ONLINE,
	
	/*配置叠加通道资源CH_RES*/
	{		
			/*通道1*/
			{OVERLAY_PIPE_ADE_CH1,1,0,0,0,0,0,0,1,0,0},
			/*通道2*/
			{OVERLAY_PIPE_ADE_CH2,1,0,0,0,0,0,0,0,1,0},
			/*通道3*/
			{OVERLAY_PIPE_ADE_CH3,1,0,0,0,0,0,0,0,1,0},
			/*通道4*/
			{OVERLAY_PIPE_ADE_CH4,1,0,0,0,0,0,0,0,1,0},
			/*通道5*/
			{OVERLAY_PIPE_ADE_CH5,1,0,1,0,0,1,0,1,0,0},
			/*通道6*/
			{OVERLAY_PIPE_ADE_CH6,1,0,0,0,1,0,0,1,0,0},
	},

	/*配置显示通道DISP_CH_RES资源*/
	{0,0,0,0,0,0,0,0,0,0,0},
	
	 /*配置叠加模块资源*/
	/*OVLY1_RES*/
	{0,0,0,0,0,0},
	/*OVLY2_RES*/
	{OVERLAY_OVLY_2,1,0,0,0,0,0,0,1,0,0},
	/*OVLY3_RES*/
	{OVERLAY_OVLY_3,1,0,0,0,1,0,0,0,0},
	//{OVERLAY_OVLY_3,1,0,0,0,0,0,1,0,0},
	
	/*ROT_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*SCL2_RES*/
	{1,0,0,0,0,1,0,0,0},
	//{0,0,0,0,0,0,0,0,0}
	
	/* ADE_MEM_SHARE_EN */
	0
},
/*pattern low resolution<-------->53*/
/*z00196871 add for FPGA test  Low Resolution  Pattern1 on 2014-03-10*/
{
	/* 叠加摸式OVERLAY_COMP_TYPE_UINT32*/
	OVERLAY_COMP_TYPE_ONLINE,
	
	/*配置叠加通道资源CH_RES*/
	{		
			/*通道1*/
			{OVERLAY_PIPE_ADE_CH1,1,0,0,0,0,0,0,0,1,0},
			/*通道2*/
			{OVERLAY_PIPE_ADE_CH2,1,0,0,0,0,0,0,0,1,0},
			/*通道3*/
			{OVERLAY_PIPE_ADE_CH3,1,0,0,0,0,0,0,0,1,0},
			/*通道4*/
			{OVERLAY_PIPE_ADE_CH4,1,0,0,0,0,0,0,0,1,0},
			/*通道5*/
			{OVERLAY_PIPE_ADE_CH5,1,0,0,0,0,1,0,1,0,0},
			/*通道6*/
			{OVERLAY_PIPE_ADE_CH6,1,0,0,1,1,0,0,1,0,0},
	},

	/*配置显示通道DISP_CH_RES资源*/
	{0,0,0,0,0,0,0,0,0,0,0},
	
	 /*配置叠加模块资源*/
	/*OVLY1_RES*/
	{0,0,0,0,0,0},
	/*OVLY2_RES*/
	{OVERLAY_OVLY_2,1,0,0,0,0,0,0,1,0,0},
	/*OVLY3_RES*/
	{OVERLAY_OVLY_3,1,0,0,0,1,0,0,0,0},
	//{OVERLAY_OVLY_3,1,0,0,0,0,0,1,0,0},
	
	/*ROT_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*SCL2_RES*/
	{1,0,0,0,0,1,0,0,0},
	//{0,0,0,0,0,0,0,0,0}
	
	/* ADE_MEM_SHARE_EN */
	0
},
/*pattern low resolution<-------->54*/
/*z00196871 add for FPGA test  Low Resolution  Pattern2 on 2014-03-10*/
{
	/* 叠加摸式OVERLAY_COMP_TYPE_UINT32*/
	OVERLAY_COMP_TYPE_ONLINE,
	
	/*配置叠加通道资源CH_RES*/
	{		
			/*通道1*/
			{OVERLAY_PIPE_ADE_CH1,1,0,0,0,0,0,0,0,1,0},
			/*通道2*/
			{OVERLAY_PIPE_ADE_CH2,1,0,0,0,0,0,0,0,1,0},
			/*通道3*/
			{OVERLAY_PIPE_ADE_CH3,1,0,0,0,0,0,0,0,1,0},
			/*通道4*/
			{OVERLAY_PIPE_ADE_CH4,1,0,0,0,0,0,0,0,1,0},
			/*通道5*/
			{OVERLAY_PIPE_ADE_CH5,1,0,1,0,0,1,0,1,0,0},
			/*通道6*/
			{OVERLAY_PIPE_ADE_CH6,1,0,1,0,1,0,0,1,0,0},
	},

	/*配置显示通道DISP_CH_RES资源*/
	{0,0,0,0,0,0,0,0,0,0,0},
	
	 /*配置叠加模块资源*/
	/*OVLY1_RES*/
	{0,0,0,0,0,0},
	/*OVLY2_RES*/
	{OVERLAY_OVLY_2,1,0,0,0,0,0,0,1,0,0},
	/*OVLY3_RES*/
	{OVERLAY_OVLY_3,1,0,0,0,1,0,0,0,0},
	//{OVERLAY_OVLY_3,1,0,0,0,0,0,1,0,0},
	
	/*ROT_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*SCL2_RES*/
	{1,0,0,0,0,1,0,0,0},
	//{0,0,0,0,0,0,0,0,0}
	
	/* ADE_MEM_SHARE_EN */
	0
},
/*pattern low resolution<-------->55*/
/*z00196871 add for FPGA test  Low Resolution  Pattern3 on 2014-03-10*/
{
	/* 叠加摸式OVERLAY_COMP_TYPE_UINT32*/
	OVERLAY_COMP_TYPE_ONLINE,
	
	/*配置叠加通道资源CH_RES*/
	{		
			/*通道1*/
			{OVERLAY_PIPE_ADE_CH1,1,0,0,0,0,0,0,0,1,0},
			/*通道2*/
			{OVERLAY_PIPE_ADE_CH2,1,0,0,0,0,0,0,0,1,0},
			/*通道3*/
			{OVERLAY_PIPE_ADE_CH3,1,0,0,0,0,0,0,0,1,0},
			/*通道4*/
			{OVERLAY_PIPE_ADE_CH4,1,0,0,0,0,0,0,0,1,0},
			/*通道5*/
			{OVERLAY_PIPE_ADE_CH5,1,0,0,0,0,1,0,1,0,0},
			/*通道6*/
			{OVERLAY_PIPE_ADE_CH6,1,0,1,0,1,0,0,1,0,0},
	},

	/*配置显示通道DISP_CH_RES资源*/
	{0,0,0,0,0,0,0,0,0,0,0},
	
	 /*配置叠加模块资源*/
	/*OVLY1_RES*/
	{0,0,0,0,0,0},
	/*OVLY2_RES*/
	{OVERLAY_OVLY_2,1,0,0,0,0,0,0,1,0,0},
	/*OVLY3_RES*/
	{OVERLAY_OVLY_3,1,0,0,0,1,0,0,0,0},
	//{OVERLAY_OVLY_3,1,0,0,0,0,0,1,0,0},
	
	/*ROT_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*SCL2_RES*/
	{1,0,0,0,0,1,0,0,0},
	//{0,0,0,0,0,0,0,0,0}
	
	/* ADE_MEM_SHARE_EN */
	0
},
/*pattern low resolution<-------->56*/
/*z00196871 add for FPGA test  Low Resolution  Pattern4 on 2014-03-10*/
{
	/* 叠加摸式OVERLAY_COMP_TYPE_UINT32*/
	OVERLAY_COMP_TYPE_ONLINE,
	
	/*配置叠加通道资源CH_RES*/
	{		
			/*通道1*/
			{OVERLAY_PIPE_ADE_CH1,1,0,0,0,0,0,0,0,1,0},
			/*通道2*/
			{OVERLAY_PIPE_ADE_CH2,1,0,0,0,0,0,0,0,1,0},
			/*通道3*/
			{OVERLAY_PIPE_ADE_CH3,1,0,0,0,0,0,0,0,1,0},
			/*通道4*/
			{OVERLAY_PIPE_ADE_CH4,1,0,0,0,0,0,0,0,1,0},
			/*通道5*/
			{OVERLAY_PIPE_ADE_CH5,1,0,1,0,0,0,0,0,1,0},
			/*通道6*/
			{OVERLAY_PIPE_ADE_CH6,1,0,1,0,1,0,0,1,0,0},
	},

	/*配置显示通道DISP_CH_RES资源*/
	{0,0,0,0,0,0,0,0,0,0,0},
	
	 /*配置叠加模块资源*/
	/*OVLY1_RES*/
	{0,0,0,0,0,0},
	/*OVLY2_RES*/
	{OVERLAY_OVLY_2,1,0,0,0,0,0,0,1,0,0},
	/*OVLY3_RES*/
	{OVERLAY_OVLY_3,1,0,0,0,1,0,0,0,0},
	//{OVERLAY_OVLY_3,1,0,0,0,0,0,1,0,0},
	
	/*ROT_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*SCL2_RES*/
	{1,0,0,0,0,1,0,0,0},
	//{0,0,0,0,0,0,0,0,0}
	
	/* ADE_MEM_SHARE_EN */
	0
},
/*pattern low resolution<-------->57*/
/*z00196871 add for FPGA test  Low Resolution  Pattern5 on 2014-03-10*/
{
	/* 叠加摸式OVERLAY_COMP_TYPE_UINT32*/
	OVERLAY_COMP_TYPE_ONLINE,
	
	/*配置叠加通道资源CH_RES*/
	{		
			/*通道1*/
			{OVERLAY_PIPE_ADE_CH1,1,0,0,0,0,0,0,0,1,0},
			/*通道2*/
			{OVERLAY_PIPE_ADE_CH2,1,0,0,0,0,0,0,0,1,0},
			/*通道3*/
			{OVERLAY_PIPE_ADE_CH3,1,0,0,0,0,0,0,0,1,0},
			/*通道4*/
			{OVERLAY_PIPE_ADE_CH4,1,0,0,0,0,0,0,0,1,0},
			/*通道5*/
			{OVERLAY_PIPE_ADE_CH5,1,0,0,0,0,0,0,0,1,0},
			/*通道6*/
			{OVERLAY_PIPE_ADE_CH6,1,0,1,0,1,0,0,1,0,0},
	},

	/*配置显示通道DISP_CH_RES资源*/
	{0,0,0,0,0,0,0,0,0,0,0},
	
	 /*配置叠加模块资源*/
	/*OVLY1_RES*/
	{0,0,0,0,0,0},
	/*OVLY2_RES*/
	{OVERLAY_OVLY_2,1,0,0,0,0,0,0,1,0,0},
	/*OVLY3_RES*/
	{OVERLAY_OVLY_3,1,0,0,0,1,0,0,0,0},
	//{OVERLAY_OVLY_3,1,0,0,0,0,0,1,0,0},
	
	/*ROT_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*SCL2_RES*/
	{1,0,0,0,0,1,0,0,0},
	//{0,0,0,0,0,0,0,0,0}
	
	/* ADE_MEM_SHARE_EN */
	0
},
/*pattern low resolution<-------->58*/
/*z00196871 add for FPGA test  Low Resolution  Pattern6 on 2014-03-10*/
{
	/* 叠加摸式OVERLAY_COMP_TYPE_UINT32*/
	OVERLAY_COMP_TYPE_ONLINE,
	
	/*配置叠加通道资源CH_RES*/
	{		
			/*通道1*/
			{OVERLAY_PIPE_ADE_CH1,1,0,0,0,0,0,0,0,1,0},
			/*通道2*/
			{OVERLAY_PIPE_ADE_CH2,1,0,0,0,0,0,0,0,1,0},
			/*通道3*/
			{OVERLAY_PIPE_ADE_CH3,1,0,0,0,0,0,0,0,1,0},
			/*通道4*/
			{OVERLAY_PIPE_ADE_CH4,1,0,0,0,0,0,0,0,1,0},
			/*通道5*/
			{OVERLAY_PIPE_ADE_CH5,1,0,0,0,0,1,0,0,1,0},
			/*通道6*/
			{OVERLAY_PIPE_ADE_CH6,1,0,0,1,1,0,0,0,1,0},
	},

	/*配置显示通道DISP_CH_RES资源*/
	{0,0,0,0,0,0,0,0,0,0,0},
	
	 /*配置叠加模块资源*/
	/*OVLY1_RES*/
	{0,0,0,0,0,0},
	/*OVLY2_RES*/
	{OVERLAY_OVLY_2,1,0,0,0,0,0,0,1,0,0},
	/*OVLY3_RES*/
	{OVERLAY_OVLY_3,1,0,0,0,1,0,0,0,0},
	//{OVERLAY_OVLY_3,1,0,0,0,0,0,1,0,0},
	
	/*ROT_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*SCL2_RES*/
	{1,0,0,0,0,1,0,0,0},
	//{0,0,0,0,0,0,0,0,0}
	
	/* ADE_MEM_SHARE_EN */
	0
},
/*pattern low resolution<-------->59*/
/*z00196871 add for FPGA test  Low Resolution  Pattern7 on 2014-03-10*/
{
	/* 叠加摸式OVERLAY_COMP_TYPE_UINT32*/
	OVERLAY_COMP_TYPE_ONLINE,
	
	/*配置叠加通道资源CH_RES*/
	{		
			/*通道1*/
			{OVERLAY_PIPE_ADE_CH1,1,0,0,0,0,0,0,0,1,0},
			/*通道2*/
			{OVERLAY_PIPE_ADE_CH2,1,0,0,0,0,0,0,0,1,0},
			/*通道3*/
			{OVERLAY_PIPE_ADE_CH3,1,0,0,0,0,0,0,0,1,0},
			/*通道4*/
			{OVERLAY_PIPE_ADE_CH4,1,0,0,0,0,0,0,0,1,0},
			/*通道5*/
			{OVERLAY_PIPE_ADE_CH5,1,0,1,0,0,1,0,0,1,0},
			/*通道6*/
			{OVERLAY_PIPE_ADE_CH6,1,0,1,0,1,0,0,0,1,0},
	},

	/*配置显示通道DISP_CH_RES资源*/
	{0,0,0,0,0,0,0,0,0,0,0},
	
	 /*配置叠加模块资源*/
	/*OVLY1_RES*/
	{0,0,0,0,0,0},
	/*OVLY2_RES*/
	{OVERLAY_OVLY_2,1,0,0,0,0,0,0,1,0,0},
	/*OVLY3_RES*/
	{OVERLAY_OVLY_3,1,0,0,0,1,0,0,0,0},
	//{OVERLAY_OVLY_3,1,0,0,0,0,0,1,0,0},
	
	/*ROT_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*SCL2_RES*/
	{1,0,0,0,0,1,0,0,0},
	//{0,0,0,0,0,0,0,0,0}
	
	/* ADE_MEM_SHARE_EN */
	0
},
/*pattern low resolution<-------->60*/
/*z00196871 add for FPGA test  Low Resolution  Pattern8 on 2014-03-10*/
{
	/* 叠加摸式OVERLAY_COMP_TYPE_UINT32*/
	OVERLAY_COMP_TYPE_ONLINE,
	
	/*配置叠加通道资源CH_RES*/
	{		
			/*通道1*/
			{OVERLAY_PIPE_ADE_CH1,1,0,0,0,0,0,0,0,1,0},
			/*通道2*/
			{OVERLAY_PIPE_ADE_CH2,1,0,0,0,0,0,0,0,1,0},
			/*通道3*/
			{OVERLAY_PIPE_ADE_CH3,1,0,0,0,0,0,0,0,1,0},
			/*通道4*/
			{OVERLAY_PIPE_ADE_CH4,1,0,0,0,0,0,0,0,1,0},
			/*通道5*/
			{OVERLAY_PIPE_ADE_CH5,1,0,0,0,0,1,0,0,1,0},
			/*通道6*/
			{OVERLAY_PIPE_ADE_CH6,1,0,1,0,1,0,0,0,1,0},
	},

	/*配置显示通道DISP_CH_RES资源*/
	{0,0,0,0,0,0,0,0,0,0,0},
	
	 /*配置叠加模块资源*/
	/*OVLY1_RES*/
	{0,0,0,0,0,0},
	/*OVLY2_RES*/
	{OVERLAY_OVLY_2,1,0,0,0,0,0,0,1,0,0},
	/*OVLY3_RES*/
	{OVERLAY_OVLY_3,1,0,0,0,1,0,0,0,0},
	//{OVERLAY_OVLY_3,1,0,0,0,0,0,1,0,0},
	
	/*ROT_RES*/
	{0,0,0,0,0,0,0,0,0,0},
	
	/*SCL2_RES*/
	{1,0,0,0,0,1,0,0,0},
	//{0,0,0,0,0,0,0,0,0}
	
	/* ADE_MEM_SHARE_EN */
	0
}
};

int overlay_get_rotation(int rot)
{
    switch (rot)
    {
        case 0:
            return ADE_ROT_NOP;
        case 90:
            return ADE_ROT_90;
        case 180:
            return ADE_ROT_180;
        case 270:
            return ADE_ROT_270;
        default:
            return ADE_ROT_NOP;
    }
}

int overlay_get_image_format(char *pic_filename, uint32_t* img_format)
{
    char  upper_filename[strlen(pic_filename) + 1];
    unsigned int   i = 0;

    for (i = 0; i < strlen(pic_filename); i++)
    {
        upper_filename[i] = toupper(pic_filename[i]);
    }

    upper_filename[i] = '\0';

    fprintf(stderr,"overlay_open_pic_file enter %s\n", pic_filename);
    fprintf(stderr,"pic_filename to upper :   %s\n", upper_filename);

    if (strstr(upper_filename,"ARGB1555"))
    {
        *img_format = ADE_RGB_565;
    }
    else if (strstr(upper_filename,"RGBA5551"))
    {
        *img_format = ADE_RGB_565;
    }
    else if ((strstr(upper_filename,"RGB565")))
    {
        *img_format  = ADE_RGB_565;
    }
    else if (strstr(upper_filename,"RGBX8888"))
    {
        *img_format  = ADE_XRGB_8888;
    }
    else if ((strstr(upper_filename,"XRGB8888")))
    {
        *img_format  = ADE_XRGB_8888;
    }
    else if ((strstr(upper_filename,"XBGR8888")))
    {
        *img_format  = ADE_XBGR_8888;
    }
    else if ((strstr(upper_filename,"RGBA8888")))
    {
        *img_format  = ADE_RGBA_8888;
    }
    else if ((strstr(upper_filename,"ARGB8888")))
    {
        *img_format  = ADE_ARGB_8888;
    }
    else if ((strstr(upper_filename,"BGRA8888")))
    {
        *img_format  = ADE_BGRA_8888;
    }
    else if ((strstr(upper_filename,"YCbCR422I")))
    {
        *img_format  = ADE_YUYV_I;
    }
    else if ((strstr(upper_filename,"CbYCrY422I")))
    {
        *img_format  = ADE_UYVY_I;
    }
    else if (strstr(upper_filename,"YCBYCR"))
    {
        *img_format  = ADE_YUYV_I;
    }
    else if (strstr(upper_filename,"CBYCRY"))
    {
        *img_format  = ADE_UYVY_I;
    }
    else if (strstr(upper_filename,"YUYV"))
    {
        *img_format  = ADE_YUYV_I;
    }
    else if (strstr(upper_filename,"UYVY"))
    {
        *img_format  = ADE_UYVY_I;
    }
	else if (strstr(upper_filename,"YVYU"))
    {
        *img_format  = ADE_YVYU_I;
    }
    else if (strstr(upper_filename,"VYUY"))
    {
        *img_format  = ADE_VYUY_I;
    } 
    else if (strstr(upper_filename,"NV12"))
    {
        *img_format  = ADE_NV12;
    } 
    else if (strstr(upper_filename,"YUV444"))
    {
        *img_format  = ADE_YUV444;
    } 
    else if (strstr(upper_filename,"NV21"))
    {
        *img_format  = ADE_NV21;
    }
    else if ((strstr(upper_filename,"RGB888")))
    {
        *img_format  = ADE_RGB_888;
    }
    else if ((strstr(upper_filename,"BGR888")))
    {
        *img_format  = ADE_BGR_888;
    }
    else
    {
        fprintf(stderr,"not support img format %s\n", pic_filename);

        return -1;
    }


    return 0;
}
int overlay_get_pixel_bpp(int format)
{
    switch (format)
    {
        case ADE_XRGB_8888:
	    case ADE_XBGR_8888:
	    case ADE_ARGB_8888:
	    case ADE_ABGR_8888:
	    case ADE_RGBA_8888:
	    case ADE_BGRA_8888:
            return 4;
        case ADE_RGB_888:
        case ADE_BGR_888:
		case ADE_YUV444:
            return 3;
        case ADE_RGB_565:
        case ADE_BGR_565:
        case ADE_YUYV_I:
        case ADE_YVYU_I:
        case ADE_UYVY_I:
        case ADE_VYUY_I:
        	return 2;
        case ADE_NV12:
        case ADE_NV21:
            return 1;
        default:
            return -1;
    }

    return -1;
}

int overlay_open_pic_file(char *pic_filename, int *picfilefd, struct OVERLAY_IMAGE_ST*  img)
{
    struct stat                         stat;

    /* 获取图片的格式 */
    if (overlay_get_image_format(pic_filename, &(img->format)) < 0)
    {
        fprintf(stderr,"overlay_get_image_format fail %s\n", pic_filename);

        return -1;
    }

    fprintf(stderr,"overlay_open_pic_file open %s\n", pic_filename);

    *picfilefd = open(pic_filename, O_RDONLY);

    if (*picfilefd < 0)
    {
        fprintf(stderr,"open pic file fail! \n");

        return -1;
    }

    if (fstat(*picfilefd, &stat) < 0)
    {
        fprintf(stderr,"fstat pic file fail! \n");

        goto err;
    }

    img->filelen = stat.st_size;

    fprintf(stderr,"overlay_open_pic_file overlay_get_pic_size %s enter!, fileSize: %d\n", pic_filename,img->filelen);

    /* 根据文件名获取图片的尺寸 */
    if (get_pic_size_by_filename(pic_filename,&img->width,&img->height) < 0)
    {
        fprintf(stderr,"get pic file size fail! \n");

        goto err;
    }

    fprintf(stderr,"open %s succ! img_format: %d, filesize: %d\n",pic_filename,img->format,img->filelen);

    return 0;

err:

    close(*picfilefd);

    return -1;
}

int overlay_ion_supp_smmu(int ion_fd)
{
    struct ion_custom_data cus_data;
    struct ion_issupport_iommu_data mem;

    cus_data.cmd = ION_HISI_CUSTOM_ISSUPPORT_IOMMU;
    cus_data.arg = (unsigned long)(&mem);
    if(0 != ioctl(ion_fd, ION_IOC_CUSTOM, &cus_data)) {
		mem.is_support_iommu = 0;
    }

	fprintf(stderr, "overlay_ion_supp_smmu mem.is_support_iommu=%d \n", mem.is_support_iommu);
	return mem.is_support_iommu;
}
int overlay_ion_free(int ion_client, struct ion_handle *ion_hnd)
{
    struct ion_handle_data handle_data;

    handle_data.handle = ion_hnd;
    if(0 != ioctl(ion_client, ION_IOC_FREE, &handle_data))
    {
        fprintf(stderr,"overlay_ion_free: ION_IOC_FREE\n");
        return -1;
    }

    return 0;
}

int overlay_get_ion_mem(int* psharefilefd, void** vir_addr, uint32_t* phy_addr, int len, uint32_t* hnd_id)
{        
    int                                 ion_fd;   
    struct ion_handle                   *ion_hnd;
    struct ion_allocation_data          alloc_data;
    struct ion_fd_data                  fd_data;
    struct ion_custom_data              cus_data;
    struct ion_phys_data                mem;

    if (g_ion_fd == 0) {
        /* open */
        ion_fd = open(ION_DEV_FILE_NAME, O_RDWR);

        if (ion_fd < 0)
        {
            fprintf(stderr,"can not open file %s\n",ION_DEV_FILE_NAME);
            return -1;
        }
    } else {
        ion_fd = g_ion_fd;
    }

    len = (len + IOMMU_PAGE_SIZE - 1) & ~(IOMMU_PAGE_SIZE - 1);
    if (overlay_ion_supp_smmu(ion_fd) > 0) {
        /*support sMMU */
        fprintf(stderr,"support smmu! \n");
        alloc_data.align = IOMMU_PAGE_SIZE;
        alloc_data.flags = HISI_ION_ALLOC_FLAG; 
        alloc_data.heap_mask = 1 << HISI_ION_HEAP_IOMMU_ID;
    } else {
        fprintf(stderr,"don't support smmu! \n");
        alloc_data.align = 0;
        alloc_data.flags = 0; 
        alloc_data.heap_mask = 1 << ION_GRALLOC_HEAP_ID;
    }

    /* alloc */
    alloc_data.len = len;    
    
    if(0 != ioctl(ion_fd, ION_IOC_ALLOC, &alloc_data))
    {
        fprintf(stderr,"ION_IOC_ALLOC  errno \n");
        
        /* to open */
        close(ion_fd);
        return -1;
    }
    
    ion_hnd = alloc_data.handle;

    /* shared */
    fd_data.handle = ion_hnd;
    if(0 != ioctl(ion_fd, ION_IOC_SHARE, &fd_data))
    {
        fprintf(stderr,"ION_IOC_SHARE error \n");
        goto err_ion_mmap;
    }

    *psharefilefd = fd_data.fd;
    *hnd_id = fd_data.handle;
    
    /* mmap */
    *vir_addr = mmap(0, len, PROT_READ| PROT_WRITE, MAP_SHARED, *psharefilefd, 0);    
    
    if (*vir_addr == MAP_FAILED) {
        goto err_ion_mmap;
    } 

    /* get phy addr */
    mem.fd_buffer = *psharefilefd;
    cus_data.cmd = ION_HISI_CUSTOM_PHYS;
    cus_data.arg = (unsigned long)(&mem);
    if (0 != ioctl(ion_fd, ION_IOC_CUSTOM, &cus_data))
    {
        fprintf(stderr, "ION_IOC_CUSTOM get phy addr errno\n");
        mem.phys_l = 0;
        goto err_ion_mmap;
    }

    *phy_addr = mem.phys_l;    
    fprintf(stderr,"overlay_get_ion_mem: vir_addr: 0x%x, len: %d\n",(int) *vir_addr,len);
    g_ion_fd = ion_fd;
    ion_vir_addr = *vir_addr;    
    g_ion_hnd = ion_hnd;

    fprintf(stderr,"overlay_get_ion_mem: ion_vir_addr: %p ,len: %d\n", ion_vir_addr,len);

    return 0;

err_ion_mmap:

    close(*psharefilefd);

    /* to mmap */
    munmap(*vir_addr,len);    

    /* to alloc */
    overlay_ion_free(ion_fd, ion_hnd);

    /* to open */
    close(ion_fd);
    
    return -1;
}

int overlay_get_display_data(char*  pic_filename, struct OVERLAY_IMAGE_ST*  img)
{
    int      rslt       = -1;
    int      picfilefd  = -1;
    int      pmemfilefd;
    int      byte = 0;
    void*    ion_vir_addr;
    uint32_t ion_phy_addr;
    int      file_len = 0;   
    int      tmp_len  = 0;
    uint32_t hnd_id;

    if ((NULL == pic_filename)
     || (NULL == img))
    {
        return -1;
    }

    if(img->phy_addr != NULL )
    {
    	return 0 ; 
    }
    
    fprintf(stderr,"overlay_get_display_data , pic_name: %s\n", pic_filename);

    /*获取格式和宽高*/
    rslt = overlay_open_pic_file(pic_filename, &picfilefd, img);
    if (rslt != 0)
    {
        fprintf(stderr,"cannot open %s\n", pic_filename);

        return -1;
    }

    /* 获取内存 */
    rslt = overlay_get_ion_mem(&pmemfilefd, &ion_vir_addr, &ion_phy_addr, img->filelen, &hnd_id);
    if (rslt != 0)
    {
        fprintf(stderr," overlay_get_ion_mem cannot open %s\n", ION_DEV_FILE_NAME);
        close(picfilefd);  

        return -1;
    }

    file_len = img->filelen;
    tmp_len = img->filelen;
    while (img->filelen)
    {
        byte = read(picfilefd,ion_vir_addr,img->filelen);
        if (byte < 0) {

            fprintf(stderr,"cannot read file %s,pmem_vir_addr: %p\n", pic_filename,ion_vir_addr);

            goto err;
        }

        img->filelen -= byte; 
        ion_vir_addr += byte;
        fprintf(stderr,"ion_vir_addr: %p\n",  ion_vir_addr);
        fprintf(stderr,"img->filelen %d,\n", img->filelen);        
    }  
    
    img->phy_addr = ion_phy_addr;
    img->filelen = tmp_len;
	close(picfilefd);  
    return 0;

err:
    /* close picture file  */
    close(picfilefd);
    close(pmemfilefd);

    munmap(ion_vir_addr, img->filelen);

    /* to alloc */
    overlay_ion_free(g_ion_fd, g_ion_hnd);

    /* to open */
    close(g_ion_fd);

    return -1;
}

int overlay_set_blk_rot(struct   st_ch_display_info  *ch_display_info,
    struct overlay_layer_info *ade_block_info, unsigned long pmem_phy_addr, int img_format, 
    int height, int width, int out_format, int wdma_number)
{
    int      strid_width;
    int      rslt = 0;
    void*                               ion_vir_addr;
    uint32_t                            ion_phy_addr;
    int                                 pmemfilefd = 0;
    uint32_t hnd_id;

    strid_width     = overlay_get_pixel_bpp(img_format);
    fprintf(stderr, "strid_width = %d, file len : %d\n", strid_width, ch_display_info->ov_image.filelen);

    ade_block_info->ch_num      = ch_display_info->ch;
    ade_block_info->surface_num = ch_display_info->overlay_num;
    ade_block_info->rotation    = ch_display_info->rotation;
    ade_block_info->dst_format  = out_format;
    ade_block_info->phy_addr    = pmem_phy_addr;
    ade_block_info->dst_phy_addr = g_overlay_dst.phy_addr;

    if (ch_display_info->has_block_rot == 1)
    {
        rslt = overlay_get_ion_mem(&pmemfilefd, &ion_vir_addr, &ion_phy_addr, height*width*overlay_get_pixel_bpp(img_format), &hnd_id);
        if (rslt != 0)
        {
            fprintf(stderr," overlay_get_ion_mem cannot open %s\n", ION_DEV_FILE_NAME);
            return -1;
        }
        
	    ade_block_info->dst_phy_addr = ion_phy_addr;
	    ch_display_info->block_rot_addr = ion_phy_addr;	 
    }	
    
    ade_block_info->stride = strid_width * width;
    ade_block_info->width  = width;
    ade_block_info->height = height;
    ade_block_info->format = img_format;
    ade_block_info->wdma_num = wdma_number;
        
    memcpy(&ade_block_info->src_rect, &ch_display_info->src_rect, sizeof(ade_block_info->src_rect));
    memcpy(&ade_block_info->dst_rect, &ch_display_info->dest_rect, sizeof(ade_block_info->dst_rect));    

    fprintf(stderr, " overlay_set_blk_rot phy_addr= 0x%x\n", pmem_phy_addr);
    fprintf(stderr, " overlay_set_blk_rot g_fb_fix_screen.smem_start = 0x%x\n", g_fb_fix_screen.smem_start);
    fprintf(stderr, " overlay_set_blk_rot ade_block_info->dst_phy_addr = 0x%x\n", ade_block_info->dst_phy_addr);

    return 0;
}
int overlay_setade_img(struct   st_ch_display_info  *ch_display_info,struct overlay_region_info *img, unsigned long pmem_phy_addr, int img_format, int height, int width)
{
    int                                 strid_width;

    strid_width     = overlay_get_pixel_bpp(img_format);
	fprintf(stderr, "xxxxxxxxxxxxxxxxstrid_width    = %d\n", strid_width);
	
    img->ch_num        = ch_display_info->ch;
    img->surface_num   = ch_display_info->overlay_num;
    img->blending      = g_ov_blending_mode;
    img->src_rect      = ch_display_info->src_rect;
    img->dst_rect      = ch_display_info->dest_rect;
    img->rotation      = ch_display_info->rotation;
    img->ch_type       = ch_display_info->ch_type;
    img->compose_mode  = ch_display_info->comp_mode;
    img->ovly_output_rect  = ch_display_info->ovly_output_rect;
    
    img->sec_ovly_output_rect.x = 0;
    img->sec_ovly_output_rect.y = 0;
    img->sec_ovly_output_rect.w = g_overlay_dst.width;
    img->sec_ovly_output_rect.h = g_overlay_dst.height;
    
    img->dst_width     = img->dst_rect.w;
    img->dst_height    = img->dst_rect.h;
    
    if (ch_display_info->has_sec_ovly == 1)
    { 
        img->sec_ovly_ch_count = sec_ovly_count;
        img->sec_ovly_surf_num = ch_display_info->overlay_num;
    }    
    else
    {
        img->sec_ovly_ch_count = 0;
        img->sec_ovly_surf_num = 0;
    }

    if (img->sec_ovly_ch_count > 1) {
        img->sec_ovly_surf_num = ovly2_ch_count;
    }
    
    img->stride        = width * strid_width; //ALIGN_DOWN(width, 8) * strid_width;
    img->format        = img_format;
    img->height        = height;
    img->width         = width;
    img->phy_addr      = pmem_phy_addr;
    if ((img_format == ADE_NV12) || (img_format == ADE_NV21)){
        img->uv_addr = img->phy_addr + img->stride *img->height + (img->src_rect.y * img->stride/2 + img->src_rect.x);
    }

    if (!ch_display_info->cfg_disable) {
        img->region_is_finished = 1;
    }

    fprintf(stderr, " overlay_setade_img img->region_is_finished = %d\n", img->region_is_finished);
    fprintf(stderr, " overlay_setade_img img->phy_addr = %x\n", img->phy_addr);
    fprintf(stderr, " overlay_setade_img ch_display_info->block_rot_addr = %p\n", ch_display_info->block_rot_addr);

    return 0;
}

int overlay_display_ade_img_blk_rot(struct st_ch_display_info  *ch_display_info, 
    struct OVERLAY_IMAGE_ST  ov_image, struct st_ch_display_info  *pan_ch_display_info, 
    int out_format, int wdma_number)
{
    int                       rslt;
    struct overlay_layer_info ade_block_info;
    
    memset(&ade_block_info, 0, sizeof(ade_block_info));
    memcpy(&(ch_display_info->ov_image), &ov_image, sizeof(struct OVERLAY_IMAGE_ST));

    /* 填写ade_img */
    rslt = overlay_set_blk_rot(ch_display_info,&ade_block_info, ov_image.phy_addr,
                               ov_image.format, ov_image.height, ov_image.width, out_format, wdma_number);

    /* 调用接口进行离线块旋转 */
	if (ioctl(g_ov_ade_fd, ADE_OVERLAY_LAYER_SET, &ade_block_info))
    {
        fprintf(stderr," ADE_OVERLAY_LAYER_SET fail\n");

        return -1;
    }
    
    return 0;
}


int overlay_display_ade_img(struct st_ch_display_info  *ch_display_info, struct OVERLAY_IMAGE_ST  ov_image, struct st_ch_display_info  *pan_ch_display_info)
{
    int                                   rslt;
    struct overlay_region_info            img;	
    struct ade_rect                       dst_rect;

    memset(&img, 0, sizeof(img));

    /* 填写ade_img */
    rslt = overlay_setade_img(ch_display_info,&img, ov_image.phy_addr,ov_image.format, ov_image.height, ov_image.width);

	if (ioctl(g_ov_ade_fd, ADE_OVERLAY_REGION_SET, &img))
    {
        fprintf(stderr," ADE_OVERLAY_REGION_SET fail\n");

        return -1;
    }
    return 0;
}

int fb_test_overlay_display_unit_blk_rot(struct   st_ch_display_info  *ch_display_info,
    struct   st_ch_display_info  *pan_ch_display_info, int out_format, int wdma_number)
{
    struct OVERLAY_IMAGE_ST             ov_image;
    int                                 rslt;

    if ((ch_display_info == NULL)) {
    	fprintf(stderr, "fb_test_overlay_display_unit_blk_rot ch_display_info is null \n");
    	return -1;
    }
    fprintf(stderr, "fb_test_overlay_display_unit_blk_rot  enter success \n");
    
    memset(&ov_image, 0, sizeof(ov_image));
	
    /* 获得需要显示的数据和参数 */
    if (overlay_get_display_data(ch_display_info->file_name,&ov_image) < 0) {
        fprintf(stderr, "fb_test_overlay_display: overlay_get_display_data fail \n");
        return -1;
    }

    fprintf(stderr, "fb_test_overlay_display_unit_blk_rot  ov_image.filelen = %d \n", ov_image.filelen);
    fprintf(stderr, "fb_test_overlay_display_unit_blk_rot  ov_image.phy_addr = %04x \n", ov_image.phy_addr);

    memcpy(&(ch_display_info->ov_image), &ov_image, sizeof(struct OVERLAY_IMAGE_ST));
	
    rslt = overlay_display_ade_img_blk_rot(ch_display_info, ov_image, pan_ch_display_info, out_format, wdma_number);

    if (rslt < 0)
    {
        fprintf(stderr,
               " overlay_display_ade_img_blk_rot  faile height:%d,width:%d\n",
                ov_image.height,
                ov_image.width);
        return -1;
    }    
    
    return 0;    
}


void overlay_display_set_offline_img(struct   st_ch_display_info  *ch_display_info, int offline_count, int file_count)
{
    int                                   rslt;
    struct overlay_region_info            img;

    memset(&img, 0, sizeof(img));
    ch_display_info->src_rect.x = 0;
    ch_display_info->src_rect.y = 0;
    ch_display_info->src_rect.w = g_overlay_dst.width;
    ch_display_info->src_rect.h = g_overlay_dst.height;
    memcpy(&ch_display_info->dest_rect, &ch_display_info->src_rect, sizeof(struct ade_rect));
    

    ch_display_info->cfg_disable = 1;
    if ((offline_count + 1) >= file_count) {
        ch_display_info->cfg_disable = 0;
    }    

    overlay_setade_img(ch_display_info, &img, g_overlay_dst.phy_addr, ADE_ARGB_8888, g_overlay_dst.height, g_overlay_dst.width);

    if (ioctl(g_ov_ade_fd, ADE_OVERLAY_REGION_SET, &img)) {
        fprintf(stderr," overlay_display_set_offline_img ADE_OVERLAY_REGION_SET fail\n");
        return;
    }
    return;
}


int fb_test_overlay_display_unit(struct   st_ch_display_info  *ch_display_info, struct   st_ch_display_info  *pan_ch_display_info)
{
    struct OVERLAY_IMAGE_ST             ov_image;
    int                                 rslt;
    
    if ((ch_display_info == NULL)) {
    	fprintf(stderr, "fb_test_overlay_display_unit ch_display_info is null \n");
    	return -1;
    }

    memset(&ov_image, 0, sizeof(ov_image));

    fprintf(stderr, "fb_test_overlay_display_unit: ch_display_info->has_block_rot = %d  \n",ch_display_info->has_block_rot);
	if (ch_display_info->has_block_rot == 1) {
        memcpy(&ov_image, &(ch_display_info->ov_image), sizeof( struct OVERLAY_IMAGE_ST));
        ov_image.phy_addr = ch_display_info->block_rot_addr;	

        ch_display_info->ion_fd = g_ion_fd;
        ch_display_info->ion_hnd = g_ion_hnd;
        ch_display_info->ion_vir_addr = ion_vir_addr;
        ch_display_info->file_len = ov_image.filelen;
        ch_display_info->rotation = 0;
        
        ch_display_info->src_rect.x = ch_display_info->dest_rect.x;
        ch_display_info->src_rect.y = ch_display_info->dest_rect.y;
        ch_display_info->src_rect.w = ch_display_info->dest_rect.w;
        ch_display_info->src_rect.h = ch_display_info->dest_rect.h;

        ov_image.width = ch_display_info->dest_rect.w;
        ov_image.height = ch_display_info->dest_rect.h;
        ov_image.format = g_blk_rot_dst_format;
	}

    /* 获得需要显示的数据和参数 */
    if (overlay_get_display_data(ch_display_info->file_name,&ov_image) < 0) {
        fprintf(stderr, "fb_test_overlay_display_unit: overlay_get_display_data fail \n");
        return -1;
    }
    
    fprintf(stderr, "fb_test_overlay_display_unit: ov_image.width : 0x%04x  \n",ov_image.width);
    fprintf(stderr, "fb_test_overlay_display_unit: ov_image.height : 0x%04x  \n",ov_image.height);
    fprintf(stderr, "fb_test_overlay_display_unit: ov_image.phy_addr : 0x%04x  \n",ov_image.phy_addr);
    fprintf(stderr, "fb_test_overlay_display_unit: ov_image.filelen : %d \n",ov_image.filelen);
    fprintf(stderr, "fb_test_overlay_display_unit: ov_image.format : %ud \n",ov_image.format);
    fprintf(stderr, "fb_test_overlay_display_unit: ch_display_info->ion_vir_addr: 0x%04x \n",ch_display_info->ion_vir_addr);

    rslt = overlay_display_ade_img(ch_display_info, ov_image, pan_ch_display_info);
    if (rslt < 0)
    {
        fprintf(stderr,
               " overlay_display_ade_img  faile height:%d,width:%d\n",
                ov_image.height,
                ov_image.width);
        return -1;
    }
 
    return 0;
    
}

void overlay_display_commit_offline_comp(int comp_mode, int pattern_index)
{
    struct overlay_compose_info ov_comp;
    struct ade_rect ovly_src_rect;
    int sharedfileid;
    
    memset(&ov_comp, 0, sizeof(ov_comp));    
	
	/*填充overlay_resource_info结构体*/	
	ov_comp.compose_pattern_num = pattern_index;
	ov_comp.compose_mode = comp_mode;
	ov_comp.dst_rect.x = 0;
	ov_comp.dst_rect.y = 0;
	ov_comp.dst_rect.w = g_overlay_dst.width;
	ov_comp.dst_rect.h = g_overlay_dst.height;
	
	ovly_src_rect = ov_comp.dst_rect;    
    ov_comp.src_rect.x = ovly_src_rect.x;
	ov_comp.src_rect.y = ovly_src_rect.y;
	ov_comp.src_rect.w = ovly_src_rect.w;
	ov_comp.src_rect.h = ovly_src_rect.h;
	
	ov_comp.dst_rotation = 0;
	ov_comp.rot_width = g_overlay_dst.width;
	ov_comp.rot_height= g_overlay_dst.height;

	ov_comp.offline_phy_addr = g_overlay_dst.phy_addr;
	ov_comp.offline_frame_phy_addr = g_overlay_dst.phy_addr;
	ov_comp.offline_dst_format = ADE_ARGB_8888;
	ov_comp.offline_dst_rect.x = 0;
	ov_comp.offline_dst_rect.y = 0;
	ov_comp.offline_dst_rect.w = g_overlay_dst.width;
	ov_comp.offline_dst_rect.h = g_overlay_dst.height;
	ov_comp.offline_stride     = g_overlay_dst.stride;
	ov_comp.is_finished = 1;  //finish

	fprintf(stderr," overlay commit success !\n");
	fprintf(stderr," ov_comp->offline_frame_phy_addr= 0x%x \n", ov_comp.offline_frame_phy_addr);

	ov_comp.wifi_phy_addr = 0;//g_fb_fix_screen.smem_start + g_fb_var_screen.xres * g_fb_var_screen.yres * 4 * 2;
	ov_comp.wifi_dst_format = ADE_ARGB_8888;
	ov_comp.wifi_dst_rect.x = 0;
	ov_comp.wifi_dst_rect.y = 0;
	ov_comp.wifi_dst_rect.w = g_overlay_dst.width;
	ov_comp.wifi_dst_rect.h = g_overlay_dst.height;
	ov_comp.wifi_stride     = g_overlay_dst.stride;

	ov_comp.wifi_src_rect.x = ov_comp.src_rect.x;
	ov_comp.wifi_src_rect.y = ov_comp.src_rect.y;
	ov_comp.wifi_src_rect.w = ov_comp.src_rect.w;
	ov_comp.wifi_src_rect.h = ov_comp.src_rect.h;
	
	if (ioctl(g_ov_ade_fd, ADE_OVERLAY_COMMIT, &ov_comp))
    {
        fprintf(stderr," ADE_OVERLAY_RES_SET fail\n");
        return;
    }
}

void overlay_display_commit_comp(struct   overlay_resource_info ov_res, int index, int rot, char* ovly_rect) 
{
    struct overlay_compose_info ov_comp;
    struct ade_rect ovly_src_rect;
    int sharedfileid;
    
    memset(&ov_comp, 0, sizeof(ov_comp));    
	
	/*填充overlay_resource_info结构体*/	
	ov_comp.compose_pattern_num = index;
	ov_comp.compose_mode = ov_res.comp_mode;
	ov_comp.dst_rect.x = 0;
	ov_comp.dst_rect.y = 0;
	ov_comp.dst_rect.w = g_overlay_dst.width;
	ov_comp.dst_rect.h = g_overlay_dst.height;

	if (ovly_rect == NULL) {
        ovly_src_rect = ov_comp.dst_rect;
	} else {
	    get_rect_by_string(&ovly_src_rect, 1, ovly_rect);
    }

    ov_comp.src_rect.x = ovly_src_rect.x;
	ov_comp.src_rect.y = ovly_src_rect.y;
	ov_comp.src_rect.w = ovly_src_rect.w;
	ov_comp.src_rect.h = ovly_src_rect.h;
	
	ov_comp.dst_rotation = overlay_get_rotation(rot);
	ov_comp.rot_width = g_overlay_dst.width;
	ov_comp.rot_height= g_overlay_dst.height;

	memcpy(&ov_comp.scl2_src_rect, &ov_comp.src_rect, sizeof(struct ade_rect));
	memcpy(&ov_comp.scl2_dst_rect, &ov_comp.dst_rect, sizeof(struct ade_rect));

	ov_comp.offline_phy_addr = g_overlay_dst.phy_addr;
	ov_comp.offline_frame_phy_addr = g_overlay_dst.phy_addr;
	ov_comp.offline_dst_format = ADE_ARGB_8888;
	ov_comp.offline_dst_rect.x = 0;
	ov_comp.offline_dst_rect.y = 0;
	ov_comp.offline_dst_rect.w = g_overlay_dst.width;
	ov_comp.offline_dst_rect.h = g_overlay_dst.height;
	ov_comp.offline_stride     = g_overlay_dst.stride;


	fprintf(stderr," ov_res->comp_mode= %d \n", ov_res.comp_mode);
	
	if ((OVERLAY_COMP_TYPE_HYBRID == ov_res.comp_mode)
	 || (OVERLAY_COMP_TYPE_ONLINE == ov_res.comp_mode)) {
        ov_comp.offline_phy_addr = 0;
        ov_comp.offline_frame_phy_addr = 0;
    	ov_comp.offline_dst_format = 0;
    	ov_comp.offline_dst_rect.x = 0;
    	ov_comp.offline_dst_rect.y = 0;
    	ov_comp.offline_dst_rect.w = 0;
    	ov_comp.offline_dst_rect.h = 0;
    	ov_comp.offline_stride     = 0;
	}

	fprintf(stderr," ov_comp->offline_frame_phy_addr= 0x%x \n", ov_comp.offline_frame_phy_addr);
	
	ov_comp.is_finished = 1;

	/*if (0 != overlay_get_ion_mem(&sharedfileid, &g_ion_wifi_vaddr, &g_ion_wifi_paddr, g_fb_var_screen.xres * g_fb_var_screen.yres * 4)) {
        fprintf(stderr," overlay_get_ion_mem get offline addr fail\n");
        return -1;
	}*/

	fprintf(stderr," overlay commit success !\n");

	ov_comp.wifi_phy_addr = 0;//g_fb_fix_screen.smem_start + g_fb_var_screen.xres * g_fb_var_screen.yres * 4 * 2;
	fprintf(stderr," ----------wifi_phy_addr = 0x%08x !\n", ov_comp.wifi_phy_addr);
	ov_comp.wifi_dst_format = ADE_ARGB_8888;
	ov_comp.wifi_dst_rect.x = 0;
	ov_comp.wifi_dst_rect.y = 0;
	ov_comp.wifi_dst_rect.w = g_overlay_dst.width;
	ov_comp.wifi_dst_rect.h = g_overlay_dst.height;
	ov_comp.wifi_stride     = g_overlay_dst.stride;
	
	ov_comp.wifi_src_rect.x = ov_comp.src_rect.x;
	ov_comp.wifi_src_rect.y = ov_comp.src_rect.y;
	ov_comp.wifi_src_rect.w = ov_comp.src_rect.w;
	ov_comp.wifi_src_rect.h = ov_comp.src_rect.h;
	
	if (ioctl(g_ov_ade_fd, ADE_OVERLAY_COMMIT, &ov_comp))
    {
        fprintf(stderr," ADE_OVERLAY_RES_SET fail\n");
        return;
    }
}

int commandfile_test_blk_rot(char* patternIndex, char* pic_name1, char* pic_name2, char* pic_name3, char* pic_name4, char* pic_name5, char* pic_name6,char* ch_list, 
            char * ovly_map, char* dst_rect, char* rotation, char* out_format, char* wdma_num)

{
    char    str_ch_list[STR_MAX_LEN]= {0};
    char    str_ovly_list[STR_MAX_LEN]= "0:0:0:0:0:0";
    char    str_dst_list[STR_MAX_LEN]= "0:0:720:1280";
    char    str_rot_list[STR_MAX_LEN]= {0};
    int	    i, j = 0;
    int     offline_count = 0;
    int     online_count = 0;
    int     block_count = 0;
    int     index;
    int	    file_num = 0;
    int     comp_rotation = 0;
    char 	*filename[ADE_CH_NUM_MAX] = {pic_name1,pic_name2,pic_name3,pic_name4,pic_name5,pic_name6};
    struct  st_ch_display_info  ch_display_info[ADE_CH_NUM_MAX];
    struct  overlay_resource_info ov_res;
    char*   ovly_rect = "0:0:720:1280";
    int     oFormat;
    int     wdma_number;

	if ((strlen(ch_list) >= STR_MAX_LEN) || (strlen(ovly_map) >= STR_MAX_LEN) 
	 || (strlen(dst_rect) >= STR_MAX_LEN) || (strlen(rotation) >= STR_MAX_LEN)) {
	    fprintf(stderr,"commandfile_test_overlay_display strlen(ch_list)= %d\n", strlen(ch_list));
        return -1;
	}
	
    fprintf(stderr,"ch_list %s\n", ch_list);
    memcpy(str_ch_list, ch_list, strlen(ch_list));
    memcpy(str_ovly_list, ovly_map, strlen(ovly_map));
    memcpy(str_dst_list, dst_rect, strlen(dst_rect));
    memcpy(str_rot_list, rotation, strlen(rotation));

    fprintf(stderr,"str_ch_list1 %s\n", str_ch_list);
    fprintf(stderr,"str_ch_list1_1: %s\n", str_ch_list);
    
    for (i = 0; i < ADE_CH_NUM_MAX; i++) 
    {
        memset(&(ch_display_info[i]), 0, sizeof(struct st_ch_display_info));
        fprintf(stderr,"str_ch_memset_list1_%d: %s\n", i, str_ch_list);
    }
    
    /* 打开fb设备 */
	g_ov_fb_fd = fb_test_open_fb();
	if (g_ov_fb_fd < 0) {
    	fprintf(stderr,"commandfile_test_blk_rot fb_test_open_fb open fb fail\n");
        return -1;
	}

    /* 打开ADE设备 */
    g_ov_ade_fd = fb_test_open_ade();
    if (g_ov_ade_fd < 0) {
		fprintf(stderr,"commandfile_test_blk_rot fb_test_open_ade open ade fail\n");
	    return -1;
	}
	
    fprintf(stderr,"str_ch_list2 %s\n", str_ch_list);
	
    memset(&ov_res, 0, sizeof(ov_res));
	
    /* 填充overlay_resource_info结构体 */	
    index = atoi(patternIndex);
    memcpy(&ov_res, &pattern[index], sizeof(ov_res));
    pg_ov_res = &pattern[index]; 

    if (ioctl(g_ov_ade_fd, ADE_OVERLAY_RES_SET, &ov_res))
    {
        fprintf(stderr," ADE_OVERLAY_RES_SET fail\n");

        return -1;
    }

    /* 获取有效的图片数据 */
    get_ch_compose_info(ch_display_info, &file_num, filename, str_ch_list, str_ovly_list, str_rot_list, str_dst_list, &ov_res, ovly_rect, &offline_count, &block_count);

    online_count = file_num - offline_count;

    fprintf(stderr,"str_ovly_list : %s\n", str_ovly_list);
    fprintf(stderr,"str_rot_list :  %s\n", str_rot_list);

    fprintf(stderr,"out_format : %s\n", out_format);
    fprintf(stderr,"wdma_num :  %s\n", wdma_num);

    oFormat = atoi(out_format);
    g_blk_rot_dst_format = oFormat;
    wdma_number = atoi(wdma_num);

    fprintf(stderr,"oFormat %d\n", oFormat);
    fprintf(stderr,"wdma_number %d\n", wdma_number);
    fprintf(stderr,"offline_count %d, online_count %d, block_count = %d\n", offline_count, online_count, block_count);

    //block rot
    fprintf(stderr,"----------commandfile_test_blk_rot, block rot \n");
    for (j = 0; j < block_count; j++) {        
        fprintf(stderr,"block rot j= %d\n", j);
        
        if (fb_test_overlay_display_unit_blk_rot(&(ch_display_info[j]), NULL, oFormat, wdma_number) < 0) {
            fprintf(stderr," fb_test_overlay_display_unit_blk_rot fail\n");
            fb_test_close_fb();
            fb_test_close_ade();

            return -1;
        }
    }

    /* 依次显示offline各个图层 */
    fprintf(stderr,"----------commandfile_test_blk_rot, offline\n");
    for (j = 0; j < offline_count; j++) {
        if (j == offline_count - 1) {
            ch_display_info[j].cfg_disable = 0;
        }
        
        fprintf(stderr,"offline j= %d\n", j);
        
        if (fb_test_overlay_display_unit(&(ch_display_info[j]), NULL) < 0) {
            fb_test_close_fb();
            fb_test_close_ade();
            return -1;
        }
    }
   
    //online begin
    fprintf(stderr," ----------commandfile_test_blk_rot:  online \n");
    j = offline_count;
    if (OVERLAY_COMP_TYPE_HYBRID == pg_ov_res->comp_mode) {
        /* commit offline with finishing */
        overlay_display_commit_offline_comp(OVERLAY_COMP_TYPE_HYBRID, index);

        /* region set the after offline compose layer */        
        overlay_display_set_offline_img(&ch_display_info[j], j, file_num);
        j = offline_count + 1;
    }

	for (; j < file_num; j++) {
			if (j == file_num - 1) {
					ch_display_info[j].cfg_disable = 0;
			}

			fprintf(stderr,"online j= %d\n", j);
			if (fb_test_overlay_display_unit(&(ch_display_info[j]), NULL) < 0) {
					fb_test_close_fb();
					fb_test_close_ade();

					return -1;
			}
	}

    //commit
    overlay_display_commit_comp(ov_res, index, comp_rotation, ovly_rect);

  	sleep(5);

    fb_test_close_fb();
    fb_test_close_ade();
    
  	for (i = 0; i < file_num; i++) {
        munmap(ch_display_info[j].ion_vir_addr, ch_display_info[j].file_len);

        /* to alloc */
        overlay_ion_free(ch_display_info[j].ion_fd, ch_display_info[j].ion_hnd);

        /* to open */
        close(ch_display_info[j].ion_fd);
  	}

  	return 0;
}
int commandfile_test_overlay_display(char* patternIndex, char* pic1_name, char* pic2_name, char* pic3_name,
								  char* pic4_name, char* pic5_name, char* pic6_name,char* ch_list, char* ovly_map,
								  char* dst_rect, char* rotation, char* comp_rot, char* ovly_rect)
{
    char     str_ch_list[STR_MAX_LEN]= {0};
	char     str_ovly_list[STR_MAX_LEN]= {0};
	char     str_dst_list[STR_MAX_LEN]= {0};
	char     str_rot_list[STR_MAX_LEN]= {0};
	int		 i, j = 0;
	int      offline_count = 0;
	int      online_count  = 0;
	int      block_count = 0;
	int      index;
   	int		 file_num = 0;
    char 	*filename[ADE_CH_NUM_MAX] = {pic1_name,pic2_name,pic3_name,pic4_name,pic5_name,pic6_name};
	struct   st_ch_display_info  ch_display_info[ADE_CH_NUM_MAX];
	struct   overlay_resource_info ov_res;
	int      comp_rotation;

	if ((strlen(ch_list) >= STR_MAX_LEN) || (strlen(ovly_map) >= STR_MAX_LEN) 
	 || (strlen(dst_rect) >= STR_MAX_LEN) || (strlen(rotation) >= STR_MAX_LEN)) {
        return -1;
	}

    fprintf(stderr,"ch_list %s\n", ch_list);

    memcpy(str_ch_list, ch_list, strlen(ch_list));
    memcpy(str_ovly_list, ovly_map, strlen(ovly_map));
    memcpy(str_dst_list, dst_rect, strlen(dst_rect));
    memcpy(str_rot_list, rotation, strlen(rotation));

	/* 打开fb设备 */
	g_ov_fb_fd = fb_test_open_fb();

	if (g_ov_fb_fd < 0) {
    	fprintf(stderr,"fb_test_overlay_display fb_test_open_fb open fb fail\n");
        return -1;
	}

    /* 打开ADE设备 */
    g_ov_ade_fd = fb_test_open_ade();

    if (g_ov_ade_fd < 0) {
		fprintf(stderr,"fb_test_overlay_display fb_test_open_ade open ade fail\n");
	    return -1;
	}

	for (i = 0; i < ADE_CH_NUM_MAX; i++) 
	{
        memset(&(ch_display_info[i]), 0, sizeof(struct   st_ch_display_info));
    }
	memset(&ov_res, 0, sizeof(ov_res));

	fprintf(stderr,"str_ch_list: %s\n", str_ch_list);
    /* 填充overlay_resource_info结构体 */	
    index = atoi(patternIndex);
	memcpy(&ov_res, &pattern[index], sizeof(ov_res));
	pg_ov_res = &pattern[index]; 

	if (ioctl(g_ov_ade_fd, ADE_OVERLAY_RES_SET, &ov_res))
    {
        fprintf(stderr," ADE_OVERLAY_RES_SET fail\n");

        return -1;
    }

    /* 获取有效的图片数据 */
	get_ch_compose_info(ch_display_info,
						&file_num,
						filename,
						str_ch_list,
						str_ovly_list,
						str_rot_list,
						str_dst_list,
						&ov_res,
						ovly_rect,
						&offline_count,
						&block_count);

    online_count = file_num - offline_count;  //2=2-0;
    
    fprintf(stderr,"offline_count %d\n", offline_count);
    fprintf(stderr,"online_count %d\n", online_count);
  
    /* 依次显示offline各个图层 */
    for (j = 0; j < offline_count; j++) {
        if (j == offline_count - 1) {
            ch_display_info[j].cfg_disable = 0;
        }
        
        fprintf(stderr,"offline j= %d\n", j);
        
        if (fb_test_overlay_display_unit(&(ch_display_info[j]), NULL) < 0) {
            fb_test_close_fb();
            fb_test_close_ade();
            return -1;
        }
    }
    
  
    j = offline_count;
    if (OVERLAY_COMP_TYPE_HYBRID == pg_ov_res->comp_mode) {
        /* commit offline with finishing */
        overlay_display_commit_offline_comp(OVERLAY_COMP_TYPE_HYBRID, index);

        /* region set the after offline compose layer */        
        overlay_display_set_offline_img(&ch_display_info[j], j, file_num);
        j = offline_count + 1;
    }
    
	/* 依次显示online各个图层 */
	for (; j < file_num; j++) {
		if (j == file_num - 1) {
			ch_display_info[j].cfg_disable = 0;
		}

		if (fb_test_overlay_display_unit(&(ch_display_info[j]), NULL) < 0) {
			fb_test_close_fb();
			fb_test_close_ade();
			
			return -1;
		}
	}

    /* commit */
    comp_rotation = (comp_rot == NULL) ? 0 : atoi(comp_rot);
    overlay_display_commit_comp(ov_res, index, comp_rotation, ovly_rect);
	
  	sleep(5);

    fb_test_close_fb();
    fb_test_close_ade();
    
  	for (i = 0; i < file_num; i++) {
  	    if (0 == ch_display_info[j].ion_hnd) {
            continue;
  	    }
  	    
        munmap(ch_display_info[j].ion_vir_addr, ch_display_info[j].file_len);

        /* to alloc */
        overlay_ion_free(ch_display_info[j].ion_fd, ch_display_info[j].ion_hnd);        
  	}

    /* to open */
    close(ch_display_info[j].ion_fd);
  	return 0;
}

int comfile_test_overlay_memshare(char* patternIndex, char* pic1_name, char* pic2_name, char* pic3_name,
								  char* pic4_name, char* pic5_name, char* pic6_name,char* ch_list, char* ovly_map,
								  char* dst_rect, char* rotation, char* comp_rot, char* ovly_rect)
{
    char     str_ch_list[STR_MAX_LEN]= {0};
	char     str_ovly_list[STR_MAX_LEN]= {0};
	char     str_dst_list[STR_MAX_LEN]= {0};
	char     str_rot_list[STR_MAX_LEN]= {0};
	int		 i, j = 0;
	int      offline_count = 0;
	int      online_count  = 0;
	int      block_count = 0;
	int      index;
   	int		 file_num = 0;
    char 	*filename[ADE_CH_NUM_MAX] = {pic1_name,pic2_name,pic3_name,pic4_name,pic5_name,pic6_name};
	struct   st_ch_display_info  ch_display_info[ADE_CH_NUM_MAX];
	struct   overlay_resource_info ov_res;
	int      comp_rotation;

	if ((strlen(ch_list) >= STR_MAX_LEN) || (strlen(ovly_map) >= STR_MAX_LEN) 
	 || (strlen(dst_rect) >= STR_MAX_LEN) || (strlen(rotation) >= STR_MAX_LEN)) {
        return -1;
	}

    fprintf(stderr,"ch_list %s\n", ch_list);

    memcpy(str_ch_list, ch_list, strlen(ch_list));
    memcpy(str_ovly_list, ovly_map, strlen(ovly_map));
    memcpy(str_dst_list, dst_rect, strlen(dst_rect));
    memcpy(str_rot_list, rotation, strlen(rotation));

	/* 打开fb设备 */
	g_ov_fb_fd = fb_test_open_fb();

	if (g_ov_fb_fd < 0) {
    	fprintf(stderr,"fb_test_overlay_display fb_test_open_fb open fb fail\n");
        return -1;
	}

    /* 打开ADE设备 */
    g_ov_ade_fd = fb_test_open_ade();

    if (g_ov_ade_fd < 0) {
		fprintf(stderr,"fb_test_overlay_display fb_test_open_ade open ade fail\n");
	    return -1;
	}

	for (i = 0; i < ADE_CH_NUM_MAX; i++) 
	{
        memset(&(ch_display_info[i]), 0, sizeof(struct   st_ch_display_info));
    }
	memset(&ov_res, 0, sizeof(ov_res));

	fprintf(stderr,"str_ch_list: %s\n", str_ch_list);
    /* 填充overlay_resource_info结构体 */	
    index = atoi(patternIndex);

	/* enable memory share en*/
	pattern[index].ade_reg_res.bits.memshare_en = 1;

	memcpy(&ov_res, &pattern[index], sizeof(ov_res));
	pg_ov_res = &pattern[index]; 

	if (ioctl(g_ov_ade_fd, ADE_OVERLAY_RES_SET, &ov_res))
    {
        fprintf(stderr," ADE_OVERLAY_RES_SET fail\n");

        return -1;
    }

    /* 获取有效的图片数据 */
	get_ch_compose_info(ch_display_info,
						&file_num,
						filename,
						str_ch_list,
						str_ovly_list,
						str_rot_list,
						str_dst_list,
						&ov_res,
						ovly_rect,
						&offline_count,
						&block_count);

    online_count = file_num - offline_count;
    
    fprintf(stderr,"offline_count %d\n", offline_count);
    fprintf(stderr,"online_count %d\n", online_count);
  
    /* 依次显示offline各个图层 */
    for (j = 0; j < offline_count; j++) {
        if (j == offline_count - 1) {
            ch_display_info[j].cfg_disable = 0;
        }
        
        fprintf(stderr,"offline j= %d\n", j);
        
        if (fb_test_overlay_display_unit(&(ch_display_info[j]), NULL) < 0) {
            fb_test_close_fb();
            fb_test_close_ade();
            return -1;
        }
    }
    
  
    j = offline_count;
    if (OVERLAY_COMP_TYPE_HYBRID == pg_ov_res->comp_mode) {
        /* commit offline with finishing */
        overlay_display_commit_offline_comp(OVERLAY_COMP_TYPE_HYBRID, index);

        /* region set the after offline compose layer */        
        overlay_display_set_offline_img(&ch_display_info[j], j, file_num);
        j = offline_count + 1;
    }
    
	/* 依次显示online各个图层 */
	for (; j < file_num; j++) {
		if (j == file_num - 1) {
			ch_display_info[j].cfg_disable = 0;
		}

		if (fb_test_overlay_display_unit(&(ch_display_info[j]), NULL) < 0) {
			fb_test_close_fb();
			fb_test_close_ade();
			
			return -1;
		}
	}

    /* commit */
    comp_rotation = (comp_rot == NULL) ? 0 : atoi(comp_rot);
    overlay_display_commit_comp(ov_res, index, comp_rotation, ovly_rect);
	
  	sleep(5);

    fb_test_close_fb();
    fb_test_close_ade();
    
  	for (i = 0; i < file_num; i++) {
  	    if (0 == ch_display_info[j].ion_hnd) {
            continue;
  	    }
  	    
        munmap(ch_display_info[j].ion_vir_addr, ch_display_info[j].file_len);

        /* to alloc */
        overlay_ion_free(ch_display_info[j].ion_fd, ch_display_info[j].ion_hnd);        
  	}

    /* to open */
    close(ch_display_info[j].ion_fd);
  	return 0;
}

#ifdef __cplusplus
    #if __cplusplus
   }
    #endif
#endif

