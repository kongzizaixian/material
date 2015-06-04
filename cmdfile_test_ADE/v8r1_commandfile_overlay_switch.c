#include "v8r1_commandfile_test.h"
#include <linux/ion.h>


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

extern int fb_test_open_fb(void);
extern int fb_test_open_ade(void);
extern int fb_test_close_fb(void);
extern int fb_test_close_ade(void);

static int m_ade_fd = -1;

#define  PATTERN_ID_FILE    "/data/pattern_id.txt"

int cmdfile_test_get_patten_id(void)
{
    FILE* fd = NULL;
    char id[10] = {0};
    
    fd = fopen(PATTERN_ID_FILE, "rt");
    if (fd == NULL) {
        fprintf(stderr, "get /data/pattern_id.txt fail \n");
        return -1;
    }
    
    fread(id, sizeof(char), sizeof(id), fd);
    
    fprintf(stderr,"pattern id is %s in /data/pattern_id.txt \n", id);

    fclose(fd);

    return atoi(id);
}

void cmdfile_test_set_patten_id(int pattern_id)
{
    FILE* fd = NULL;   
    loff_t  pos = 0;
    char id[10] = {0};
    
    fd = fopen(PATTERN_ID_FILE, "wt+");
    if (fd == NULL) {
        fprintf(stderr, "cmdfile_test_set_patten_id:get /data/pattern_id.txt fail \n");
        return;
    }
    
    snprintf(id,sizeof(id), "%d", pattern_id);    
    fprintf(stderr, "id = %d, count=%d \n", id, strlen(id));
    id[strlen(id)] = 0;
    
    fputs(id, fd);
    
    fclose(fd);

    return;
}

int cmdfile_test_overlay_start_self_refresh(int ade_fd)
{
    int refresh_mode = ADE_REFRESH_SWITCH;
    
    if (ade_fd < 0) {
        fprintf(stderr, "cmdfile_test_overlay_start_self_refresh: error: ade_fd < 0 \n");
        return -1;
    }

    if (ioctl(ade_fd, ADE_OVERLAY_SELF_REFS, &refresh_mode) < 0) {
        fprintf(stderr, "cmdfile_test_overlay_start_self_refresh: ioctl error \n");
        return -1;
    }

    return 0;
}

/* start mode switch test */
void cmdfile_test_overlay_mode_switch(char* pattern, char* pic1_name, char* pic2_name, char* pic3_name,
								  char* pic4_name, char* pic5_name, char* pic6_name,char* ch_list, char* ovly_map,
								  char* dst_rect, char* rotation, char* comp_rot, char* ovly_rect)
{
    int p_index;
    int old_idx;
    int ret;
    
    p_index = atoi(pattern);
    if (m_ade_fd == -1) {
        ret = fb_test_open_fb();
        if(-1 == ret){
            return;
        }
        
        m_ade_fd = fb_test_open_ade();

        /* disable ade_power off */
        //fb_set_ade_power_off(0);
    }
    
    old_idx = cmdfile_test_get_patten_id();    
	if (old_idx == -1) {

		/* NO patten_id, then set patten_id */
		cmdfile_test_set_patten_id(p_index);
        //for error test
        //commandfile_test_overlay_display(pattern, pic1_name, pic2_name, pic3_name, pic4_name, pic5_name, pic6_name, ch_list,
        //                         ovly_map, dst_rect, rotation, comp_rot, ovly_rect);    

	} else {
		if (old_idx != p_index) {
				/* need start self-refresh */
				cmdfile_test_set_patten_id(p_index);
				if (cmdfile_test_overlay_start_self_refresh(m_ade_fd) < 0) {
						fprintf(stderr, "cmdfile_test_overlay_mode_switch: cmdfile_test_overlay_start_self_refresh is error");
						return;
				}
				fprintf(stderr, "p_index = %d \n", p_index);
		}
	}

    fprintf(stderr, "old_idx = %d ,p_index = %d\n", old_idx, p_index);

    /* start display */
    commandfile_test_overlay_display(pattern, pic1_name, pic2_name, pic3_name, pic4_name, pic5_name, pic6_name, ch_list,
                                 ovly_map, dst_rect, rotation, comp_rot, ovly_rect);
}

/* stop mode switch test */
void cmdfile_test_overlay_mode_switch_stop()
{
    cmdfile_test_set_patten_id(-1);
    //fb_set_ade_power_off(1);
    fb_test_close_fb();
    fb_test_close_ade();
}

static int mem_share_fd = -1;
/* balong_ade_mem_share_test */
int codec_mem_alloc_proc(char *eventflag, char *memsize)
{
		struct mem_share_test       mem_share_para;
        int ret = 0;
        
		mem_share_para.event_flag   = atoi(eventflag);
		mem_share_para.mem_size     = atoi(memsize);

		fprintf(stderr, "event_flag = %d, mem_size = %d\n", 
						mem_share_para.event_flag, mem_share_para.mem_size);

		if (mem_share_fd == -1) {
				ret = fb_test_open_fb();
				if(-1 == ret){
                    return -1;
				}
				mem_share_fd = fb_test_open_ade();
				fprintf(stderr, "Open ade succ!\n");
		}

		if(ioctl(mem_share_fd, ADE_MEM_SHARE_CODEC_PROC, &mem_share_para) < 0){
				return -1;
		}
		
		fprintf(stderr, "Exec ioctl mem_share succ!\n");

		return 0;
}


#ifdef __cplusplus
    #if __cplusplus
}
    #endif
#endif

