#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QLayout>
#include <QScrollArea>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QStandardPaths>
#include <QImageReader>
#include <QImageWriter>
#include <QMessageBox>
#include <QPixmap>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>

using namespace cv;

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
    void loadImage();
    void convertImage();

    void on_dial_sliderMoved(int position);

    void on_horizontalSlider_sliderMoved(int position);

    void on_pushButton_3_clicked(bool checked);

    void on_pushButton_6_clicked(bool checked);

    void on_pushButton_7_clicked(bool checked);

    void on_pushButton_4_clicked(bool checked);

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void on_checkBox_clicked(bool checked);

    void on_checkBox_2_clicked(bool checked);

    void on_checkBox_3_clicked(bool checked);

    void on_listWidget_currentRowChanged(int currentRow);

    void on_pushButton_5_clicked();

    void on_horizontalSlider_2_sliderMoved(int position);

private:
    QImage MatRGB2QImage(const cv::Mat3b &src);
    QImage MatConverted2QImage(const cv::Mat3b &src);
    void convertYUVToRGB(cv::Mat &src);
    void convertRGBToYUV(cv::Mat &src, cv::Mat &original);
    bool chromaSubsampling(cv::Mat &src, const int a, const int b);
    void DCT_start(cv::Mat &src);
    bool isOutputRGB;
    bool isDCTOn;
    bool isIDCTOn;
    bool isQuantizationOn;
    int colorIsolationStatus;
    int presetRow;
    int quantizationCoefficient;
    Ui::MainWindow *ui;
    int cSampleA, cSampleB;
    Mat cvImg, convertedImg;


};

#endif // MAINWINDOW_H
