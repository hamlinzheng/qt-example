#include <QCoreApplication>
#include <QThread>
#include <QDebug>
#include <opencv2/opencv.hpp>

class CaptureThread : public QThread
{
    Q_OBJECT
public:
    CaptureThread(QObject *parent = nullptr) : QThread(parent) {
        qDebug() << "CaptureThread created in thread:" << QThread::currentThread();
    }

protected:
    void run() override {
        std::string pipeline = "v4l2src device=/dev/video0 min-buffers=2 io-mode=mmap ! \
                                video/x-raw,format=NV12,width=1280,height=720,framerate=30/1 ! \
                                videoconvert ! video/x-raw, format=(string)BGR ! appsink";
        
        cv::VideoCapture cap(pipeline, cv::CAP_GSTREAMER); // Time-consuming
        if (!cap.isOpened()) {
            qWarning() << "Failed to open camera with pipeline:" << QString::fromStdString(pipeline);
            return;
        }

        cv::Mat frame;
        while (true) {
            if (cap.read(frame)) {
                // qDebug() << "CaptureThread running in thread:" << QThread::currentThread();
                emit frameReady(frame);
            } else {
                qWarning() << "Failed to read frame from camera.";
                // break;
            }
        }
    }

signals:
    void frameReady(const cv::Mat &frame);
};

class DisplayHandler : public QObject
{
    Q_OBJECT
public:
    explicit DisplayHandler(QObject *parent = nullptr) : QObject(parent) {
        qDebug() << "DisplayHandler created in thread:" << QThread::currentThread();
        cv::namedWindow("Video Stream", cv::WINDOW_AUTOSIZE);
    }

public slots:
    void handleFrame(const cv::Mat &frame) {
        // qDebug() << "Frame received in thread:" << QThread::currentThread();
        // qDebug() << "Frame size:" << frame.size().width << "x" << frame.size().height;
        cv::imshow("Video Stream", frame);
    }
};

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qDebug() << "Main thread:" << QThread::currentThread();

    qRegisterMetaType<cv::Mat>("cv::Mat");
    // Set environment variable GST_VIDEO_CONVERT_USE_RGA=1
    qputenv("GST_VIDEO_CONVERT_USE_RGA", "1");
    // Set environment variable GST_VIDEO_FLIP_USE_RGA=1
    qputenv("GST_VIDEO_FLIP_USE_RGA", "1");

    CaptureThread *captureThread = new CaptureThread();
    DisplayHandler *displayHandler = new DisplayHandler();
    QObject::connect(captureThread, &CaptureThread::frameReady,
                     displayHandler, &DisplayHandler::handleFrame);

    captureThread->start();

    return a.exec();
}

#include "main.moc"
