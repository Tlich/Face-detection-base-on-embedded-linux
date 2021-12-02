#ifndef V4L2CAM_H
#define V4L2CAM_H

#include "common.h"

#include <QObject>
#include <QTextStream>
#include <QVector>
#include <QImage>

#include <linux/videodev2.h> //inside have v4l2 strcut
#include <sys/ioctl.h>
#include <sys/mman.h>

#include <iostream>
#include <sstream>
#include <errno.h>
#include <fcntl.h>
#include <iosfwd>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <malloc.h>

/*#ifdef QtV4l2
#if defined(V4L2CAM_LIBRARY)
#  define V4L2CAMSHARED_EXPORT Q_DECL_EXPORT
#else
#  define V4L2CAMSHARED_EXPORT Q_DECL_IMPORT
#endif
#endif*/


typedef struct {
    int exposureType;
    int exposure;
    int gain;
    int whiteBalance;
    int brightness;
    int saturation;
    int contrast;
} v4l2Set;

typedef struct {
    int index;
    char* description;
    QVector<char *> framesize;
} formatType;

typedef struct {
    int imgwidth;
    int imgheight;
    uchar* addr;
} v4l2pic;

#ifdef QtV4l2
class V4L2CAMSHARED_EXPORT V4l2Cam {
#else
class V4l2Cam {
#endif
public:
    V4l2Cam(uint videoNum);
    V4l2Cam();
    ~V4l2Cam();
//*******************************
    QImage getpic();
    int convert_yuv_to_rgb_pixel(int y, int u, int v);
    int yuv2rgb(unsigned char *yuv, unsigned char *rgb, unsigned int width, unsigned int height);
    QImage pixmap;
//*******************************
    int OPenCamera(int videoNum);
    v4l2pic *Camera_Read();
    v4l2pic pic2send;
    char * devName;
    void Camera_Set(v4l2Set set);

    uint32_t Width, Height, Fps;
    QVector<formatType> FormatType;

    /*设置像素格式。一般的USB摄像头都会支持YUYV，有些还支持其他的格式。
     * 通过前一步对摄像头所支持像素格式查询，下面需要对格式进行设置。
     * 命令为VIDIOC_S_FMT，通过结构体v4l2_format把图像的像素格式设置为V4L2_PIX_FMT_YUYV，高度和宽度设置为IMAGEHEIGHT和IMAGEWIDTH。
     * 一般情况下一个摄像头所支持的格式是不可以随便更改的，我尝试把把一个只支持YUYV和MJPEG的摄像头格式改为RGB24或者JPEG，都没有成功。*/
    int setPixelformat(uint32_t width = 320, uint32_t height = 240,
                       uint32_t format = V4L2_PIX_FMT_YUYV, //V4L2_PIX_FMT_JPEG,
                       uint32_t field = V4L2_FIELD_NONE);

    int setFps(uint32_t fps = 30, bool HDmode = false); //fps:30   HDmode:false

    /*申请缓存区使用参数VIDIOC_REQBUFS和结构体v4l2_requestbuffers。
     * v4l2_requestbuffers结构中定义了缓存的数量，系统会据此申请对应数量的视频缓存。*/
    int memsetCam();

    int queryam();  //列举摄像头 列举摄像头所支持像素格式  列举对应的图片格式所支持的分辨率
    int releaseMen();
    void CloseDev();

    int setBrightness(int value);
    int setContrast(int value);
    int setHUE(int value);
    int setSaturation(int value);
    int setGAMMA(int value);
    int setWhiteBalance(int value);
    int setAutoWhiteBalance(bool value);
    int setGain(int value);
    int setAutoGain(bool value);
    int setExposure(int value);
    int setAutoExposure(bool value);

    int getBrightness();
    int getContrast();
    int getHUE();
    int getSaturation();
    int getGAMMA();
    int getWhiteBalance();
    int getAutoWhiteBalance();
    int getGain();
    int getAutoGain();
    int getExposure();
    int getAutoExposure();

private:
    char cameraaddr[15];
    int fd;
    struct v4l2_buffer buf;
    v4l2pic v4l2img; //v4l2的图片？
    uint32_t imgWidth, imgHeight;

};

#endif // V4L2CAM_H
