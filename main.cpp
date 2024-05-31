/// \brief ͼ��ת��ʾ��
/// \example ImageConvert.cpp
// ��DemoΪ����ʾSDK��ʹ�ã�û�и����޸����IP�Ĵ��룬������֮ǰ����ʹ
// ������ͻ����޸����IP��ַ������������������һ�¡�                 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include "OPTApi.h" // ���SDK API
#include <opencv2/opencv.hpp>  

int main()
{
	printf("Loding......\n");
	int ret = OPT_OK;
	unsigned int cameraIndex = 0; // ��ʼ�������Ŵ�0��ʼ����
	OPT_HANDLE devHandle = NULL;
	OPT_Frame frame; //����ͼ�����
	//
	OPT_PixelConvertParam stPixelConvertParam; //����ת����ͼ������
	unsigned char* pDstBuf = NULL;//��ʼ������ת����ͼ���ڴ�
	unsigned int nDstBufSize = 0; //��ʼ������ת����ͼ���ܴ�С
	//
	int keynum = 0;
	bool isGrab = true;

	OPT_DeviceList deviceInfoList;// �����豸 
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
	ret = OPT_CreateHandle(&devHandle, modeByIndex, (void*)&cameraIndex);// �����豸���
	if (OPT_OK != ret)
	{
		printf("Create devHandle failed! ErrorCode[%d]\n", ret);
		return -1;
	}
	//
	ret = OPT_Open(devHandle);// ����� 
	if (OPT_OK != ret)
	{
		printf("Open camera failed! ErrorCode[%d]\n", ret);
		return -1;
	}
	//
	ret = OPT_StartGrabbing(devHandle);	// ��ʼ���� 
	if (OPT_OK != ret)
	{
		printf("Start grabbing failed! ErrorCode[%d]\n", ret);
		return -1;
	}
	printf("Star Play......\n");

	while (isGrab) {
		
		ret = OPT_GetFrame(devHandle, &frame, 500);// ��ȡһ֡ͼ��
		if (OPT_OK != ret)
		{
			printf("Get frame failed! ErrorCode[%d]\n", ret);
			return -1;
		}

		//
		nDstBufSize = sizeof(unsigned char) * frame.frameInfo.width * frame.frameInfo.height * 3;
		pDstBuf = (unsigned char*)malloc(nDstBufSize);
		// ͼ��ת����BGR8
		memset(&stPixelConvertParam, 0, sizeof(stPixelConvertParam));
		stPixelConvertParam.nWidth = frame.frameInfo.width;
		stPixelConvertParam.nHeight = frame.frameInfo.height;
		stPixelConvertParam.ePixelFormat = frame.frameInfo.pixelFormat;
		stPixelConvertParam.pSrcData = frame.pData;
		stPixelConvertParam.nSrcDataLen = frame.frameInfo.size;
		stPixelConvertParam.nPaddingX = frame.frameInfo.paddingX;
		stPixelConvertParam.nPaddingY = frame.frameInfo.paddingY;
		stPixelConvertParam.eBayerDemosaic = demosaicNearestNeighbor;
		stPixelConvertParam.eDstPixelFormat = gvspPixelBGR8;//ת���ĸ�ʽ
		stPixelConvertParam.pDstBuf = pDstBuf;
		stPixelConvertParam.nDstBufSize = nDstBufSize;
		//
		ret = OPT_PixelConvert(devHandle, &stPixelConvertParam);
		//
		cv::Mat PMat(frame.frameInfo.height, frame.frameInfo.width, CV_8UC3, pDstBuf);//Ϊ����ʾͼƬ �ȸ�Ϊmat����
		cv::imshow("opt", PMat);

		if (cv::waitKey(1) > 0){
			isGrab = false;
			free(pDstBuf);
			delete[] PMat;
			break;
			//pDstBuf = NULL;
		}
		//

		ret = OPT_ReleaseFrame(devHandle, &frame);// �ͷ�ͼ�񻺴�
		if (OPT_OK != ret)
		{
			printf("Release frame failed! ErrorCode[%d]\n", ret);
			return -1;
		}
	}
	
	ret = OPT_StopGrabbing(devHandle);	// ֹͣ���� 
	if (OPT_OK != ret)
	{
		printf("Stop grabbing failed! ErrorCode[%d]\n", ret);
		return -1;
	}

	
	ret = OPT_Close(devHandle);	// �ر����
	if (OPT_OK != ret)
	{
		printf("Close camera failed! ErrorCode[%d]\n", ret);
		return -1;
	}

	if (devHandle != NULL)
	{
		// �����豸���
		// Destroy Device Handle
		OPT_DestroyHandle(devHandle);
	}

	return 0;
}
