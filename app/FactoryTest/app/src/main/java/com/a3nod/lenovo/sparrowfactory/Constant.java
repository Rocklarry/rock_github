package com.a3nod.lenovo.sparrowfactory;

import android.os.Environment;

/**
 * Description
 * Created by aaa on 2017/11/14.
 */

public class Constant
{
    public static final String TEST_LOG_PATH= Environment.getExternalStorageDirectory().getAbsolutePath()+"/runin/";

    public static final String PROJECT_DIR="/sdcard/sparrow";

    public static final String RUNIN_DIR=PROJECT_DIR+"/runin";
    public static final String RUNIN_REPORT_PATH=RUNIN_DIR+"/runin_report.json";

    public static final String MMI_DIR=PROJECT_DIR+"/mmi";
    public static final String MMI_REPORT_PATH=MMI_DIR+"/mmi_report.json";

    public static final String PCBA_DIR=PROJECT_DIR+"/pcba";
    public static final String PCBA_REPORT_PATH=PCBA_DIR+"/pcba_report.json";

    public static final String CAMERA_DIR=PROJECT_DIR+"/camera";
    public static final String PHOTO_PATH=CAMERA_DIR+"/test.bmp";

    public static final String PATH_PCBA_SN="/sdcard/sparrow/pcba_sn";
    public static final String PATH_MMI_SN="/sdcard/sparrow/mmi_sn";
    public static final String PATH_SN="/sdcard/sparrow/sn";


    public static final String SP_KEY_NAME="runintest";

    public static final String SP_KEY_BOOT_TIME="boot_time";



    public static final String INTENT_KEY_IS_AUTO="is_auto";
    public static final String INTENT_KEY_REBOOT_SUCCESS="reboot_success";
    public static final String INTENT_KEY_TEST_ID="test_id";
    public static final String INTENT_KEY_TEST_RESULT="test_result";
    public static final String INTENT_KEY_START_AUTO_RUNIN="start_auto_runin";
    public static final String INTENT_KEY_ERROR_RESTART ="error_restart";
    public static final String INTENT_KEY_ERROR_MSG ="error_message";
    public static final String INTENT_KEY_TEST_MODEL ="test_model";
    public static final int INTENT_VALUE_TEST_MODEL_RUNIN =2;
    public static final int INTENT_VALUE_TEST_MODEL_MMI =1;
    public static final int INTENT_VALUE_TEST_MODEL_PCBA =0;

    public static final String INTENT_KEY_RUNIN_TEST_ITEM ="runin_test_item";
    public static final String INTENT_KEY_RUNIN_TEST_RESULT ="runin_item_test_result";
    public static final String INTENT_KEY_ACTION_SHOW_SN ="action_show_sn";

    public static final String INTENT_ACTION_SWITCH_MIC="SWITCH_MIC";
    public static final String INTENT_KEY_MIC_GROUP="MIC_GROUP";
    public static final String INTENT_ACTION_RESULT="action_test_result";


    public static final String INTENT_ACTION_START_RUNIN="action_start_runin_test";
    public static final String INTENT_KEY_RUNIN_DURATION="runin_duration";
    public static final String INTENT_ACTION_STOP_RUNIN="action_stop_runin_test";



    public static final boolean RUNIN_TYPE_SINGLE=false;
    public static final boolean RUNIN_TYPE_AUTO=true;

    public static final String ACTION_NEXT_TEST="action_reboot_result";

    public static final String ACTION_START_PCBA="com.pcba_test";
    public static final String ACTION_WRITE_MMI_SN="com.mmi.test";
    public static final String ACTION_WRITE_SN="com.write.sn";
    public static final String ACTION_OPEN_CAMERA ="com.sparrow.camera.take_picture";
    public static final String ACTION_TAKE_PICTURE ="com.sparrow.camera.capture";

    public static final String ACTION_START_RUNIN ="action_start_runin";
    public static final String ACTION_STOP_RUNIN ="action_stop_runin";

    public static final String ACTION_VOLUME_UP ="action_volume_up";
    public static final String ACTION_VOLUME_DOWN ="action_volume_down";
    public static final String INTENT_KEY_CURRENT_VOLUME ="current_volume";
    public static final String INTENT_KEY_VOLUME_TYPE ="volume_type";



    public static final int RUNIN_STATUS_RUNNING=-3;
    public static final int RUNIN_STATUS_CANCEL=-2;
    public static final int RUNIN_STATUS_SUCCESS=-1;
    public static final String SP_KEY_IS_AUTORUNIN="is_auto_runin";
    public static final String SP_KEY_RUNIN_TEST_ITEM="auto_runin_test_item";
    public static final String SP_KEY_RUNIN_TEST_START_TIME="runin_test_end_time";
    public static final String SP_KEY_RUNIN_TEST_END_TIME="runin_test_start_time";
    public static final String SP_KEY_RUNIN_DURATION="runin_test_duration";

    public static final String SP_KEY_RUNIN_ERROR="runin_error";

    public static final String SP_KEY_RUNIN_CURRENT_TIME="runin_test_current_time";

    public static final String SP_KEY_RUNIN_CURRENT_TEST_ID="runin_current_test_item_id";
    public static final String SP_KEY_CPU_TEST_DURATION="cpu_test_duration";
    public static final String SP_KEY_MEMORY_TEST_DURATION="memory_test_duration";
    public static final String SP_KEY_EMMC_TEST_DURATION="emmc_test_duration";
    public static final String SP_KEY_LCD_TEST_DURATION="lcd_test_duration";
    public static final String SP_KEY_2D_TEST_DURATION="2d_test_duration";
    public static final String SP_KEY_3D_TEST_DURATION="3d_test_duration";
    public static final String SP_KEY_AUDIO_TEST_DURATION="audio_test_duration";
    public static final String SP_KEY_VIDEO_TEST_DURATION="video_test_duration";
    public static final String SP_KEY_CAMERA_TEST_DURATION="camera_test_duration";
    public static final String SP_KEY_REBOOT_TEST_DURATION="reboot_test_duration";

    public static final String SP_KEY_REBOOT_TIMES="reboot_test_times";
    public static final String SP_KEY_REBOOT_CURRENT_TIME="reboot_test_current_time";



    public static final String SP_KEY_CPU_TEST_SUCCESS="cpu_test_success";
    public static final String SP_KEY_MEMORY_TEST_SUCCESS="memory_test_success";
    public static final String SP_KEY_EMMC_TEST_SUCCESS="emmc_test_success";
    public static final String SP_KEY_LCD_TEST_SUCCESS="lcd_test_success";
    public static final String SP_KEY_2D_TEST_SUCCESS="2d_test_success";
    public static final String SP_KEY_3D_TEST_SUCCESS="3d_test_success";
    public static final String SP_KEY_MIC_TEST_SUCCESS="mic_test_success";
    public static final String SP_KEY_AUDIO_TEST_SUCCESS="audio_test_success";
    public static final String SP_KEY_VIDEO_TEST_SUCCESS ="video_test_success";
    public static final String SP_KEY_CAMERA_TEST_SUCCESS="camera_test_success";
    public static final String SP_KEY_SLEEP_TEST_SUCCESS="sleep_test_success";
    public static final String SP_KEY_REBOOT_TEST_SUCCESS="reboot_test_success";

    public static final String SP_KEY_CPU_TEST_FAIL="cpu_test_fail";
    public static final String SP_KEY_MEMORY_TEST_FAIL="memory_test_fail";
    public static final String SP_KEY_EMMC_TEST_FAIL="emmc_test_fail";
    public static final String SP_KEY_LCD_TEST_FAIL="lcd_test_fail";
    public static final String SP_KEY_2D_TEST_FAIL="2d_test_fail";
    public static final String SP_KEY_3D_TEST_FAIL="3d_test_fail";
    public static final String SP_KEY_MIC_TEST_FAIL="mic_test_fail";
    public static final String SP_KEY_AUDIO_TEST_FAIL="audio_test_fail";
    public static final String SP_KEY_VIDEO_TEST_FAIL ="video_test_fail";
    public static final String SP_KEY_CAMERA_TEST_FAIL="camera_test_fail";
    public static final String SP_KEY_SLEEP_TEST_FAIL="sleep_test_fail";
    public static final String SP_KEY_REBOOT_TEST_FAIL="reboot_test_fail";


    public static final String SP_KEY_CPU_TEST_REMARK="cpu_test_remark";
    public static final String SP_KEY_MEMORY_TEST_REMARK="memory_test_remark";
    public static final String SP_KEY_EMMC_TEST_REMARK="emmc_test_remark";
    public static final String SP_KEY_LCD_TEST_REMARK="lcd_test_remark";
    public static final String SP_KEY_2D_TEST_REMARK="2d_test_remark";
    public static final String SP_KEY_3D_TEST_REMARK="3d_test_remark";
    public static final String SP_KEY_MIC_TEST_REMARK="mic_test_remark";
    public static final String SP_KEY_AUDIO_TEST_REMARK="audio_test_remark";
    public static final String SP_KEY_VIDEO_TEST_REMARK="video_test_remark";
    public static final String SP_KEY_CAMERA_TEST_REMARK="camera_test_remark";
    public static final String SP_KEY_SLEEP_TEST_REMARK="sleep_test_remark";
    public static final String SP_KEY_REBOOT_TEST_REMARK="reboot_test_remark";

    public static Boolean TEST_TYPE_MMI1 = false;
    public static Boolean TEST_TYPE_MMI2 = false;
    public static Boolean TEST_TYPE_MMI_AUDO= false;

}
