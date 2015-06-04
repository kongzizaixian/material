/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : v8r1_fb_test.c
  版 本 号   : 初稿
  作    者   : gongyu
  生成日期   : 2012年6月29日
  最近修改   :
  功能描述   : framebuffer driver测试程序
  函数列表   :
  修改历史   :
  1.日    期   : 2012年6月29日
    作    者   : gongyu
    修改内容   : 创建文件
  2.日    期   : 2013年09月02日
    作    者   : z00196871
    修改内容   : 修改
******************************************************************************/
/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include  "v8r1_commandfile_test.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

extern int fb_test_open_fb(void);
extern int fb_test_close_fb(void);

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/

int    g_pan_fb_fd;
char  *g_pan_fbbuf;

struct fb_var_screeninfo g_pan_fb_var_screen;

struct fb_fix_screeninfo g_pan_fb_fix_screen;

char* filenames[2]= {"red_320x480_ARGB8888.bin","blue_320x480_ARGB8888.bin"};

int bpps[2] = {32,32};

int files = 2;

int flags[2] = {0,0};
char const*const LCD_FILE2 = "/sys/class/leds/lcd_backlight0/brightness";



/*****************************************************************************
 函 数 名  : pan_set_framebuffer_resolution
 功能描述  : 设置分辨率
 输入参数  : unsigned n
 输出参数  : 无
 返 回 值  : static void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年6月29日
    作    者   : gongyu
    修改内容   : 新生成函数

*****************************************************************************/
// static void pan_set_framebuffer_resolution()
// {    
    // g_pan_fb_var_screen.yres = 640;//640;
    // g_pan_fb_var_screen.xres = 480;

    // g_pan_fb_var_screen.reserved[3]     = 104;    
    // g_pan_fb_var_screen.pixclock        = 25715 * 1000;
    // g_pan_fb_var_screen.vmode           = FB_VMODE_NONINTERLACED;
    // g_pan_fb_var_screen.right_margin    = 16;
    // g_pan_fb_var_screen.hsync_len       = 96;
    // g_pan_fb_var_screen.left_margin     = 48;

    // g_pan_fb_var_screen.lower_margin    = 10;
    // g_pan_fb_var_screen.vsync_len       = 2;
    // g_pan_fb_var_screen.upper_margin    = 33;

	 // fprintf(stderr," g_pan_fb_fd2:%c\n",g_pan_fb_fd);
    // if(ioctl(g_pan_fb_fd, K3FB_TIMING_SET, &g_pan_fb_var_screen) < 0) 
    // {    
        // fprintf(stderr,"active fb swap failed!\n");
    // }
    // else
    // {
        // fprintf(stderr,"active buffer OK:\n");
    // }
        
// }


/*****************************************************************************
 函 数 名  : pan_set_active_framebuffer
 功能描述  : 激活fb
 输入参数  : unsigned n
 输出参数  : 无
 返 回 值  : static void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年6月29日
    作    者   : gongyu
    修改内容   : 新生成函数

*****************************************************************************/
static void pan_set_active_framebuffer(int bpp,int flag)
{     
    fprintf(stderr,"pan_set_active_framebuffer enter\n");
	
    g_pan_fb_var_screen.yres_virtual      =  g_pan_fb_var_screen.yres;
    g_pan_fb_var_screen.xres_virtual      =  g_pan_fb_var_screen.xres;
    g_pan_fb_var_screen.yoffset           = 0;
    g_pan_fb_var_screen.xoffset           = 0;
    g_pan_fb_var_screen.bits_per_pixel    = bpp;
    g_pan_fb_var_screen.width		      = 0;
    g_pan_fb_var_screen.height	 	      = 0;   
    g_pan_fb_var_screen.activate          = FB_ACTIVATE_VBL; 
    g_pan_fb_var_screen.rotate		      = 0;

    fprintf(stderr,"set_active_framebuffer, activate: %d,bits_per_pixel: %d\n",g_pan_fb_var_screen.activate, bpp);
	fprintf(stderr,"g_pan_fb_var_screen.yoffset : %d\n",g_pan_fb_var_screen.yoffset);

    /* ARGB1555 */
	if ((16 == bpp) && (flag == 10))
	{
        g_pan_fb_var_screen.blue.offset	    = 0;
        g_pan_fb_var_screen.red.offset      = 10;
        g_pan_fb_var_screen.transp.offset   = 15;	
        g_pan_fb_var_screen.green.length    = 5;	
        g_pan_fb_var_screen.transp.length   = 1;
        g_pan_fb_var_screen.green.offset     = 5;
        g_pan_fb_var_screen.blue.length      = 5;
        g_pan_fb_var_screen.red.length       = 5;
        g_pan_fb_var_screen.green.msb_right  = 0;
        g_pan_fb_var_screen.blue.msb_right   = 0;
        g_pan_fb_var_screen.red.msb_right    = 0;
        g_pan_fb_var_screen.transp.msb_right = 0;	
	    fprintf(stderr,"format ARGB 1555\n");
    }
    
	 //ABGR1555 
    if((16 == bpp) && (flag == 20))
    {
        g_pan_fb_var_screen.blue.offset	    = 10;
        g_pan_fb_var_screen.red.offset      = 0;
        g_pan_fb_var_screen.transp.offset   = 15;	
        g_pan_fb_var_screen.green.length    = 5;	
        g_pan_fb_var_screen.transp.length   = 1;
        g_pan_fb_var_screen.green.offset     = 5;
        g_pan_fb_var_screen.blue.length      = 5;
        g_pan_fb_var_screen.red.length       = 5;
        g_pan_fb_var_screen.green.msb_right  = 0;
        g_pan_fb_var_screen.blue.msb_right   = 0;
        g_pan_fb_var_screen.red.msb_right    = 0;
        g_pan_fb_var_screen.transp.msb_right = 0;
        fprintf(stderr,"format ABGR 1555\n");
    }

    /* RGB565 */
	if ((16 == bpp) && (flag == 11))
	{
        g_pan_fb_var_screen.blue.offset	    = 0;
        g_pan_fb_var_screen.red.offset      = 11;
        g_pan_fb_var_screen.transp.offset   = 0;	
        g_pan_fb_var_screen.green.length    = 6;	
        g_pan_fb_var_screen.transp.length   = 0;

        g_pan_fb_var_screen.green.offset     = 5;
        g_pan_fb_var_screen.blue.length      = 5;
        g_pan_fb_var_screen.red.length       = 5;
        g_pan_fb_var_screen.green.msb_right  = 0;
        g_pan_fb_var_screen.blue.msb_right   = 0;
        g_pan_fb_var_screen.red.msb_right    = 0;
        g_pan_fb_var_screen.transp.msb_right = 0;	
	    fprintf(stderr,"format RGB 565\n");
    }
    /* BGR565 */
    if((16 == bpp) && (flag == 21))
    {
        g_pan_fb_var_screen.blue.offset	    = 11;
        g_pan_fb_var_screen.red.offset      = 0;
        g_pan_fb_var_screen.transp.offset   = 0;	
        g_pan_fb_var_screen.green.length    = 6;	
        g_pan_fb_var_screen.transp.length   = 0;

        g_pan_fb_var_screen.green.offset     = 5;
        g_pan_fb_var_screen.blue.length      = 5;
        g_pan_fb_var_screen.red.length       = 5;
        g_pan_fb_var_screen.green.msb_right  = 0;
        g_pan_fb_var_screen.blue.msb_right   = 0;
        g_pan_fb_var_screen.red.msb_right    = 0;
        g_pan_fb_var_screen.transp.msb_right = 0;
        fprintf(stderr,"format BGR 565\n");
    }
    /* ARGB8888, A is the highest */
    if ((32 == bpp)&& (flag ==10))
    {
        g_pan_fb_var_screen.blue.offset	     = 0;
        g_pan_fb_var_screen.red.offset       = 16;
        g_pan_fb_var_screen.green.offset     = 8;        
        g_pan_fb_var_screen.transp.offset    = 24;
        
        g_pan_fb_var_screen.blue.length      = 8;
        g_pan_fb_var_screen.red.length       = 8;
        g_pan_fb_var_screen.green.length     = 8;
        g_pan_fb_var_screen.transp.length    = 8;
        
        g_pan_fb_var_screen.green.msb_right  = 0;
        g_pan_fb_var_screen.blue.msb_right   = 0;
        g_pan_fb_var_screen.red.msb_right    = 0;
        g_pan_fb_var_screen.transp.msb_right = 0;	
	    fprintf(stderr,"format ARGB8888,A is the highest \n");
    }
	/* ABGR8888 A is highest */
    if ((32 == bpp)&& (flag ==20))
    {
        g_pan_fb_var_screen.blue.offset	     = 16;
        g_pan_fb_var_screen.red.offset       = 0;
        g_pan_fb_var_screen.green.offset     = 8;        
        g_pan_fb_var_screen.transp.offset    = 24;
        
        g_pan_fb_var_screen.blue.length      = 8;
        g_pan_fb_var_screen.red.length       = 8;
        g_pan_fb_var_screen.green.length     = 8;
        g_pan_fb_var_screen.transp.length    = 8;
        
        g_pan_fb_var_screen.green.msb_right  = 0;
        g_pan_fb_var_screen.blue.msb_right   = 0;
        g_pan_fb_var_screen.red.msb_right    = 0;
        g_pan_fb_var_screen.transp.msb_right = 0;	
	    fprintf(stderr,"format ABGR8888,A is the highest \n");
    }

	/* XRGB8888 X is the highest */
    if ((32 == bpp)&& (flag == 11))
    {
        g_pan_fb_var_screen.blue.offset	     = 0;
        g_pan_fb_var_screen.red.offset       = 16;
        g_pan_fb_var_screen.green.offset     = 8;        
        g_pan_fb_var_screen.transp.offset    = 24;
        
        g_pan_fb_var_screen.blue.length      = 8;
        g_pan_fb_var_screen.red.length       = 8;
        g_pan_fb_var_screen.green.length     = 8;
        g_pan_fb_var_screen.transp.length    = 0;
        
        g_pan_fb_var_screen.green.msb_right  = 0;
        g_pan_fb_var_screen.blue.msb_right   = 0;
        g_pan_fb_var_screen.red.msb_right    = 0;
        g_pan_fb_var_screen.transp.msb_right = 0;	
	    fprintf(stderr,"format XRGB8888, X is the highest\n");
    }
    /* XBGR8888, X is the highest*/
    if ((32 == bpp)&& (flag == 21))
    {
        g_pan_fb_var_screen.blue.offset	     = 16;
        g_pan_fb_var_screen.red.offset       = 0;
        g_pan_fb_var_screen.green.offset     = 8;        
        g_pan_fb_var_screen.transp.offset    = 24;
        
        g_pan_fb_var_screen.blue.length      = 8;
        g_pan_fb_var_screen.red.length       = 8;
        g_pan_fb_var_screen.green.length     = 8;
        g_pan_fb_var_screen.transp.length    = 0;
        
        g_pan_fb_var_screen.green.msb_right  = 0;
        g_pan_fb_var_screen.blue.msb_right   = 0;
        g_pan_fb_var_screen.red.msb_right    = 0;
        g_pan_fb_var_screen.transp.msb_right = 0;
	    fprintf(stderr,"format XBGR8888, X is the highest\n");
    }

	if(ioctl(g_pan_fb_fd, FBIOPUT_VSCREENINFO, &g_pan_fb_var_screen) < 0) 
    {
        fprintf(stderr,"error!active fb swap failed!\n");
    }
    else
    {
        fprintf(stderr,"active buffer OK:\n");
    }   	
}

/*****************************************************************************
 函 数 名  : dump_screen_info
 功能描述  : dump info
 输入参数  : struct fb_fix_screeninfo *fi
             struct fb_var_screeninfo *vi
 输出参数  : 无
 返 回 值  : static void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年6月29日
    作    者   : gongyu
    修改内容   : 新生成函数

*****************************************************************************/
void dump_screen_info(struct fb_fix_screeninfo *fi, struct fb_var_screeninfo *vi)
{
    fprintf(stderr,"vi.xres = %d\n", vi->xres);
    fprintf(stderr,"vi.yres = %d\n", vi->yres);
    fprintf(stderr,"vi.xresv = %d\n", vi->xres_virtual);
    fprintf(stderr,"vi.yresv = %d\n", vi->yres_virtual);
    fprintf(stderr,"vi.xoff = %d\n", vi->xoffset);
    fprintf(stderr,"vi.yoff = %d\n", vi->yoffset);
    fprintf(stderr, "vi.bits_per_pixel = %d\n", vi->bits_per_pixel);
    fprintf(stderr, "vi.width = %d\n", vi->width);
    fprintf(stderr, "vi.height = %d\n", vi->height);
    fprintf(stderr, "vi.rotate = %d\n", vi->rotate);

    fprintf(stderr, "fi.line_length = %d\n", fi->line_length);
    fprintf(stderr, "fi.smem_len = %d\n", fi->smem_len);

}

/*****************************************************************************
 函 数 名  :pan_get_framebuffer()
 功能描述  : 获取fb
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年6月29日
    作    者   : gongyu
    修改内容   : 新生成函数
  2.日    期   : 2013年09月02日
    作    者   : z00196871
    修改内容   : 修改
*****************************************************************************/
int  pan_get_framebuffer( void )
{
    int                                     fb_fd = -1;
    void                                    *bits;

    if (ioctl(g_pan_fb_fd, FBIOGET_VSCREENINFO, &g_pan_fb_var_screen) < 0) {
    
        fprintf(stderr,"error!failed to get fb0 vscreen_info\n");
        
        goto err;
    }

    if (ioctl(g_pan_fb_fd, FBIOGET_FSCREENINFO, &g_pan_fb_fix_screen) < 0){
    
        fprintf(stderr,"error!failed to get fb0 fixscreen_info\n");

        goto err;
    }
		
    dump_screen_info(&g_pan_fb_fix_screen, &g_pan_fb_var_screen);  	

    g_pan_fbbuf = mmap(0, g_pan_fb_fix_screen.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, g_pan_fb_fd, 0);

    if (g_pan_fbbuf == MAP_FAILED) {
    
        fprintf(stderr,"error!failed to mmap framebuffer");

        goto err;
    }

    fprintf(stderr,"v8r1_get_framebuffer ok");

    return 0;

err:
    fb_test_close_fb();

    return -1;
}
/*****************************************************************************
 函 数 名  : pan_displayPic
 功能描述  : 显示一张图片
 输入参数  : 无
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年6月29日
    作    者   : gongyu
    修改内容   : 新生成函数
  2.日    期   : 2013年09月02日
    作    者   : z00196871
    修改内容   : 修改
*****************************************************************************/
void pan_displayPic(char  *pic_filename)
{
    int                                 picFilefd = -1;
    struct stat                         stat;
    int                                 fileLen = 0;
    int                                 byte;
    struct fb_var_screeninfo            var_info;

    picFilefd = open(pic_filename,O_RDONLY);

    if (picFilefd < 0){
    
        fprintf(stderr,"error!cannot open file\n");
        
        return;
    }

    if (fstat(picFilefd, &stat) < 0) {
    
        fprintf(stderr,"error!cannot fstat file\n");
        
        goto err;
    }

    fileLen = stat.st_size;
	fprintf(stderr,"fileLen before read %d\n",fileLen);

    while (fileLen)
    {
        byte = read(picFilefd,g_pan_fbbuf,fileLen);

        if (byte < 0) {
        
            fprintf(stderr,"error!cannot read file \n");
			
            
            goto err;
        }

        fileLen     -= byte;

        g_pan_fbbuf += byte;
    }
    
    if (ioctl(g_pan_fb_fd, FBIOPAN_DISPLAY, &g_pan_fb_var_screen) < 0) {
    
        fprintf(stderr,"error!failed to FBIOPAN_DISPLAY fb0 fb_var_screen \n");

    }
	else
	{
	    fprintf(stderr,"set pandisplay. \n");
	}

err:
    fprintf(stderr,"fileLen after read %d\n",fileLen);
    close(picFilefd);

     /*关闭fb设备 */
    munmap(g_pan_fbbuf,g_pan_fb_fix_screen.smem_len);

    /*close(g_pan_fb_fd)*/;

    return;
}

/*****************************************************************************
  3 函数实现
*****************************************************************************/
int fb_test_pan_unit(char *pic_filename, int bpp,int flag)
{
	if (pan_get_framebuffer() < 0)  {
        fprintf(stderr,"error!v8r1_get_framebuffer fail \n");
        return -1;
    }

 	/* 初始化fb设备 */
    pan_set_active_framebuffer(bpp,flag);
    dump_screen_info(&g_pan_fb_fix_screen, &g_pan_fb_var_screen);

    /* 显示一张图片 */
    pan_displayPic(pic_filename);    

    return 0;
}

int fb_test_pan_display(char *pic_filename, char* bpp,char* flag)
{    
   	g_pan_fb_fd = fb_test_open_fb();
    if (g_pan_fb_fd < 0) {
        fprintf(stderr,"error! open fb0 fail in pan_display \n");
        return -1;
    }
    
	fb_test_pan_unit(pic_filename,atoi(bpp),atoi(flag));	
 	sleep(10);	  	
	fb_test_close_fb();  	
    return 0;
}


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif


