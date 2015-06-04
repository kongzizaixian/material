LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES:= \
	$(TOPDIR)kernel/drivers/video/hisi/hi6220 \
	
LOCAL_SRC_FILES := v8r1_commandfile_test.c v8r1_commandfile_overlay_display.c  v8r1_commandfile_open_fb.c  v8r1_commandfile_comm.c  v8r1_commandfile_pan_display.c v8r1_commandfile_overlay_switch.c
LOCAL_MODULE = test-v8r1-ade-cmd
LOCAL_MODULE_TAGS := eng
LOCAL_FORCE_STATIC_EXECUTABLE := true
LOCAL_STATIC_LIBRARIES := libc
include $(BUILD_EXECUTABLE)
