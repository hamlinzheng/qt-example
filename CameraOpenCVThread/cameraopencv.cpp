#include "cameraopencv.h"

CameraOpenCV::CameraOpenCV(const QString& device, const QString& format, const QString& resolution, const QString& framerate, QThread *thread)
{
    QStringList resList = resolution.split("*");
    if (resList.size() != 2) {
        qWarning() << "CameraOpenCV: Invalid resolution format. Expected 'width*height'.";
        exit(1);
    }
    QString width = resList[0];
    QString height = resList[1];
    
    // std::string pipeline = "v4l2src device=/dev/video0 min-buffers=2 io-mode=mmap ! video/x-raw,format=NV12,width=1280,height=720,framerate=25/1 ! videoconvert ! video/x-raw,format=BGR ! appsink drop=true sync=false";
    std::string pipeline = "v4l2src device=" + device.toStdString() + " min-buffers=2 io-mode=mmap ! \
                            video/x-raw,format=" + format.toStdString() + ",width=" + width.toStdString() + ",height=" + height.toStdString() + ",framerate=" + framerate.toStdString() + "/1" + " ! \
                            videoconvert ! video/x-raw,format=BGR ! appsink drop=true sync=false";
    cap.open(pipeline, cv::CAP_GSTREAMER);
    // cap.open(0, cv::CAP_V4L2);
    if (cap.isOpened()) {
        isCapturing = true;
    }
    else
    {
        qWarning() << "CameraOpenCV: Can't open camera " << device;
        exit(1);
    }

    std::string backendName = cap.getBackendName();
    qDebug() << "CameraOpenCV: Backend name: " << QString::fromStdString(backendName);
    // qDebug() << "CameraOpenCV: OpenCV Build Information: " << QString::fromStdString(cv::getBuildInformation());
    
    if (thread != nullptr) { // Move Worker to thread if thread is not nullptr
        this->moveToThread(thread);
        thread->setObjectName("CameraOpenCVThread");
        connect(thread, &QThread::started, this, &CameraOpenCV::start);
    }
}

CameraOpenCV::~CameraOpenCV()
{
}

void CameraOpenCV::start()
{
    qDebug() << "CameraOpenCV: Start capturing... Thread: " << QThread::currentThreadId();
    cv::Mat frame;
    while (isCapturing) {
        if (cap.read(frame)) {
            // TYPE: CV_8UC1: 0 {NV12(1CH*1280*1080)}, CV_8UC3: 16 {BGR(3CH*1280*720)}
            // qDebug() << "CameraOpenCV: Frame type: " << frame.type() << ", channels: " << frame.channels() << ", size: " << frame.size().width << "x" << frame.size().height;

            // cv::cvtColor(frame, frame, cv::COLOR_YUV2BGR_NV12);
            emit frameReady(frame);
            
            if (isSignalConnected(QMetaMethod::fromSignal(&CameraOpenCV::imageReady))) {
                // qDebug() << "Converting frame to image.";
                QImage image(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_BGR888);
                QImage copiedImage = image.copy();
                emit imageReady(copiedImage);
            }
        }
    }
}

