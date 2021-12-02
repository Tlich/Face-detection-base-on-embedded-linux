#include "camera_widget.h"
#include "ui_camera_widget.h"


Camera_Widget::Camera_Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Camera_Widget)
{
    ui->setupUi(this);
    cam = new V4l2Cam();
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(video_show()));
    connect(ui->open_camera, SIGNAL(clicked()), this, SLOT(ClickButton()));
    connect(ui->close_camera, SIGNAL(clicked()), this, SLOT(CloseButton()));
    connect(ui->face_recognition, SIGNAL(clicked()), this, SLOT(Face_reg()));
    connect(ui->off_face, SIGNAL(clicked()), this, SLOT(Off_face()));
    ui->close_camera->setEnabled(false);
    ui->face_recognition->setEnabled(false);
    ui->off_face->setEnabled(false);

}

Camera_Widget::~Camera_Widget()
{
    cam->releaseMen();
    cam->CloseDev();
    delete ui;
}

void Camera_Widget::ClickButton()
{
    ui->surface_video->setText("start v4l2 camera");
    cam->OPenCamera(1);
    cam->setPixelformat(320, 240, V4L2_PIX_FMT_YUYV, V4L2_FIELD_NONE);
    cam->setFps(30, false);
    if(!cam->memsetCam())
    {
        timer->start(1);
    }

    ui->open_camera->setEnabled(false);
    ui->close_camera->setEnabled(true);
    ui->face_recognition->setEnabled(true);
    ui->off_face->setEnabled(true);
    //cam->start();
}

void Camera_Widget::CloseButton()
{
    timer->stop();
    cam->releaseMen();
    cam->CloseDev();

    ui->surface_video->setText("close dev");
    ui->open_camera->setEnabled(true);
    ui->face_recognition->setEnabled(false);
    ui->off_face->setEnabled(false);
    //qDebug() << "main_thread";
}

void Camera_Widget::video_show()
{
    if(Flag == 0)
    {
        pic = cam->get_pic();
    }
    else
    {
        pic = cam->Recog_face();
    }
    ui->surface_video->setPixmap(QPixmap::fromImage(pic));
}

void Camera_Widget::Face_reg()
{
    Flag = 1;
    timer->start(1);
}

void Camera_Widget::Off_face()
{
    Flag = 0;
    timer->start(1);
}
