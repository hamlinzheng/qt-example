#ifndef CAMERAOPENCV_H
#define CAMERAOPENCV_H

#include <QThread>
#include <QCamera>
#include <QVideoProbe>
#include <QImage>
#include <QDebug>
#include <opencv2/opencv.hpp>

class CameraOpenCV : public QObject
{
    Q_OBJECT
public:
    explicit CameraOpenCV(const QString& device, const QString& format, const QString& resolution, const QString& framerate, QThread *thread = nullptr);
    ~CameraOpenCV();

signals:
    void frameReady(const cv::Mat &frame);
    void imageReady(const QImage& image);

private slots:
    void start();

private:
    cv::VideoCapture cap;
    bool isCapturing = false;
};

#endif // CAMERAOPENCV_H
