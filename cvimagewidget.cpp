#include "cvimagewidget.h"

CVImageWidget::CVImageWidget(QWidget *parent) : QWidget(parent)
{

}

void CVImageWidget::showImage(const cv::Mat &image)
{
    switch(image.type())
    {
    case CV_8UC1:
        cvtColor(image, temp_, CV_GRAY2RGB);
        break;
    case CV_8UC3:
        cvtColor(image, temp_, CV_BGR2RGB);
        break;
    }

    assert(temp_.isContinuous());
    qimage_ = QImage(temp_.data, temp_.cols, temp_.rows, temp_.cols * 3,
                     QImage::Format_RGB888);

    this->setFixedSize(image.cols, image.rows);
    repaint();
}

void CVImageWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.drawImage(QPoint(0,0), qimage_);
    painter.end();
}
