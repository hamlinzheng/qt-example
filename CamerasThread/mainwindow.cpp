#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    qDebug() << "MainWindow thread: " << QThread::currentThreadId();

    for (int i = 0; i < 4; ++i) {
        QThread* thread = new QThread;
        m_cameraProbes[i] = new CameraProbe(QString("/dev/video%1").arg(i), thread);
        connect(m_cameraProbes[i], &CameraProbe::imageReady, this, [this, i](const QImage& image) {
            onImageReady(image, i);
        });
        thread->start();
    }

    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::timerInterval);
    timer->start(1000);
}

MainWindow::~MainWindow()
{
    delete ui;
    for (int i = 0; i < 4; ++i) {
        delete m_cameraProbes[i];
    }
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
