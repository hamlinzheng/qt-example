#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QImage>
#include <QTimer>
#include <QDebug>
#include <QPainter>
#include "cameraprobe.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onFrameReady(const QVideoFrame& frame);
    void onImageReady(const QImage& image);
    void timerInterval();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    Ui::MainWindow *ui;
    CameraProbe *m_cameraProbe;
    QImage m_currentImage;
    int m_frameCaptureCount = 0;
    int m_frameRenderCount = 0;
};

#endif // MAINWINDOW_H
