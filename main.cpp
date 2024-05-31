/// \brief 图像转化示例
/// \example ImageConvert.cpp
// 本Demo为简单演示SDK的使用，没有附加修改相机IP的代码，在运行之前，请使
// 用相机客户端修改相机IP地址的网段与主机的网段一致。                 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include "OPTApi.h" // 你的SDK API
#include <opencv2/opencv.hpp>  

int main()
{
	printf("Loding......\n");
	int ret = OPT_OK;
	unsigned int cameraIndex = 0; // 初始化相机编号从0开始计数
	OPT_HANDLE devHandle = NULL;
	OPT_Frame frame; //声明图像变量
	//
	OPT_PixelConvertParam stPixelConvertParam; //声明转码后的图像数据
	unsigned char* pDstBuf = NULL;//初始化声明转码后的图像内存
	unsigned int nDstBufSize = 0; //初始化声明转码后的图像总大小
	//
	int keynum = 0;
	bool isGrab = true;

	OPT_DeviceList deviceInfoList;// 发现设备 
	ret = OPT_EnumDevices(&deviceInfoList, interfaceTypeAll);
	if (OPT_OK != ret)
	{
		printf("Enumeration devices failed! ErrorCode[%d]\n", ret);
		getchar();
		return -1;
	}
	//
	if (deviceInfoList.nDevNum < 1)
	{
		printf("no camera\n");
		getchar();
		return -1;
	}
	//
	ret = OPT_CreateHandle(&devHandle, modeByIndex, (void*)&cameraIndex);// 创建设备句柄
	if (OPT_OK != ret)
	{
		printf("Create devHandle failed! ErrorCode[%d]\n", ret);
		return -1;
	}
	//
	ret = OPT_Open(devHandle);// 打开相机 
	if (OPT_OK != ret)
	{
		printf("Open camera failed! ErrorCode[%d]\n", ret);
		return -1;
	}
	//
	ret = OPT_StartGrabbing(devHandle);	// 开始拉流 
	if (OPT_OK != ret)
	{
		printf("Start grabbing failed! ErrorCode[%d]\n", ret);
		return -1;
	}
	printf("Star Play......\n");

	while (isGrab) {
		
		ret = OPT_GetFrame(devHandle, &frame, 500);// 获取一帧图像
		if (OPT_OK != ret)
		{
			printf("Get frame failed! ErrorCode[%d]\n", ret);
			return -1;
		}

		//
		nDstBufSize = sizeof(unsigned char) * frame.frameInfo.width * frame.frameInfo.height * 3;
		pDstBuf = (unsigned char*)malloc(nDstBufSize);
		// 图像转换成BGR8
		memset(&stPixelConvertParam, 0, sizeof(stPixelConvertParam));
		stPixelConvertParam.nWidth = frame.frameInfo.width;
		stPixelConvertParam.nHeight = frame.frameInfo.height;
		stPixelConvertParam.ePixelFormat = frame.frameInfo.pixelFormat;
		stPixelConvertParam.pSrcData = frame.pData;
		stPixelConvertParam.nSrcDataLen = frame.frameInfo.size;
		stPixelConvertParam.nPaddingX = frame.frameInfo.paddingX;
		stPixelConvertParam.nPaddingY = frame.frameInfo.paddingY;
		stPixelConvertParam.eBayerDemosaic = demosaicNearestNeighbor;
		stPixelConvertParam.eDstPixelFormat = gvspPixelBGR8;//转换的格式
		stPixelConvertParam.pDstBuf = pDstBuf;
		stPixelConvertParam.nDstBufSize = nDstBufSize;
		//
		ret = OPT_PixelConvert(devHandle, &stPixelConvertParam);
		//
		cv::Mat PMat(frame.frameInfo.height, frame.frameInfo.width, CV_8UC3, pDstBuf);//为了显示图片 先改为mat类型
		cv::imshow("opt", PMat);

		if (cv::waitKey(1) > 0){
			isGrab = false;
			free(pDstBuf);
			delete[] PMat;
			break;
			//pDstBuf = NULL;
		}
		//

		ret = OPT_ReleaseFrame(devHandle, &frame);// 释放图像缓存
		if (OPT_OK != ret)
		{
			printf("Release frame failed! ErrorCode[%d]\n", ret);
			return -1;
		}
	}
	
	ret = OPT_StopGrabbing(devHandle);	// 停止拉流 
	if (OPT_OK != ret)
	{
		printf("Stop grabbing failed! ErrorCode[%d]\n", ret);
		return -1;
	}

	
	ret = OPT_Close(devHandle);	// 关闭相机
	if (OPT_OK != ret)
	{
		printf("Close camera failed! ErrorCode[%d]\n", ret);
		return -1;
	}

	if (devHandle != NULL)
	{
		// 销毁设备句柄
		// Destroy Device Handle
		OPT_DestroyHandle(devHandle);
	}

	return 0;
}
