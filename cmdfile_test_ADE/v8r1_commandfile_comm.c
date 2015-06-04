#include "v8r1_commandfile_test.h"



#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  2 全局变量定义
*****************************************************************************/


/* default value is rgb */
int   g_ch_rgb = 0;
int   g_blending_mode = HWC_BLENDING_PREMULT;

struct fb_var_screeninfo g_fb_var_screen;
struct fb_fix_screeninfo g_fb_fix_screen;
struct OVERLAY_IMAGE_ST  g_overlay_dst;

int    g_fb_fd = -1;
int    g_ade_fd = -1;

extern int ovly2_ch_count;
extern int sec_ovly_count;
extern struct overlay_resource_info *pg_ov_res;
extern int   g_ion_fd;

extern int overlay_get_ion_mem(int* psharefilefd, void** vir_addr, uint32_t* phy_addr, int len, uint32_t* hnd_id);
void get_num_by_string(int *num_list, char* string);
void get_overlay_display_rect_info(struct st_ch_display_info  *ch_display_info, int ch_rot, struct ade_rect ch_dst_rect, struct ade_rect   ovly_out_rect);
extern int fb_test_open_fb(void);
/*****************************************************************************
  3 函数实现
*****************************************************************************/
void get_ch_compose_info(struct st_ch_display_info  *ch_display_info,
						 int *file_num, 
						 char *filename[],
						 char* ch_list,
						 char* ovly_map,
						 char* rotation,
						 char* dst_rect,
						 struct overlay_resource_info* ov_res,
						 char* ovly_rect,
						 int *offline_count,
						 int * block_count)
{
	int   	 ch_num_list[ADE_CH_NUM_MAX] = {-1};
	int   	 ovly_list[ADE_CH_NUM_MAX]   = {-1};
	int   	 ch_rot_list[ADE_CH_NUM_MAX] = {0}; 
	struct ade_rect   ch_dst_rect[ADE_CH_NUM_MAX];
	struct ade_rect   ovly_out_rect;
	int 	 i;
	int      j = 0;

	*file_num = 0;

	get_num_by_string(ch_num_list,  ch_list);
	get_num_by_string(ovly_list,  ovly_map);
	get_num_by_string(ch_rot_list,  rotation);
	get_rect_by_string(ch_dst_rect, ADE_CH_NUM_MAX, dst_rect);

	if (ovly_rect == NULL) {
        ovly_out_rect.x = 0;
        ovly_out_rect.y = 0;
        ovly_out_rect.w = g_overlay_dst.width;
        ovly_out_rect.h = g_overlay_dst.height;
	} else {
    	get_rect_by_string(&ovly_out_rect, 1, ovly_rect);
    }
	
	for (i = 0; i < ADE_CH_NUM_MAX; i++) {	    
		if ((strstr(filename[i],".bin") == NULL) || (ch_num_list[i] == -1)
		 || (ovly_list[i] == -1)) {
			continue;
		}

		fprintf(stderr,"ch_num_list[%d] : %d\n",i, ch_num_list[i]);

		ch_display_info[j].ch 		   = ch_num_list[i];
		ch_display_info[j].overlay_num = ovly_list[i];
		ch_display_info[j].file_name   = filename[i];
		ch_display_info[j].cfg_disable = 1;
		ch_display_info[j].is_ov_compose = 1;
		ch_display_info[j].ch_cap      = ov_res->compose_ch_res[ch_num_list[i]].ul32;

		if (pg_ov_res != NULL) {
            if (pg_ov_res->compose_ch_res[ch_display_info[j].ch].bits.has_ovly3) {
                if (pg_ov_res->ovly3_res.bits.ovly_en) 
                {
                    if (pg_ov_res->ovly3_res.bits.has_ovly1 || pg_ov_res->ovly3_res.bits.has_ovly2)
                    {
                        sec_ovly_count++;
                        ch_display_info[j].has_sec_ovly = 1;
                    }

                    if (pg_ov_res->ovly3_res.bits.has_720p_rot || pg_ov_res->ovly3_res.bits.has_1080p_rot) 
                    {
                        if (pg_ov_res->rot_res.bits.has_ovly1 || pg_ov_res->rot_res.bits.has_ovly2)
                        {
                            sec_ovly_count++;
                            ch_display_info[j].has_sec_ovly = 1;
                        }

                        if (pg_ov_res->rot_res.bits.has_scl2)
                        {
                            if (pg_ov_res->scl2_res.bits.has_ovly1 || pg_ov_res->scl2_res.bits.has_ovly2)
                            {
                                sec_ovly_count++;
                                ch_display_info[j].has_sec_ovly = 1;
                            }                            
                        }
                    }

                    if (pg_ov_res->ovly3_res.bits.has_scl2) 
                    {
                        if (pg_ov_res->scl2_res.bits.has_ovly1 || pg_ov_res->scl2_res.bits.has_ovly2)
                        {
                            sec_ovly_count++;
                            ch_display_info[j].has_sec_ovly = 1;
                        }
                    }
                }
            }    
		}

		if (ov_res->compose_ch_res[ch_num_list[i]].bits.has_ovly1) {
            ch_display_info[j].ch_type = OVERLAY_PIPE_TYPE_OFFLINE;
            (*offline_count)++;
            fprintf(stderr,"*offline_count = %d\n",*offline_count);
		} else if (ov_res->compose_ch_res[ch_num_list[i]].bits.has_ovly3 
		    && (   ov_res->ovly3_res.bits.has_ovly1
		        || ov_res->rot_res.bits.has_ovly1
		        || ov_res->scl2_res.bits.has_ovly1
		        || ov_res->ovly3_res.bits.has_offline)) {
            ch_display_info[j].ch_type = OVERLAY_PIPE_TYPE_OFFLINE;
            (*offline_count)++;
            fprintf(stderr,"*offline_count = %d\n",*offline_count);
		} else if ((ov_res->compose_ch_res[ch_num_list[i]].bits.has_ovly2)
		        && (ov_res->ovly2_res.bits.has_offline)) {
		    ch_display_info[j].ch_type = OVERLAY_PIPE_TYPE_OFFLINE;
            (*offline_count)++;
            fprintf(stderr,"*offline_count = %d\n",*offline_count);            
		} else {
		    ch_display_info[j].ch_type = OVERLAY_PIPE_TYPE_ONLINE;   //ch 0 1  set value  kkk
		    if ((ov_res->compose_ch_res[ch_num_list[i]].bits.has_ovly2)
		        && (!ov_res->ovly2_res.bits.has_offline)) {
                ovly2_ch_count++;  // how mang overlays kkk
		    }
		}		
		
		ch_display_info[j].comp_mode   = ov_res->comp_mode;   //cmp_mode     

		/* 获取src rect 和dest rect */
		get_overlay_display_rect_info(&(ch_display_info[j]), ch_rot_list[i], ch_dst_rect[i], ovly_out_rect);

        if ((ov_res->compose_ch_res[ch_num_list[i]].bits.has_block_rot)
		 && (ch_display_info[j].rotation != 0))
		{
			(*block_count)++;
			ch_display_info[j].has_block_rot = 1;
		}

		fprintf(stderr,"offline_count = %d\n",*offline_count);
	    fprintf(stderr,"block_count = %d\n",*block_count);
	    fprintf(stderr,"has_block_rot = %d\n",ch_display_info[j].has_block_rot);
	    fprintf(stderr,"ch_display_info.ch : %d\n",ch_display_info[j].ch);
	    fprintf(stderr,"ch_display_info.overlay_num : %d\n",ch_display_info[j].overlay_num);
	    fprintf(stderr,"ch_display_info.ch_type : %d\n",ch_display_info[j].ch_type);
	    fprintf(stderr,"ch_display_info.file_name : %s\n",ch_display_info[j].file_name);
	    fprintf(stderr,"ch_display_info.ch_cap : 0x%x\n",ch_display_info[j].ch_cap);
	    
		j++;
	}

	*file_num = j;

	/* 只有一个图层 */
	if (*file_num == 1) {
		ch_display_info[*file_num - 1].is_ov_compose = 0;
	}
}

int get_pic_size_by_filename(char *pic_filename , uint32_t* width, uint32_t* height)
{
    char* file_size1;
    char  seps2[10] = {0};
    int   i = 0;
    int   j = 0;
    int	  firstPos = 0;
    int	  secPos   = strlen(pic_filename);


    fprintf(stderr,"pic_filename seps1 : %s\n",pic_filename);

    if (!(strchr(pic_filename, 'x'))
     && !(strchr(pic_filename, 'X')))
    {
    	fprintf(stderr,"pic_filename not include size : %s\n",pic_filename);

    	return -1;
    }

    file_size1 = strchr(pic_filename, '_');

    if (file_size1)
    {
    	firstPos = strlen(pic_filename) - strlen(file_size1);

    	fprintf(stderr,"pic_filename file_size1 : %s\n",file_size1);

    	file_size1 = strchr(file_size1 + 1, '_');

    	if (file_size1)
    	{
    		secPos = strlen(pic_filename) - strlen(file_size1);
    	}
    }

    *width     = 0;
    *height    = 0;

    for (i = firstPos + 1; i < secPos; i++)
    {
        if ((pic_filename[i] <= '9')
         && (pic_filename[i] >= '0'))
        {
            seps2[j]  = pic_filename[i];
            j++;
        }
		else if ((pic_filename[i] == 'x')
		      || (pic_filename[i] == 'X'))
		{
		    *width = atoi(seps2);

		     j = 0;
		}
    }

    seps2[j] = '\0';

    *height = atoi(seps2);

    fprintf(stderr,"pic_filename width : %d, height: %d\n", *width, *height);

    return 0;
}

void get_dst_rect_by_rect_str(char* rect, struct ade_rect* dst_rect)
{
    int                                 i = 0;
    int                                 j = 0;
    int                                 k = 0;
    char                                num_char[5] = {0};
    uint32_t*                           dst_data[4] = {&dst_rect->x,&dst_rect->y,
                                                       &dst_rect->w,&dst_rect->h};


    fprintf(stderr,"get_dst_rect_by_rect_str: rect:%s\n",rect);

    if (0 == rect)
    {
        dst_rect->x = 0;
        dst_rect->y = 0;
        dst_rect->w = g_overlay_dst.width;
        dst_rect->h = g_overlay_dst.height;


        fprintf(stderr, "get_dst_rect_by_rect_str dst_rect.x       = %d\n", dst_rect->x);
        fprintf(stderr, "get_dst_rect_by_rect_str dst_rect.y       = %d\n", dst_rect->y);
        fprintf(stderr, "get_dst_rect_by_rect_str dst_rect.w       = %d\n", dst_rect->w);
        fprintf(stderr, "get_dst_rect_by_rect_str dst_rect.h       = %d\n", dst_rect->h);

        return;
    }

    while (rect[j])
    {
        if (rect[j] <= '9' && rect[j] >= '0')
        {
            num_char[k] = rect[j];

            k++;
        }
        else if (('x' == rect[j])
               ||('X' == rect[j])
               ||(':' == rect[j]))
        {
            if (i > 3)
            {
                fprintf(stderr,"get_dst_rect_by_rect_str: dst_rect.x:%d\n",dst_rect->x);
                fprintf(stderr,"get_dst_rect_by_rect_str: dst_rect.y:%d\n",dst_rect->y);
                fprintf(stderr,"get_dst_rect_by_rect_str: dst_rect.w:%d\n",dst_rect->w);
                fprintf(stderr,"get_dst_rect_by_rect_str: dst_rect.h:%d\n",dst_rect->h);

                return;
            }

            /* 依次为x, y ,w, h */
            *dst_data[i] = atoi(num_char);

             i++;

             k = 0;
        }

        j++;
    }

    *dst_data[i] = atoi(num_char);

    fprintf(stderr,"get_dst_rect_by_rect_str:: dst_rect.x:%d\n",dst_rect->x);
    fprintf(stderr,"get_dst_rect_by_rect_str:: dst_rect.y:%d\n",dst_rect->y);
    fprintf(stderr,"get_dst_rect_by_rect_str:: dst_rect.w:%d\n",dst_rect->w);
    fprintf(stderr,"get_dst_rect_by_rect_str:: dst_rect.h:%d\n",dst_rect->h);

    return;
}

void get_num_by_string(int *num_list, char* string)
{
	char str[4];
	int  i = 0;
	int  j = 0;
	int  k = 0;    
	
    fprintf(stderr,"string:: %s\n", string);
    
	while (string[i] != '\0') {
		if ((string[i] != ':') && (string[i] != 'x')) {
			if (j < 4) {
				str[j] = string[i];
				j++;
			}
		}

		if ((string[i] == ':') || (string[i] == 'x')) {
			str[j%4] = '\0';
			num_list[k] = atoi(str);
			fprintf(stderr,"get_num_by_string:: num_list[%d]=%d\n", k, num_list[k]);
			k++;
			j = 0;
		}

		i++;
	}
	str[j%4] = '\0';
	num_list[k] = atoi(str);
	fprintf(stderr,"get_num_by_string:: num_list[%d]=%d\n", k, num_list[k]);
}

/* string的格式为:x1:y1_x2:y2_x3:y3_x4:y4_x5:y5_x6:y6
*/

void get_xy_by_string(struct st_xy_pos *xy_pos, char* string)
{
	char  str[10] = {0};
	int   i = 0;
	int   j = 0;
	int   k = 0;

	while (string[i] != '\0') {
		if ((string[i] != ':') && (string[i] != '_')) {
			if (j < 10) {
				str[j] = string[i];
				j++;
			}
		}

		if (string[i] == ':') {
			str[j%10] = '\0';
			xy_pos[k].x = atoi(str);

			j = 0;
		}

		if (string[i] == '_') {
			str[j%10] = '\0';
			xy_pos[k].y = atoi(str);

			j = 0;

			k++;
		}

		i++;
	}
    str[j%10] = '\0';
    xy_pos[k].y = atoi(str);	
}


/* x1:y1:w1:h1_x2:y2:w2:h2 */
void get_rect_by_string(struct ade_rect* ch_dst_rect, int ch_num, char* string)
{
    int   i = 0;
	int   j = 0;
	int   m = 0;
	int   n = 0;
	char  str[10] = {0};
    
    memset(ch_dst_rect, 0, sizeof(struct ade_rect) * ch_num);
    if (string == NULL) {        
        return;
    }

    while (string[i] != '\0') {
        if (n >= ch_num) {
            fprintf(stderr, "n=%d is over flow ch_num= %d\n", n, ch_num);
            return;
        }
        
		if ((string[i] != ':') && (string[i] != '_')) {
			if (j < 10) {
				str[j] = string[i];
				j++;
			}
		}

		if (string[i] == ':') {
			str[j%10] = '\0';

            if (m == 0) {			
			    ch_dst_rect[n].x = atoi(str);
			} else if (m == 1) {
                ch_dst_rect[n].y = atoi(str);
			} else if (m == 2) {
                ch_dst_rect[n].w = atoi(str);
			}			

			j = 0;
			m++;
		}

		if (string[i] == '_') {
			str[j%10] = '\0';
			ch_dst_rect[n].h = atoi(str);

			j = 0;
            m = 0;
            
			n++;
		}

		i++;
	}

	if (n >= ch_num) {
        fprintf(stderr, "n=%d is over flow ch_num=%d \n", n, ch_num);
        return;
    }
	
    str[j%10] = '\0';
	ch_dst_rect[n].h = atoi(str);
    
}

/* xy_list的格式为:x1:y1_x2:y2_x3:y3_x4:y4_x5:y5_x6:y6,如果通道不使用，值为-1
 * ch1_rect格式为:x:y:w:h,只能设置ch1的值
 * rotation的值为，0,90,180,270，只能设置ch1的值
 */
void get_overlay_display_rect_info(struct st_ch_display_info  *ch_display_info, int ch_rot, struct ade_rect ch_dst_rect, struct ade_rect   ovly_out_rect)
{
	uint32_t    width  = 0;
	uint32_t    height = 0;
	CH_RES      ch_res;
	

	if ((NULL == ch_display_info)) {
		fprintf(stderr, "para is null \n");
		return;
	}

	get_pic_size_by_filename(ch_display_info->file_name, &width, &height);

	ch_display_info->src_rect.x = 0;
	ch_display_info->src_rect.y = 0;
	ch_display_info->src_rect.w = width;
	ch_display_info->src_rect.h = height;

	ch_display_info->dest_rect.x = 0;
	ch_display_info->dest_rect.y = 0;
	ch_display_info->dest_rect.w = width;
	ch_display_info->dest_rect.h = height;

	ch_display_info->rotation    = ch_rot;

	if (ch_dst_rect.w != 0 && ch_dst_rect.h != 0) {
        ch_display_info->dest_rect.x = ch_dst_rect.x;
    	ch_display_info->dest_rect.y = ch_dst_rect.y;
    	ch_display_info->dest_rect.w = ch_dst_rect.w;
    	ch_display_info->dest_rect.h = ch_dst_rect.h;
	}

	if (pg_ov_res->compose_ch_res[ch_display_info->ch].bits.has_ovly2) {
        ch_display_info->ovly_output_rect.w = g_overlay_dst.width;
    	ch_display_info->ovly_output_rect.h = g_overlay_dst.height;
    	ch_display_info->ovly_output_rect.x = 0;
    	ch_display_info->ovly_output_rect.y = 0;
	}

	if (ch_display_info->has_sec_ovly == 1) {
	    ch_res.ul32 = pg_ov_res->compose_ch_res[ch_display_info->ch].ul32;
	    if ((ch_res.bits.has_scl1 || ch_res.bits.has_scl3)
	     && (1 == sec_ovly_count)) {
            ch_display_info->ovly_output_rect.w = ch_display_info->dest_rect.w;
        	ch_display_info->ovly_output_rect.h = ch_display_info->dest_rect.h;
        	ch_display_info->ovly_output_rect.x = ch_display_info->dest_rect.x;
        	ch_display_info->ovly_output_rect.y = ch_display_info->dest_rect.y;
    	} else {           
            ch_display_info->ovly_output_rect.w = ovly_out_rect.w;
        	ch_display_info->ovly_output_rect.h = ovly_out_rect.h;
        	ch_display_info->ovly_output_rect.x = ovly_out_rect.x;
        	ch_display_info->ovly_output_rect.y = ovly_out_rect.y;

        	/* ch_display_info->ovly_output_rect.w = ch_display_info->src_rect.w;
        	ch_display_info->ovly_output_rect.h = ch_display_info->src_rect.h;
        	ch_display_info->ovly_output_rect.x = ch_display_info->src_rect.x;
        	ch_display_info->ovly_output_rect.y = ch_display_info->src_rect.y;   
        	*/
    	}    	
	} else {
        ch_display_info->ovly_output_rect.w = ovly_out_rect.w;
    	ch_display_info->ovly_output_rect.h = ovly_out_rect.h;
    	ch_display_info->ovly_output_rect.x = ovly_out_rect.x;
    	ch_display_info->ovly_output_rect.y = ovly_out_rect.y;
	}
}

int get_resource()
{
    
    void*    ion_vir_addr;
    int      file_len = 0;  
    int      rslt = 0;
    
	g_fb_fd = fb_test_open_fb();
    file_len = g_fb_var_screen.xres  * g_fb_var_screen.yres * 4;   
    rslt = overlay_get_ion_mem(&g_overlay_dst.share_fd, &ion_vir_addr, &g_overlay_dst.phy_addr,file_len ,&g_overlay_dst.hnd_id);
    if (rslt != 0)
    {
        fprintf(stderr," overlay_get_ion_mem error\n");
        return -1;
    }

    g_overlay_dst.width     = g_fb_var_screen.xres;
    g_overlay_dst.height    = g_fb_var_screen.yres;
    g_overlay_dst.filelen   = file_len;
    g_overlay_dst.stride    = g_fb_var_screen.xres * 4;

    return 0;
}

int free_resource()
{
    int ret = 0;
	ret = overlay_ion_free(g_ion_fd, g_overlay_dst.hnd_id);
    if (ret != 0){
        fprintf(stderr,"  free dst mem fail \n");
        return ret;
    }
    return 0;
}

