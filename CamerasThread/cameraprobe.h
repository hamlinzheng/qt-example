#ifndef CAMERAPROBE_H
#define CAMERAPROBE_H

#include <QThread>
#include <QCamera>
#include <QVideoProbe>
#include <QImage>
#include <QCameraInfo>
#include <QDebug>
#include <QVideoFrame>
#include <functional>

class CameraProbe : public QObject
{
    Q_OBJECT
public:
    CameraProbe(const QString& deviceName, QThread *thread = nullptr);
    ~CameraProbe();

    void connectImageReady(const std::function<void(const QImage&)>& slot);

signals:
    void frameReady(const QVideoFrame& frame);
    void imageReady(const QImage& image);

private slots:
    void start();
    void onVideoFrameProbed(const QVideoFrame& frame);

private:
    QCamera *m_camera;
    QVideoProbe *m_videoProbe;
};

#endif // CAMERAPROBE_H
