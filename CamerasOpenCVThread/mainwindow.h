#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QImage>
#include <QTimer>
#include <QDebug>
#include <QPainter>
#include "cameraopencv.h"

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
    void onCvFrameReady(const cv::Mat &frame);
    void onFrameReady(const QVideoFrame& frame);
    void onImageReady(const QImage& image, int cameraIndex);
    void timerInterval();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    Ui::MainWindow *ui;
    CameraOpenCV *m_cameras[4];
    QImage m_currentImages[4];
    int m_frameCaptureCounts[4] = {0, 0, 0, 0};
    int m_frameRenderCounts[4] = {0, 0, 0, 0};
};

#endif // MAINWINDOW_H
