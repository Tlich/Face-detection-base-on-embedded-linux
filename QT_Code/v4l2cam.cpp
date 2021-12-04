#include "v4l2cam.h"



V4l2Cam::V4l2Cam(uint videoNum) : fd(-1)
{
    sprintf(cameraaddr,"/dev/video%d",videoNum);
    if ((fd = open(cameraaddr, O_RDWR)) == -1){
        qDebug() << "open failed" << cameraaddr;
        return;
    }
    queryam();
}

V4l2Cam::V4l2Cam()
{

}

V4l2Cam::~V4l2Cam()
{
    if (-1 != fd)
    {
        close(fd);
    }
}

int V4l2Cam::OPenCamera(int videoNum) //open camera
{
    qDebug() << "OPenCamera" << cameraaddr;
    sprintf(cameraaddr,"/dev/video%d", videoNum);
    if ((fd = open(cameraaddr, O_RDWR)) == -1){
        qDebug() << "open failed" << cameraaddr;
        return -1;
    }
    queryam(); //列举摄像头 列举摄像头所支持像素格式  列举对应的图片格式所支持的分辨率
    return 0;
}

int V4l2Cam::queryam() //列举摄像头 列举摄像头所支持像素格式  列举对应的图片格式所支持的分辨率
{
    struct v4l2_capability cap;   //cap结构体
    memset(&cap, 0, sizeof(struct v4l2_capability));  //为cap分配内存
    //通过调用IOCTL函数和接口命令VIDIOC_QUERYCAP查询摄像头的信息，结构体v4l2_capability中有包括驱动名称driver、card、bus_info、version以及属性capabilities。
    if (ioctl(fd, VIDIOC_QUERYCAP, &cap) == -1){
        qDebug() << "ERROR:unable Querying Capabilities";
        return -1;
    }
    devName = ((char*)cap.card);
    #ifdef Debug
    qDebug() << "get all camera dev";
    qDebug() << "dev name:" << devName;
    #endif
    struct v4l2_fmtdesc fmtdesc; //存的是摄像头支持的传输格式
    struct v4l2_frmsizeenum frmsize; //存的是摄像头，对应的图片格式所支持的分辨率
    fmtdesc.index = 0;         //camera index 0
    fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    while(ioctl(fd, VIDIOC_ENUM_FMT, &fmtdesc) != -1)  //列举摄像头所支持像素格式。
    {
        formatType type;
        type.index = fmtdesc.index++;
        type.description = ((char*)fmtdesc.description);
        frmsize.pixel_format = fmtdesc.pixelformat;
        frmsize.index = 0;
        #ifdef Debug
        qDebug() << "get Camera format: VIDIOC_ENUM_FMT";
        qDebug() << "camera index:" << type.index;
        qDebug() << "fmtdesc.description:" << fmtdesc.description;
        qDebug() << "fmtdesc.pixelformat:" << fmtdesc.pixelformat;;
        #endif
        while (ioctl(fd, VIDIOC_ENUM_FRAMESIZES, &frmsize) >= 0) //存的是摄像头，对应的图片格式所支持的分辨率
        {
           #ifdef Debug
           qDebug() << "get photos format: VIDIOC_ENUM_FRAMESIZES";
           qDebug() << "format index:" << frmsize.index+1;
           qDebug() << "frmsize.discrete.width" << frmsize.discrete.width;
           qDebug() << "frmsize.discrete.height:" << frmsize.discrete.height;
           #endif
           frmsize.index++;
        }
        FormatType.push_back(type);
    }
    return 0;
}

int V4l2Cam::setPixelformat(uint32_t width, uint32_t height, uint32_t format, uint32_t field){ //设置像素格式。
   /*width、height、format：格式MJPE，YUYV 、field*/
    struct v4l2_format fmt;
    memset(&fmt, 0, sizeof(struct v4l2_format));

    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    v4l2img.imgwidth = width;
    v4l2img.imgheight = height;

    fmt.fmt.pix.width = width;
    fmt.fmt.pix.height = height;

    imgWidth = width;
    imgHeight = height;

    Width = fmt.fmt.pix.width;
    Height = fmt.fmt.pix.height;

    fmt.fmt.pix.pixelformat = format;  //设置格式
    fmt.fmt.pix.field = field;         //设置？？

    if (ioctl(fd, VIDIOC_S_FMT, &fmt) == -1){
        qDebug() << "Setting Pixel Format failed [VIDIOC_S_FMT]" << fd;
        return -1;
    }

    if (ioctl(fd, VIDIOC_G_FMT, &fmt) == -1){  //再通过命令VIDIOC_G_FMT将摄像头设置读取回来
        qDebug() << "Setting Pixel Format failed [VIDIOC_G_FMT]" << fd;
        return -1;
    }
    qDebug() << "setok " << fd << v4l2img.imgwidth << v4l2img.imgheight;
    #ifdef Debug
    qDebug() << "v4l2img.imgwidth:" << v4l2img.imgwidth;
    qDebug() << "v4l2img.imgheight:" << v4l2img.imgheight;
    #endif
    return 0;
}

int V4l2Cam::setFps(uint32_t fps, bool HDmode){ //获取或设置数据流的参数，一般设置帧率等参数
    struct v4l2_streamparm str;
    memset(&str, 0, sizeof(struct v4l2_streamparm));

    str.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    str.parm.capture.capability = V4L2_CAP_TIMEPERFRAME;
    if(HDmode)   //是否为高清模式。如果是：则设置为：V4L2_MODE_HIGHQUALITY。 高清模式会牺牲其它信息。通常设置为0。
        str.parm.capture.capturemode = V4L2_MODE_HIGHQUALITY;
        str.parm.capture.timeperframe.numerator = 1;
        str.parm.capture.timeperframe.denominator = fps;
    if (ioctl(fd, VIDIOC_S_PARM, &str) == -1){
        qDebug() << "Setting FPS failed" ;
        return -1;
    }
    if (ioctl(fd, VIDIOC_G_PARM, &str) == -1){
        qDebug() << "Setting FPS failed" ;
        return -1;
    }
    qDebug() << "Setting FPS OK" ;
    Fps = str.parm.capture.timeperframe.denominator;
    return 0;
}
/*申请缓存区使用参数VIDIOC_REQBUFS和结构体v4l2_requestbuffers。
 * v4l2_requestbuffers结构中定义了缓存的数量，系统会据此申请对应数量的视频缓存。
 */
int V4l2Cam::memsetCam()
{
    struct v4l2_requestbuffers req;
    req.count = 1;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    if (ioctl(fd, VIDIOC_REQBUFS, &req) == -1) //申请缓存区
    {
        qDebug() << "Requesting Buffer error";
        return -1;
    }
    #ifdef Debug
    qDebug() << "Requesting Buffe OK";
    #endif
    //用来判断内存有没有问题的
    v4l2img.addr = (uchar*)malloc(req.count * sizeof(*v4l2img.addr)); //allocate memory
    if(!v4l2img.addr){
        qDebug() << "Out of memory";
        return -1;
    }

    for(uint32_t n_buffers = 0;n_buffers < req.count; n_buffers++)
    {
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = n_buffers;
        if(ioctl(fd, VIDIOC_QUERYBUF, &buf) == -1){  //VIDIOC_QUERYBUF: 把VIDIOC_REQBUFS中分配的数据缓存转换成物理地址
            qDebug() << "Querying Buffer error";
            return -1;
        }
        #ifdef Debug
        qDebug() << "VIDIOC_QUERYBUF:把VIDIOC_REQBUFS中分配的数据缓存转换成物理地址";
        #endif

        //mmap系统调用并不是完全为了用于共享内存而设计的。它本身提供了不同于一般对普通文件的访问方式，进程可以像读写内存一样对普通文件的操作。
        v4l2img.addr = (uchar*)mmap (NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
        if(v4l2img.addr == MAP_FAILED)
        {
            qDebug() << "Buffer map error";
            return -1;
        }
        #ifdef Debug
        qDebug() << "mmap将文件映射到调用进程的地址空间里";
        #endif
    }

    for (uint32_t n_buffers = 0; n_buffers < req.count; n_buffers++)
    {
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = n_buffers;
        if (ioctl(fd, VIDIOC_QBUF, &buf) == -1)  //把数据从缓存中读取出来
        {
            qDebug() << "Querying Buffer error";
            return -1;
        }
    }

    //之后就可以开始采集视频了。使用命令VIDIOC_STREAMON。
    enum v4l2_buf_type type;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (ioctl(fd, VIDIOC_STREAMON, &type) == -1){ //开始视频显示函数
        qDebug() << "Stream on error" ;
        return -1;
    }

    Camera_Read();
    return 0;
}

v4l2pic* V4l2Cam::Camera_Read()
{
    //qDebug() << "start read frame";
    ioctl(fd,VIDIOC_QBUF,&buf); //VIDIOC_QBUF：把数据从缓存中读取出来
    buf.index = 0;
    memcpy(&pic2send, &v4l2img, sizeof(v4l2img));
    ioctl(fd, VIDIOC_DQBUF, &buf);//VIDIOC_DQBUF：把数据放回缓存队列
    return &pic2send;
}

QImage V4l2Cam::get_pic()
{
    QTime timedebuge;
    timedebuge.start();//开始计时

    Camera_Read();
    uchar *rgb24 = new uchar[v4l2img.imgwidth * v4l2img.imgheight * 3]();
    yuv2rgb(v4l2img.addr, (unsigned char *)rgb24, v4l2img.imgwidth, v4l2img.imgheight);
    pixmap = QImage((unsigned char*)rgb24, v4l2img.imgwidth, v4l2img.imgheight, v4l2img.imgwidth * 3, QImage::Format_RGB888);

    qDebug()<<"pic："<< timedebuge.elapsed() <<"ms";
    return pixmap;
}

QImage V4l2Cam::Recog_face()
{
    QTime timedebuge;
    timedebuge.start();//开始计时

    Camera_Read();

    qDebug()<<"read time:"<< timedebuge.elapsed() <<"ms";

    uchar *rgb24 = new uchar[v4l2img.imgwidth * v4l2img.imgheight * 3]();
    yuv2rgb(v4l2img.addr, (unsigned char *)rgb24, v4l2img.imgwidth, v4l2img.imgheight);
    Mat pic = Mat(v4l2img.imgheight, v4l2img.imgwidth, CV_8UC3, rgb24);

    qDebug()<<"rgb2Mat："<< timedebuge.elapsed() <<"ms";

    Mat gray_image;
    vector<Rect> faces;
    CascadeClassifier face_cascade;
#ifdef arm
    if( !face_cascade.load("/usr/opencv_arm/share/OpenCV/haarcascades/haarcascade_frontalface_alt2.xml") )
#else
    if( !face_cascade.load("/usr/local/share/OpenCV/haarcascades/haarcascade_frontalface_alt2.xml") )
#endif
    {
        qDebug() << "can not load the file...\n";
        return pixmap;
    }
    cvtColor( pic, gray_image, CV_RGB2GRAY ); //to gray

    qDebug()<<"mat2gray:"<< timedebuge.elapsed() <<"ms";

    equalizeHist( gray_image, gray_image ); //直方图均衡化

    qDebug()<<"直方图均衡化:"<< timedebuge.elapsed() <<"ms";

#ifdef arm
    face_cascade.detectMultiScale( gray_image, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(100, 100) );
#else
    face_cascade.detectMultiScale( gray_image, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(50, 50) );
#endif
    //框选出脸部区域

    qDebug()<<"face regoc:"<< timedebuge.elapsed() <<"ms";

    for (int i = 0; i < faces.size(); i++)
    {
        RNG rng(i);
        Scalar color = Scalar(0,255,0);//RGB //Scalar(rng.uniform(0,255), rng.uniform(0,255), 20);
        rectangle(pic, faces[static_cast<int>(i)], color, 2, 8, 0);
    }
    //imshow("face", pic);waitKey(0);
    pixmap = QImage((const unsigned char*)pic.data, pic.cols, pic.rows, pic.step, QImage::Format_RGB888);
    //pixmap = QImage((unsigned char*)rgb24, v4l2img.imgwidth, v4l2img.imgheight, v4l2img.imgwidth * 3, QImage::Format_RGB888);
    //QImage qim((const unsigned char*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);

    qDebug()<<"pic+opencv："<< timedebuge.elapsed() <<"ms";

    return pixmap;
}

void V4l2Cam::closeThread()
{

}

void V4l2Cam::run()
{
    while(1)
    {
        qDebug() << "thread" << QThread::currentThreadId();
        sleep(1);
    }
}

int V4l2Cam::yuv2rgb(unsigned char *yuv, unsigned char *rgb, unsigned int width, unsigned int height)
{
    unsigned int in, out = 0;
    unsigned int pixel_16;
    unsigned char pixel_24[3];
    unsigned int pixel32;
    int y0, u, y1, v;
    for(in = 0; in < width * height * 2; in += 4) {
        pixel_16 = yuv[in + 3] << 24 | yuv[in + 2] << 16 | yuv[in + 1] <<  8 | yuv[in + 0];
        y0 = (pixel_16 & 0x000000ff);
        u  = (pixel_16 & 0x0000ff00) >>  8;
        y1 = (pixel_16 & 0x00ff0000) >> 16;
        v  = (pixel_16 & 0xff000000) >> 24;
        pixel32 = convert_yuv_to_rgb_pixel(y0, u, v);
        pixel_24[0] = (pixel32 & 0x000000ff);
        pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
        pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;
        rgb[out++] = pixel_24[0];
        rgb[out++] = pixel_24[1];
        rgb[out++] = pixel_24[2];
        pixel32 = convert_yuv_to_rgb_pixel(y1, u, v);
        pixel_24[0] = (pixel32 & 0x000000ff);
        pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
        pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;
        rgb[out++] = pixel_24[0];
        rgb[out++] = pixel_24[1];
        rgb[out++] = pixel_24[2];
    }
    return 0;
}

int V4l2Cam::convert_yuv_to_rgb_pixel(int y, int u, int v)  //yuv -> rgb
{
    unsigned int pixel32 = 0;
    unsigned char *pixel = (unsigned char *)&pixel32;
    int r, g, b;
    r = y + (1.370705 * (v-128));
    g = y - (0.698001 * (v-128)) - (0.337633 * (u-128));
    b = y + (1.732446 * (u-128));
    if(r > 255) r = 255;
    if(g > 255) g = 255;
    if(b > 255) b = 255;
    if(r < 0) r = 0;
    if(g < 0) g = 0;
    if(b < 0) b = 0;
    pixel[0] = r * 220 / 256;
    pixel[1] = g * 220 / 256;
    pixel[2] = b * 220 / 256;
    return pixel32;
}

int V4l2Cam::yuv2bgr(unsigned char *yuv, unsigned char *rgb, unsigned int width, unsigned int height)
{
    unsigned int in, out = 0;
    unsigned int pixel_16;
    unsigned char pixel_24[3];
    unsigned int pixel32;
    int y0, u, y1, v;
    for(in = 0; in < width * height * 2; in += 4) {
        pixel_16 = yuv[in + 3] << 24 | yuv[in + 2] << 16 | yuv[in + 1] <<  8 | yuv[in + 0];
        y0 = (pixel_16 & 0x000000ff);
        u  = (pixel_16 & 0x0000ff00) >>  8;
        y1 = (pixel_16 & 0x00ff0000) >> 16;
        v  = (pixel_16 & 0xff000000) >> 24;
        pixel32 = convert_yuv_to_bgr_pixel(y0, u, v);
        pixel_24[0] = (pixel32 & 0x000000ff);
        pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
        pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;
        rgb[out++] = pixel_24[0];
        rgb[out++] = pixel_24[1];
        rgb[out++] = pixel_24[2];
        pixel32 = convert_yuv_to_bgr_pixel(y1, u, v);
        pixel_24[0] = (pixel32 & 0x000000ff);
        pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
        pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;
        rgb[out++] = pixel_24[0];
        rgb[out++] = pixel_24[1];
        rgb[out++] = pixel_24[2];
    }
    return 0;
}

int V4l2Cam::convert_yuv_to_bgr_pixel(int y, int u, int v)  //yuv -> bgr
{
    unsigned int pixel32 = 0;
    unsigned char *pixel = (unsigned char *)&pixel32;
    int r, g, b;
    r = y + (1.370705 * (v-128));
    g = y - (0.698001 * (v-128)) - (0.337633 * (u-128));
    b = y + (1.732446 * (u-128));
    if(r > 255) r = 255;
    if(g > 255) g = 255;
    if(b > 255) b = 255;
    if(r < 0) r = 0;
    if(g < 0) g = 0;
    if(b < 0) b = 0;
    pixel[0] = b * 220 / 256;
    pixel[1] = g * 220 / 256;
    pixel[2] = r * 220 / 256;
    return pixel32;
}

int V4l2Cam::releaseMen()
{
    if(buf.length > 1)
    {
        munmap(v4l2img.addr, buf.length);
        close(fd);
        return 0;
    }
    else
        return -1;
}

void V4l2Cam::CloseDev()
{
    qDebug() << "Close Device";
    munmap(v4l2img.addr, buf.length);
    close(fd);
    fd = -1;
}




int V4l2Cam::getBrightness()  //获得亮度数据
{
    struct v4l2_control ctrl;
    int ret;
    ctrl.id = V4L2_CID_BRIGHTNESS;
    ret = ioctl(fd, VIDIOC_G_CTRL, &ctrl);
    if(ret != 0) return -255;
    return ctrl.value;
}

int V4l2Cam::getContrast()  //获取对比度？？
{
    struct v4l2_control ctrl;
    int ret;
    ctrl.id = V4L2_CID_CONTRAST;
    ret = ioctl(fd, VIDIOC_G_CTRL, &ctrl);
    if(ret != 0) return -255;
    return ctrl.value;
}

int V4l2Cam::getHUE()
{
    struct v4l2_control ctrl;
    int ret;
    ctrl.id = V4L2_CID_HUE;
    ret = ioctl(fd, VIDIOC_G_CTRL, &ctrl);
    if(ret != 0) return -255;
    return ctrl.value;
}

int V4l2Cam::getSaturation()
{
    struct v4l2_control ctrl;
    int ret;
    ctrl.id = V4L2_CID_SATURATION;
    ret = ioctl(fd, VIDIOC_G_CTRL, &ctrl);
    if(ret != 0) return -255;
    return ctrl.value;
}

int V4l2Cam::getGAMMA()
{
    struct v4l2_control ctrl;
    int ret;
    ctrl.id = V4L2_CID_GAMMA;
    ret = ioctl(fd, VIDIOC_G_CTRL, &ctrl);
    if(ret != 0) return -255;
    return ctrl.value;
}

int V4l2Cam::getWhiteBalance()
{
    struct v4l2_control ctrl;
    int ret;
    ctrl.id = V4L2_CID_WHITENESS;
    ret = ioctl(fd, VIDIOC_G_CTRL, &ctrl);
    if(ret != 0) return -255;
    return ctrl.value;
}

int V4l2Cam::getAutoWhiteBalance()
{
    struct v4l2_control ctrl;
    int ret;
    ctrl.id = V4L2_CID_AUTO_WHITE_BALANCE;
    ret = ioctl(fd, VIDIOC_G_CTRL, &ctrl);
    if(ret != 0) return -255;
    return (bool)ctrl.value;
}

int V4l2Cam::getGain()
{
    struct v4l2_control ctrl;
    int ret;
    ctrl.id = V4L2_CID_GAIN;
    ret = ioctl(fd, VIDIOC_G_CTRL, &ctrl);
    if(ret != 0) return -255;
    return ctrl.value;
}

int V4l2Cam::getAutoGain()
{
    struct v4l2_control ctrl;
    int ret;
    ctrl.id = V4L2_CID_AUTOGAIN;
    ret = ioctl(fd, VIDIOC_G_CTRL, &ctrl);
    if(ret != 0) return -255;
    return (bool)ctrl.value;
}

int V4l2Cam::getExposure()
{
    struct v4l2_control ctrl;
    int ret;
    ctrl.id = V4L2_CID_EXPOSURE;
    ret = ioctl(fd, VIDIOC_G_CTRL, &ctrl);
    if(ret != 0) return -255;
    return ctrl.value;
}

int V4l2Cam::getAutoExposure()
{
    struct v4l2_control ctrl;
    int ret;
    ctrl.id = V4L2_CID_EXPOSURE_AUTO;
    ret = ioctl(fd, VIDIOC_G_CTRL, &ctrl);
    if(ret != 0) return -255;
    if(ctrl.value == 1)
        return false;
    else
        return true;
}

int V4l2Cam::setBrightness(int value)
{
    struct v4l2_control ctrl;
    int ret;
    ctrl.id = V4L2_CID_BRIGHTNESS;
    ctrl.value = value;
    ret = ioctl(fd, VIDIOC_S_CTRL, &ctrl);
    if(ret != 0) return -1;
    return 0;
}

int V4l2Cam::setContrast(int value)
{
    struct v4l2_control ctrl;
    int ret;
    ctrl.id = V4L2_CID_CONTRAST;
    ctrl.value = value;
    ret = ioctl(fd, VIDIOC_S_CTRL, &ctrl);
    if(ret != 0) return -1;
    return 0;
}

int V4l2Cam::setHUE(int value)
{
    struct v4l2_control ctrl;
    int ret;
    ctrl.id = V4L2_CID_HUE;
    ctrl.value = value;
    ret = ioctl(fd, VIDIOC_S_CTRL, &ctrl);
    if(ret != 0) return -1;
    return 0;
}

int V4l2Cam::setSaturation(int value)
{
    struct v4l2_control ctrl;
    int ret;
    ctrl.id = V4L2_CID_SATURATION;
    ctrl.value = value;
    ret = ioctl(fd, VIDIOC_S_CTRL, &ctrl);
    if(ret != 0) return -1;
    return 0;
}

int V4l2Cam::setGAMMA(int value)
{
    struct v4l2_control ctrl;
    int ret;
    ctrl.id = V4L2_CID_GAMMA;
    ctrl.value = value;
    ret = ioctl(fd, VIDIOC_S_CTRL, &ctrl);
    if(ret != 0) return -1;
    return 0;
}

int V4l2Cam::setWhiteBalance(int value)
{
    struct v4l2_control ctrl;
    int ret;
    ctrl.id = V4L2_CID_WHITENESS;
    ctrl.value = value;
    ret = ioctl(fd, VIDIOC_S_CTRL, &ctrl);
    if(ret != 0) return -1;
    return 0;
}

int V4l2Cam::setAutoWhiteBalance(bool value)
{
    struct v4l2_control ctrl;
    int ret;
    ctrl.id = V4L2_CID_AUTO_WHITE_BALANCE;
    ctrl.value = value;
    ret = ioctl(fd, VIDIOC_S_CTRL, &ctrl);
    if(ret != 0) return -1;
    return 0;
}

int V4l2Cam::setGain(int value)
{
    struct v4l2_control ctrl;
    int ret;
    ctrl.id = V4L2_CID_GAIN;
    ctrl.value = value;
    ret = ioctl(fd, VIDIOC_S_CTRL, &ctrl);
    if(ret != 0) return -1;
    return 0;
}

int V4l2Cam::setAutoGain(bool value)
{
    struct v4l2_control ctrl;
    int ret;
    ctrl.id = V4L2_CID_AUTOGAIN;
    ctrl.value = value;
    ret = ioctl(fd, VIDIOC_S_CTRL, &ctrl);
    if(ret != 0) return -1;
    return 0;
}

int V4l2Cam::setExposure(int value)
{
    struct v4l2_control ctrl;
    int ret;
    ctrl.id = V4L2_CID_EXPOSURE;
    ctrl.value = value;
    ret = ioctl(fd, VIDIOC_S_CTRL, &ctrl);
    if(ret != 0) return -1;
    return 0;
}

int V4l2Cam::setAutoExposure(bool value)
{
    struct v4l2_control ctrl;
    int ret;
    ctrl.id = V4L2_CID_EXPOSURE_AUTO_PRIORITY;
    ctrl.value = value;
    ret = ioctl(fd, VIDIOC_S_CTRL, &ctrl);
    if(ret != 0) return -1;
    return 0;
}


