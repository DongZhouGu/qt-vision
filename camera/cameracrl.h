#ifndef __CAMERACRL_H
#define __CAMERACRL_H


#include "MvCameraControl.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdio.h>
#include "string.h"


static bool PrintDeviceInfo(MV_CC_DEVICE_INFO* pstMVDevInfo);//显示相机信息

class MVCamera{
    int nRet;//相机操作是否成功
    void* handle;//相机操作句柄
    MV_CC_DEVICE_INFO* pDeviceInfo;//相机设备信息
    MV_FRAME_OUT_INFO_EX stImageInfo;//相机输出帧信息
    unsigned int g_nPayloadSize;//相机抓取图片的数据大小
    unsigned char* pData;//图片数据存储地址
    int grab_state;

    int RGB2BGR();//将RGB转化为BGR--用于cvmat类型
    bool Convert2Mat(cv::Mat &img);//将图像数据转化为cvmat类型
public:
    MVCamera();
    int State();
    int ConnectCamera();//链接相机
    int GetFrame();//相机获取一帧图像
    bool Image(cv::Mat &img);//读取该相机现有的图像
    int CloseCamera();//关闭相机
    ~MVCamera();
};




inline MVCamera::MVCamera(){
    nRet = MV_OK;
    handle = NULL;
    pDeviceInfo = NULL;

    stImageInfo = {0};
    memset(&stImageInfo, 0, sizeof(MV_FRAME_OUT_INFO_EX));
    
    g_nPayloadSize = 0;

    pData = NULL;
    grab_state = 0;
}





inline MVCamera::~MVCamera(){
    if (pData != NULL)
    	free(pData);
    pData = NULL;

    // Destroy handle
    nRet = MV_CC_DestroyHandle(handle);
    if (MV_OK != nRet){
        printf("Destroy Handle fail! nRet [0x%x]\n", nRet);
    }
    handle = NULL;
}




inline int MVCamera::State(){
    return nRet;
}


#endif
