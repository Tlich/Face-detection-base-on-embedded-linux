#ifndef CAMERA_WIDGET_H
#define CAMERA_WIDGET_H

#include <QWidget>
#include <QApplication>
#include <QTimer>
#include "v4l2cam.h"

namespace Ui {
class Camera_Widget;
}

class Camera_Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Camera_Widget(QWidget *parent = nullptr);
    ~Camera_Widget();
    QImage pic;
    QTimer *timer;
    int Flag = 0;
public slots:
    void ClickButton();
    void CloseButton();
    void video_show();
    void Face_reg();
    void Off_face();
private:
    Ui::Camera_Widget *ui;
    V4l2Cam* cam;
};

#endif // CAMERA_WIDGET_H
