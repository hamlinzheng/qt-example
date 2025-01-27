#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    qDebug() << "MainWindow thread: " << QThread::currentThreadId();

    QThread* thread = new QThread;
    m_cameraProbe = new CameraProbe(QString("/dev/video0"), thread);

    // connect(m_cameraProbe, &CameraProbe::frameReady, this, &MainWindow::onFrameReady);
    connect(m_cameraProbe, &CameraProbe::imageReady, this, &MainWindow::onImageReady);
    
    // This style makes the slot function not executed in the main thread
    // connect(m_cameraProbe, &CameraProbe::frameReady, [this](const QVideoFrame& frame) {
    //     onFrameReady(frame);
    // });

    thread->start();

    // 
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::timerInterval);
    timer->start(1000);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_cameraProbe;
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
