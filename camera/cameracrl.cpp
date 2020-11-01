#include "cameracrl.h"

//获取相机的设备信息
static bool PrintDeviceInfo(MV_CC_DEVICE_INFO *pstMVDevInfo) {

    if (NULL == pstMVDevInfo) {
        printf("The Pointer of pstMVDevInfo is NULL!\n");
        return false;
    }
    if (pstMVDevInfo->nTLayerType == MV_GIGE_DEVICE) {
        int nIp1 = ((pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0xff000000) >> 24);
        int nIp2 = ((pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x00ff0000) >> 16);
        int nIp3 = ((pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x0000ff00) >> 8);
        int nIp4 = (pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x000000ff);

        // print current ip and user defined name
        printf("CurrentIp: %d.%d.%d.%d\n", nIp1, nIp2, nIp3, nIp4);
        printf("UserDefinedName: %s\n\n", pstMVDevInfo->SpecialInfo.stGigEInfo.chUserDefinedName);
    } else if (pstMVDevInfo->nTLayerType == MV_USB_DEVICE) {
        printf("UserDefinedName: %s\n", pstMVDevInfo->SpecialInfo.stUsb3VInfo.chUserDefinedName);
        printf("Serial Number: %s\n", pstMVDevInfo->SpecialInfo.stUsb3VInfo.chSerialNumber);
        printf("Device Number: %d\n\n", pstMVDevInfo->SpecialInfo.stUsb3VInfo.nDeviceNumber);
    } else {
        printf("Not support.\n");
    }

    return true;
}

//--------------------------------------------------------MVCamera私有函数

// int MVCamera::RGB2BGR( unsigned char* pRgbData, unsigned int nWidth, unsigned int nHeight )

//将RGB数据的图像转化为BGR数据的图像
int MVCamera::RGB2BGR() {

    if (NULL == pData) {
        return MV_E_PARAMETER;
    }

    unsigned int nHeight = stImageInfo.nHeight;
    unsigned int nWidth = stImageInfo.nWidth;
    for (unsigned int j = 0; j < nHeight; j++) {
        for (unsigned int i = 0; i < nWidth; i++) {
            unsigned char red = pData[j * (nWidth * 3) + i * 3];
            pData[j * (nWidth * 3) + i * 3] = pData[j * (nWidth * 3) + i * 3 + 2];
            pData[j * (nWidth * 3) + i * 3 + 2] = red;
        }
    }

    return MV_OK;
}

// bool Convert2Mat(MV_FRAME_OUT_INFO_EX* pstImageInfo, unsigned char * pData, cv::Mat &srcImage)

//将获取的图像数据转化为opencv mat数据类型并且存储为"MatImage.bmp"
bool MVCamera::Convert2Mat(cv::Mat &img) {

    if (stImageInfo.enPixelType == PixelType_Gvsp_Mono8) {
        cv::Mat(stImageInfo.nHeight, stImageInfo.nWidth, CV_8UC1, pData).copyTo(img);
    } else if (stImageInfo.enPixelType == PixelType_Gvsp_RGB8_Packed) {
        RGB2BGR();
        cv::Mat(stImageInfo.nHeight, stImageInfo.nWidth, CV_8UC3, pData).copyTo(img);
    } else {
        printf("unsupported pixel format\n");
        return false;
    }

    if (NULL == img.data) {
        return false;
    }

    //     //save converted image in a local file
    //     try {
    // #if defined (VC9_COMPILE)
    //         cvSaveImage("MatImage.bmp", &(IplImage(srcImage)));
    // #else
    //         cv::imwrite("MatImage.bmp", srcImage);
    // #endif
    //     }
    //     catch (cv::Exception& ex) {
    //         fprintf(stderr, "Exception saving image to bmp format: %s\n", ex.what());
    //     }

    //     srcImage.release();

    return true;
}

//--------------------------------------------------------MVCamera公共函数

// int ConnectCamera(void* handle, unsigned int& g_nPayloadSize)

//连接相机
int MVCamera::ConnectCamera() {
    nRet = MV_OK;

    do {
        //列出所有可用的相机
        MV_CC_DEVICE_INFO_LIST stDeviceList;
        memset(&stDeviceList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));
        nRet = MV_CC_EnumDevices(MV_GIGE_DEVICE | MV_USB_DEVICE, &stDeviceList);
        if (MV_OK != nRet) {
            printf("Enum Devices fail! nRet [0x%x]\n", nRet);
            break;
        }

        if (stDeviceList.nDeviceNum > 0) {
            for (unsigned int i = 0; i < stDeviceList.nDeviceNum; i++) {
                printf("[device %d]:\n", i);
                pDeviceInfo = stDeviceList.pDeviceInfo[i];
                if (NULL == pDeviceInfo) {
                    break;
                }
                PrintDeviceInfo(pDeviceInfo);
            }
        } else {
            printf("Find No Devices!\n");
            nRet = -1;
            break;
        }

        //选择一个相机连接
        printf("Please Input camera index:");
        unsigned int nIndex = 0;
        scanf("%d", &nIndex);
        if (nIndex >= stDeviceList.nDeviceNum) {
            printf("Input error!\n");
            nRet = -1;
            break;
        }
        pDeviceInfo = stDeviceList.pDeviceInfo[nIndex];

        //创建所选相机的句柄
        nRet = MV_CC_CreateHandle(&handle, pDeviceInfo);
        if (MV_OK != nRet) {
            printf("Create Handle fail! nRet [0x%x]\n", nRet);
            break;
        }

        //打开相机
        nRet = MV_CC_OpenDevice(handle);
        if (MV_OK != nRet) {
            printf("Open Device fail! nRet [0x%x]\n", nRet);
            break;
        }

        //找到最优的网络传输数据包大小(It only works for the GigE camera)
        if (pDeviceInfo->nTLayerType == MV_GIGE_DEVICE) {
            int nPacketSize = MV_CC_GetOptimalPacketSize(handle);
            //int nPacketSize = 1400;
	    printf("%d\n", nPacketSize);
            if (nPacketSize > 0) {
                //do{
                    nRet = MV_CC_SetIntValue(handle, "GevSCPSPacketSize", nPacketSize);
                //}while(nRet != MV_OK);
                if (nRet != MV_OK) {
                    printf("Warning: Set Packet Size fail nRet [0x%x]!\n", nRet);
                }
                nRet = MV_OK;
            } else {
                printf("Warning: Get Packet Size fail nRet [0x%x]!\n", nPacketSize);
            }
        }

        //关闭触发模式
        nRet = MV_CC_SetEnumValue(handle, "TriggerMode", 0);
        if (MV_OK != nRet) {
            printf("Set Trigger Mode fail! nRet [0x%x]\n", nRet);
            break;
        }

        // Get payload size
        MVCC_INTVALUE stParam;
        memset(&stParam, 0, sizeof(MVCC_INTVALUE));
        nRet = MV_CC_GetIntValue(handle, "PayloadSize", &stParam);
        if (MV_OK != nRet) {
            printf("Get PayloadSize fail! nRet [0x%x]\n", nRet);
            break;
        }
        g_nPayloadSize = stParam.nCurValue;
        pData = (unsigned char *)malloc(sizeof(unsigned char) * (g_nPayloadSize));
         if (pData == NULL) {
            printf("Allocate memory failed.\n");
            nRet = -1;
        }        
    }while (0);

    return nRet;
}

    // int GetFrame(void* handle,unsigned int g_nPayloadSize, cv::Mat &srcImage)

    //相机获取一帧图像
    int MVCamera::GetFrame() {

        nRet = MV_OK;

        do {
            //开始抓图
            if (grab_state == 0){
            	nRet = MV_CC_StartGrabbing(handle);
            	if (MV_OK != nRet) {
            	    printf("Start Grabbing fail! nRet [0x%x]\n", nRet);
            	    break;
            	}
		grab_state = 1;
	    }

            //从相机获取一帧图像timeout=1000ms
            //do{
                nRet = MV_CC_GetOneFrameTimeout(handle, pData, g_nPayloadSize, &stImageInfo, 2000);
            //}while(nRet != MV_OK);
            if (nRet == MV_OK) {
                // printf("Get One Frame: Width[%d], Height[%d], nFrameNum[%d]\n",
                //     stImageInfo.nWidth, stImageInfo.nHeight, stImageInfo.nFrameNum);
            } else {
                printf("No data[0x%x]\n", nRet);
                break;
            }
		
	    // Stop grab image
            //nRet = MV_CC_StopGrabbing(handle);
            //MV_CC_StopGrabbing(handle);
            //if (MV_OK != nRet) {
            //    printf("Stop Grabbing fail! nRet [0x%x]\n", nRet);
            //    break;
            //}

        } while (0);

        return nRet;
    }

    //获取相机现有的图像
    bool MVCamera::Image(cv::Mat & img) {

        //将获取到的图像转化为cvmat类型
        bool bConvertRet = false;
        bConvertRet = Convert2Mat(img);
        // print result
        if (!bConvertRet) {
            printf("OpenCV format convert failed.\n");
        }

        return bConvertRet;
    }

    //关闭相机
    int MVCamera::CloseCamera() {
        do {
            // Stop grab image
	    //do{
            	nRet = MV_CC_StopGrabbing(handle);
            //}while(nRet != MV_OK);
	    //if (MV_OK != nRet) {
            //    printf("Stop Grabbing fail! nRet [0x%x]\n", nRet);
            //    break;
            //}

            // Close device
            nRet = MV_CC_CloseDevice(handle);
            if (MV_OK != nRet) {
                printf("ClosDevice fail! nRet [0x%x]\n", nRet);
                break;
            }

            // Destroy handle
    	    //nRet = MV_CC_DestroyHandle(handle);
            //if (MV_OK != nRet){
            //    printf("Destroy Handle fail! nRet [0x%x]\n", nRet);
            //}
        } while (0);

        return nRet;
    }
