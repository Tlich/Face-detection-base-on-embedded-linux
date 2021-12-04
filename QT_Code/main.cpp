#include "camera_widget.h"

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/videoio.hpp>
#include <opencv/cv.h>
using namespace cv;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Camera_Widget w;
    w.show();
    return a.exec();
}
