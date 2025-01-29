#include "cameraprobe.h"

CameraProbe::CameraProbe(const QString& deviceName, QThread *thread)
{
    m_camera = new QCamera(deviceName.toStdString().c_str(), this);
    m_videoProbe = new QVideoProbe(this);

    if (m_videoProbe->setSource(m_camera)) {
        connect(m_videoProbe, &QVideoProbe::videoFrameProbed, this, &CameraProbe::onVideoFrameProbed);
    } else {
        qWarning() << "Failed to set video probe source.";
    }

    if (thread != nullptr) { // Move Worker to thread if thread is not nullptr
        this->moveToThread(thread);
        connect(thread, &QThread::started, this, &CameraProbe::start);
    }
}

CameraProbe::~CameraProbe()
{
    m_camera->stop();
    delete m_camera;
    delete m_videoProbe;
}

void CameraProbe::start()
{
    m_camera->start();
}

void CameraProbe::onVideoFrameProbed(const QVideoFrame& frame)
{
    // qDebug() << "onVideoFrameProbed thread: " << QThread::currentThreadId();
    if (!frame.isValid()) {
        return;
    }
    emit frameReady(frame);

    if (isSignalConnected(QMetaMethod::fromSignal(&CameraProbe::imageReady))) {
        // qDebug() << "Converting frame to image.";
        QVideoFrame cloneFrame(frame);
        cloneFrame.map(QAbstractVideoBuffer::ReadOnly);
        QImage image = cloneFrame.image();
        cloneFrame.unmap();
        emit imageReady(image);
    }
}
