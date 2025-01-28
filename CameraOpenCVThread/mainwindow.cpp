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

    QThread* thread = new QThread;
    // Using OpenCV capture
    m_cameraOpenCV = new CameraOpenCV("/dev/video0", "NV12", "1280*720", "25", thread);
    // connect(m_cameraOpenCV, &CameraOpenCV::frameReady, this, &MainWindow::onCvFrameReady);
    connect(m_cameraOpenCV, &CameraOpenCV::imageReady, this, &MainWindow::onImageReady);

    thread->start();

    // 
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

void MainWindow::onImageReady(const QImage& image)
{
    // qDebug() << "onImageReady thread: " << QThread::currentThreadId();
    m_currentImage = image;
    m_frameCaptureCount++;
    update();
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    // qDebug() << "paintEvent thread: " << QThread::currentThreadId();
    QMainWindow::paintEvent(event);
    if (!m_currentImage.isNull()) {
        QPainter painter(this);
        QRect targetRect(0, 0, this->width(), this->height());
        painter.drawImage(targetRect, m_currentImage);
        m_frameRenderCount++;
    }
}

void MainWindow::timerInterval()
{
    qDebug() << "Frame capture: " << m_frameCaptureCount << " Frame render: " << m_frameRenderCount;
    m_frameCaptureCount = 0;
    m_frameRenderCount = 0;
}
