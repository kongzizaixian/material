#include  "v8r1_commandfile_test.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

extern struct fb_var_screeninfo g_fb_var_screen;
extern struct fb_fix_screeninfo g_fb_fix_screen;

/*****************************************************************************
 1 函数实现
*****************************************************************************/
int fb_test_close_fb(void)
{
    /* 获取fb设备，并打开 */
    if (g_fb_fd > 0) {
    	close(g_fb_fd);
    }

    return 0;
}


int fb_test_close_ade(void)
{
    /*关闭ade设备 */
    if (g_ade_fd > 0) {
    	close(g_ade_fd);
    }

    return 0;
}
int fb_test_open_fb(void)
{
    /* 获取fb设备，并打开 */
    if (g_fb_fd == -1) {
        g_fb_fd = open(FB_DEV_FILE_PATH, O_RDWR);
        if (g_fb_fd < 0) {
            fprintf(stderr,"cannot open /dev/graphics/fb0, retrying with /dev/fb0\n");
            if ((g_fb_fd = open("/dev/fb0", O_RDWR)) < 0) {        
                fprintf(stderr,"cannot open /dev/fb0\n");
                return -1;
            }
        }
    }

    if(0 != ioctl(g_fb_fd, FBIOGET_VSCREENINFO, &g_fb_var_screen)){
        fprintf(stderr,"FBIOGET_VSCREENINFO fail\n");
        return -1;
    }
    
    if( 0 != ioctl(g_fb_fd, FBIOGET_FSCREENINFO, &g_fb_fix_screen)){
        fprintf(stderr,"FBIOGET_FSCREENINFO fail\n");
        return -1;
    }
    
	fprintf(stderr,"open fb:%d succ!\n",g_fb_fd);
    return g_fb_fd;
}

int fb_test_open_ade(void)
{
    /* 获取ade设备，并打开 */
    if (g_ade_fd == -1) {
        g_ade_fd = open(ADE_DEV_FILE_PATH, O_RDWR);

        if (g_ade_fd < 0) {
            fprintf(stderr,"cannot open /dev/graphics/ade\n");
            return -1;
        }
    }
    
	fprintf(stderr,"open ade:%d succ!\n",g_ade_fd);
    return g_ade_fd;
}


int fb_test_openclose_ade(char* opp)
{
	int times = atoi(opp);
	while(times--)
	{
		 g_ade_fd = fb_test_open_ade();
	       if ( g_ade_fd < 0) {

	            return -1;
	        }
		   sleep(1);
		fb_test_close_ade();
		sleep(1);

	}
	return 0;
}
int fb_test_get_fb(void)
{
    /* 获取fb设备id */
	 fprintf(stderr,"get_fb: %d\n",g_fb_fd);
    return (g_fb_fd);
}

void fb_set_ade_power_off(int val)
{
    int fd = -1;

    fd = open(ADE_OFF_DEBUGFS, O_RDWR);
    if (fd < 0) {
        fprintf(stderr,"open /sys/kernel/debug/mit1_ade_off error:%d\n",g_fb_fd);
        return;
    } 

    fprintf(fd,"%d", val);
    close(fd);
}


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

