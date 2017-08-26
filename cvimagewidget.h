#ifndef CVIMAGEWIDGET_H
#define CVIMAGEWIDGET_H

#include <QWidget>
#include <QImage>
#include <QPainter>
#include <opencv2/opencv.hpp>

class CVImageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CVImageWidget(QWidget *parent = nullptr);
    QSize sizeHint() const {return qimage_.size(); }
    QSize minimumSizeHint() const {return qimage_.size(); }
protected:
    void paintEvent(QPaintEvent *event);
signals:

public slots:
    void showImage(const cv::Mat& image);

private:
    cv::Mat temp_;
    QImage qimage_;

};

#endif // CVIMAGEWIDGET_H
