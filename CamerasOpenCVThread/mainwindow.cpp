#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    qRegisterMetaType<cv::Mat>("cv::Mat");
    // Set environment variable GST_VIDEO_CONVERT_USE_RGA=1
    qputenv("GST_VIDEO_CONVERT_USE_RGA", "1");
    // Set environment variable GST_VIDEO_FLIP_USE_RGA=1
    qputenv("GST_VIDEO_FLIP_USE_RGA", "1");

    qDebug() << "MainWindow thread: " << QThread::currentThreadId();

    for (int i = 0; i < 4; ++i) {
        QThread* thread = new QThread;
        m_cameras[i] = new CameraOpenCV(QString("/dev/video%1").arg(i), "NV12", "1280*720", "25", thread);
        connect(m_cameras[i], &CameraOpenCV::imageReady, this, [this, i](const QImage& image) { onImageReady(image, i); });
        thread->start();
    }

    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::timerInterval);
    timer->start(1000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onCvFrameReady(const cv::Mat &frame)
{
    // qDebug() << "onCvFrameReady thread: " << QThread::currentThreadId();
}

void MainWindow::onFrameReady(const QVideoFrame& frame)
{
    // qDebug() << "onFrameReady thread: " << QThread::currentThreadId();
}

void MainWindow::onImageReady(const QImage& image, int cameraIndex)
{
    m_currentImages[cameraIndex] = image;
    m_frameCaptureCounts[cameraIndex]++;
    if (cameraIndex ==0 ) {
        update();
    }
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QMainWindow::paintEvent(event);
    QPainter painter(this);
    int width = this->width() / 2;
    int height = this->height() / 2;

    for (int i = 0; i < 4; ++i) {
        if (!m_currentImages[i].isNull()) {
            QRect targetRect((i % 2) * width, (i / 2) * height, width, height);
            painter.drawImage(targetRect, m_currentImages[i]);
            m_frameRenderCounts[i]++;
        }
    }
}

void MainWindow::timerInterval()
{
    for (int i = 0; i < 4; ++i) {
        qDebug() << "Camera" << i << "Frame capture:" << m_frameCaptureCounts[i] << "Frame render:" << m_frameRenderCounts[i];
        m_frameCaptureCounts[i] = 0;
        m_frameRenderCounts[i] = 0;
    }
}
