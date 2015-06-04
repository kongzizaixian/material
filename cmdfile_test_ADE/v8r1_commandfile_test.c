#include "v8r1_commandfile_test.h"
#include "balong_ade.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

extern int commandfile_test_blk_rot(char* patternIndex, char* pic_name1, char* pic_name2, char* pic_name3, char* pic_name4, char* pic_name5, char* pic_name6,char* ch_list, 
                char * ovly_map, char* dst_rect, char* rotation, char* out_format, char* wdma_num);
                
extern int get_resource();
extern int free_resource();
int main(int argc, char**argv)
{

	int opt;
    int ret = 0;
	fprintf(stderr,"main argc = %d\n", argc);
    ret = get_resource();
    if (ret != 0){
        fprintf(stderr,"  get_resource fail \n");
        return ret;
    }

	while ((opt = getopt(argc,argv,"v:p:k:g:m:b:sHh")) != -1)
	{
		switch (opt) {
			case 'v':
	        {
            	/* argv[2]  pattern num
            	 * argv[3]~argv[8]是文件名,
            	 * argv[9] 是通道，如果通道未使用，则填写-1,
            	 * argv[10] 是通道与图层的映射关系，如果不存在映射关系，则为-1，
            	 * argv[11] 目标rect
            	 * argv[12] ch的旋转角度
            	 * 
            	*/
            	fprintf(stderr,"-v pattern:%s f1:%s,f2:%s,f3:%s,f4:%s,f5:%s,f6:%s ch:%s, ovly:%s, dst_rect:%s, rotation:%s, comp_rot:%s, ovly_output:%s\n",
            		argv[2],argv[3],argv[4],argv[5],argv[6],argv[7],argv[8],argv[9], argv[10], argv[11], argv[12], argv[13],argv[14]);


            	commandfile_test_overlay_display(argv[2],argv[3],argv[4],argv[5],argv[6],argv[7],argv[8],argv[9], argv[10], argv[11], argv[12],argv[13], argv[14]);
                break;
            }
            case 'p':
                fb_test_pan_display(argv[2],argv[3],argv[4]);
                break;
            case 'k':
                codec_mem_alloc_proc(argv[2],argv[3]);
                break;
            case 'g':
            	comfile_test_overlay_memshare(argv[2],argv[3],argv[4],argv[5],argv[6],argv[7],argv[8],argv[9], argv[10], argv[11], argv[12],argv[13], argv[14]);
                break;
            case 'm':
                cmdfile_test_overlay_mode_switch(argv[2],argv[3],argv[4],argv[5],argv[6],argv[7],argv[8],argv[9], argv[10], argv[11], argv[12],argv[13], argv[14]);
                break;
            case 'b':
	        {   
				commandfile_test_blk_rot(argv[2], argv[3], argv[4], argv[5], argv[6], argv[7], argv[8],argv[9], argv[10], argv[11], argv[12], argv[13], argv[14]);
                break;
            }
            case 's':
                cmdfile_test_overlay_mode_switch_stop();
                break;
            case 'H':
            case 'h':                
            default:
                fprintf(stderr,"-v arg[1]:pattern_num=1~30 match with STC \n");
                fprintf(stderr,"   arg[2-7]:filename=xxx_wxh_format.bint \n");
                fprintf(stderr,"   arg[8]:ch_list,such as = 0:1:2:3:4:5 \n");
                fprintf(stderr,"   arg[9]:surface_num,such as = 1:2:3:4:5:0 \n");
                fprintf(stderr,"   arg[10]: ch_dst_rect_list,such as = x1:y1:w1:h1_x2:y2:w2:h2_... \n");
                fprintf(stderr,"   arg[11]: rotation_list,such as = 0:0:90:270:180:0 \n");
                fprintf(stderr,"   when compose mode is hybird, must have a invalid picture take a position for after offline compose layer \n");
                fprintf(stderr,"   such as -v 10 pic1.bin pic2.bin xxx.bin 1:2:3:4 3:4:1:0 ..., so, ch1,ch2 is offline, ch3,ch4 is online, ch3 will read \n");
                fprintf(stderr,"   data from framebuffer, then compose with ch4 \n");
                fprintf(stderr,"-m: the arg is the same as -v, you can start to test Mode switch with this command. such as -m 1 ...(online), -m 2 ...(offline) \n");
                fprintf(stderr,"    then must use -s to stop the test \n");

		        fprintf(stderr,"-w: test for mem share to codec;");
				fprintf(stderr,"\targ[1]: eventflag(such as 1 means alloc mem, 2 means freeing mem)\n");
				fprintf(stderr,"\targ[2]: memSize: 76800 or 153600\n"); 

		        fprintf(stderr,"-b: block rot \n");
                fprintf(stderr,"-s: Stop mode switch test,or use to close FB dev and ADE dev \n");
                fprintf(stderr,"-p arg[2]: filename,arg[3]:bpp, arg[4]: flag,10,20,11,21 \n");
                fprintf(stderr,"-h or -H, help \n");
                break;
            
	    }
	}
    free_resource();
	
	return 0;
}

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

