#ifndef ____V8R1_COMMANDFILE_TEST_H____
#define ____V8R1_COMMANDFILE_TEST_H____

/*****************************************************************************
  1 ÆäËûÍ·ÎÄ¼þ°üº¬
*****************************************************************************/

#include <stdlib.h>
#include <unistd.h>

#include <fcntl.h>
#include <stdio.h>

#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <time.h>

#include <linux/fb.h>
#include <linux/kd.h>
#include <linux/android_pmem.h>
#include "balong_ade.h"



#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define ADE_CH_NUM_MAX					(6)

extern int   g_blending_mode;

extern int    g_fb_fd;
extern int    g_ade_fd;

#define PMEM_DEV_FILE_NAME           "/dev/gralloc_pmem"
#define ION_DEV_FILE_NAME            "/dev/ion"
#define FB_DEV_FILE_PATH            "/dev/graphics/fb0"
#define ADE_DEV_FILE_PATH           "/dev/graphics/ade"

#define ADE_OFF_DEBUGFS             "/sys/kernel/debug/mit1_ade_off"


#define HWC_BLENDING_PREMULT 			(0x0105)
#define HWC_BLENDING_COVERAGE 			(0x0405)

enum HISI_ION_HEAP_ID {
	INVALID_HEAP_ID = -1,
	ION_SYSTEM_HEAP_ID = 0,
	ION_SYSTEM_CONTIG_HEAP_ID = 1,
	ION_GRALLOC_HEAP_ID = 2,
	ION_DMA_HEAP_ID = 3,
	ION_DMA_POOL_HEAP_ID = 4,
	ION_CPU_DRAW_HEAP_ID = 5,
	ION_CAMERA_HEAP_ID = 6,
	ION_OVERLAY_HEAP_ID = 7,
    ION_VCODEC_HEAP_ID = 8,
    ION_ISP_HEAP_ID = 9,
    ION_FB_HEAP_ID = 10,
    ION_VPU_HEAP_ID = 11,
    ION_JPU_HEAP_ID = 12,
	HISI_ION_HEAP_IOMMU_ID,
	ION_HEAP_ID_RESERVED = 31 /* Bit reserved */
};

#define IOMMU_PAGE_SHIFT      12
#define IOMMU_PAGE_SIZE       (8<<10) /*PAGE_SIZE 4k*/
         
#define HISI_ION_HEAP_IOMMU_USER_MASK (1 << HISI_ION_HEAP_IOMMU_ID)
#define HISI_ION_ALLOC_FLAG (ION_FLAG_CACHED | ION_FLAG_CACHED_NEEDS_SYNC)

#define ION_FLAG_CACHED 1		/* mappings of this buffer should be
					   cached, ion will do cache
					   maintenance when the buffer is
					   mapped for dma */
#define ION_FLAG_CACHED_NEEDS_SYNC 2	/* mappings of this buffer will created
					   at mmap time, if this is set
					   caches must be managed manually */


enum K3V2_ION_HEAP_ID {
	K3V2_ION_HEAP_CAMERA_ID = 1,
	K3V2_ION_HEAP_GRALLOC_ID,
	K3V2_ION_HEAP_OVERLAY_ID,
	K3V2_ION_HEAP_K3V2_ID,
	K3V2_ION_HEAP_USER_ID,
	K3V2_ION_HEAP_SYS_ID,
	K3V2_ION_HEAP_SYS_CONTIG_ID,
};


enum ION_K3V2_CUSTOM_CMD {
	ION_HISI_CUSTOM_PHYS,
	ION_HISI_FLUSH_ALL_CACHES,
    ION_HISI_CUSTOM_GET_KERN_VA,
    ION_HISI_CUSTOM_FREE_KERN_VA,
    ION_HISI_CUSTOM_ISSUPPORT_IOMMU,
};

enum {
    HAL_PIXEL_FORMAT_RGBA_8888          = 1,
    HAL_PIXEL_FORMAT_RGBX_8888          = 2,
    HAL_PIXEL_FORMAT_RGB_888            = 3,
    HAL_PIXEL_FORMAT_RGB_565            = 4,
    HAL_PIXEL_FORMAT_BGRA_8888          = 5,
    HAL_PIXEL_FORMAT_RGBA_5551          = 6,
    HAL_PIXEL_FORMAT_RGBA_4444          = 7,

    /* 0x8 - 0xFF range unavailable */

    /*
     * 0x100 - 0x1FF
     *
     * This range is reserved for pixel formats that are specific to the HAL
     * implementation.  Implementations can use any value in this range to
     * communicate video pixel formats between their HAL modules.  These formats
     * must not have an alpha channel.  Additionally, an EGLimage created from a
     * gralloc buffer of one of these formats must be supported for use with the
     * GL_OES_EGL_image_external OpenGL ES extension.
     */

    /*
     * Android YUV format:
     *
     * This format is exposed outside of the HAL to software decoders and
     * applications.  EGLImageKHR must support it in conjunction with the
     * OES_EGL_image_external extension.
     *
     * YV12 is a 4:2:0 YCrCb planar format comprised of a WxH Y plane followed
     * by (W/2) x (H/2) Cr and Cb planes.
     *
     * This format assumes
     * - an even width
     * - an even height
     * - a horizontal stride multiple of 16 pixels
     * - a vertical stride equal to the height
     *
     *   y_size = stride * height
     *   c_size = ALIGN(stride/2, 16) * height/2
     *   size = y_size + c_size * 2
     *   cr_offset = y_size
     *   cb_offset = y_size + c_size
     *
     */
    HAL_PIXEL_FORMAT_YV12   = 0x32315659, // YCrCb 4:2:0 Planar



    /* Legacy formats (deprecated), used by ImageFormat.java */
    HAL_PIXEL_FORMAT_YCbCr_422_SP       = 0x10, // NV16
    HAL_PIXEL_FORMAT_YCrCb_420_SP       = 0x11, // NV21
    HAL_PIXEL_FORMAT_YCbCr_422_P        = 0x12,
    HAL_PIXEL_FORMAT_YCbCr_420_P        = 0x13,
    HAL_PIXEL_FORMAT_YCbCr_422_I        = 0x14, // YUY2
    HAL_PIXEL_FORMAT_YCbCr_420_I        = 0x15,
    HAL_PIXEL_FORMAT_CbYCrY_422_I       = 0x16,
    HAL_PIXEL_FORMAT_CbYCrY_420_I       = 0x17,
    HAL_PIXEL_FORMAT_YCbCr_420_SP_TILED = 0x20,
    HAL_PIXEL_FORMAT_YCbCr_420_SP       = 0x21,
    HAL_PIXEL_FORMAT_YCrCb_420_SP_TILED = 0x22,
    HAL_PIXEL_FORMAT_YCrCb_422_SP       = 0x23,
    HAL_PIXEL_FORMAT_YUYV_422_I       = 0x23,
    HAL_PIXEL_FORMAT_UYVY_422_I       = 0x24,
    HAL_PIXEL_FORMAT_YVYU_422_I       = 0x25,
    HAL_PIXEL_FORMAT_VYUY_422_I       = 0x26,
    HAL_PIXEL_FORMAT_YCrCb_420_SP_INTERLACE = 0x10C,
};

enum {
    /* flip source image horizontally (around the vertical axis) */
    HAL_TRANSFORM_FLIP_H    = 0x01,
    /* flip source image vertically (around the horizontal axis)*/
    HAL_TRANSFORM_FLIP_V    = 0x02,
    /* rotate source image 90 degrees clockwise */
    HAL_TRANSFORM_ROT_90    = 0x04,
    /* rotate source image 180 degrees */
    HAL_TRANSFORM_ROT_180   = 0x03,
    /* rotate source image 270 degrees clockwise */
    HAL_TRANSFORM_ROT_270   = 0x07,
};


struct st_xy_pos {
	int x;
	int y;
};

struct OVERLAY_IMAGE_ST{
    uint32_t            width;
    uint32_t            height;
    uint32_t            format;
    uint32_t            filelen;
    uint32_t            phy_addr;
    uint32_t            stride;
    uint32_t            hnd_id;
    uint32_t            share_fd;
};
struct ion_phys_data {
	int fd_buffer;
	unsigned int phys_h;
	unsigned int phys_l;
	unsigned int size;
};

struct ion_issupport_iommu_data{
	int is_support_iommu;
};


struct st_ch_display_info {
	char*		file_name;
	int			ch;
	uint32_t    ch_cap;
	uint32_t    ch_type;
	uint32_t    comp_mode;
	int			overlay_num;
	int			rotation;
	uint32_t	cfg_disable;
	uint32_t	is_ov_compose;
	uint32_t    has_sec_ovly;
	uint32_t    sec_ovly_surf_num;
	struct ade_rect src_rect;
	struct ade_rect dest_rect;
	struct ade_rect ovly_output_rect;
	uint32_t      has_block_rot;
	int*            block_rot_addr;

    int* ion_vir_addr;
    int  ion_fd;
    struct ion_handle     *ion_hnd;
    int  file_len;
    struct OVERLAY_IMAGE_ST             ov_image;
};

extern int commandfile_test_overlay_display(char* patternIndex, char* pic1_name, char* pic2_name, char* pic3_name,
								  char* pic4_name, char* pic5_name, char* pic6_name,char* ch_list, char* ovly_map,
								   char* dst_rect, char* rotation, char* comp_rot, char* ovly_rect);
extern int comfile_test_overlay_memshare(char* patternIndex, char* pic1_name, char* pic2_name, char* pic3_name,
								  char* pic4_name, char* pic5_name, char* pic6_name,char* ch_list, char* ovly_map,
								   char* dst_rect, char* rotation, char* comp_rot, char* ovly_rect);
extern void cmdfile_test_overlay_mode_switch(char* pattern, char* pic1_name, char* pic2_name, char* pic3_name,
								  char* pic4_name, char* pic5_name, char* pic6_name,char* ch_list, char* ovly_map,
								  char* dst_rect, char* rotation, char* comp_rot, char* ovly_rect);
extern void cmdfile_test_overlay_mode_switch_stop();

extern int codec_mem_alloc_proc(char *eventflag, char *memsize); 
extern int fb_test_pan_display(char *pic_filename, char* bpp,char* flag);
extern void get_ch_compose_info(struct st_ch_display_info  *ch_display_info,int *file_num, char *filename[],char* ch_list,
			 char* ovly_map, char* rotation, char* dst_rect, struct overlay_resource_info* ov_res, char* ovly_rect,int *offline_count, int *block_count);
 								   
extern void get_rect_by_string(struct ade_rect* ch_dst_rect, int ch_num, char* string);
extern void fb_set_ade_power_off(int val);

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif
