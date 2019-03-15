#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "utils.h"

static cv::Matx<double,8,8> dct_T;

static double lquantization[8][8] = {
        {16, 11, 10, 16, 24, 40, 51, 61},
        {12, 12, 14, 19, 26, 58, 60, 55},
        {14, 13, 16, 24, 40, 57, 69, 56},
        {14, 17, 22, 29, 51, 87, 80, 62},
        {18, 22, 37, 56, 68,109,103, 77},
        {24, 35, 55, 64, 81,104,113, 92},
        {49, 64, 78, 87,103,121,120,101},
        {72, 92, 95, 98,112,100,103, 99}
        };

static double cquantization[8][8] = {
        {17,18,24,47,99,99,99,99},
        {18,21,26,66,99,99,99,99},
        {24,26,56,99,99,99,99,99},
        {47,99,99,99,99,99,99,99},
        {99,99,99,99,99,99,99,99},
        {99,99,99,99,99,99,99,99},
        {99,99,99,99,99,99,99,99},
        {99,99,99,99,99,99,99,99}
        };

cv::Matx<double,8,8> applyDCT(cv::Matx<double,8,8> matrix) {
    return dct_T*matrix*dct_T.t();
}

cv::Matx<double,8,8> applyIDCT(cv::Matx<double,8,8> matrix) {
    return dct_T.t()*matrix*dct_T;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->horizontalSlider->setRange(0, 1);
    ui->horizontalSlider->setValue(0);
    ui->label->setScaledContents(true);
    ui->dial->setRange(0, 3);
    ui->dial->setValue(1);
    ui->horizontalSlider_2->setRange(1, 100);
    ui->horizontalSlider_2->setValue(10);
    quantizationCoefficient = 1;
    ui->pushButton_3->setCheckable(true);
    ui->pushButton_6->setCheckable(true);
    ui->pushButton_7->setCheckable(true);
    ui->pushButton_4->setCheckable(true);
    ui->pushButton_4->setChecked(true);
    ui->checkBox->setChecked(true);
    ui->checkBox_2->setChecked(true);
    ui->checkBox_3->setChecked(true);
    isDCTOn = true;
    isIDCTOn = true;
    isQuantizationOn = true;
    colorIsolationStatus = 4;
    cSampleA = 2;
    cSampleB = 2;
    presetRow = 0;
    isOutputRGB = true;
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            if(i == 0 ) dct_T(i,j) = 0.5/(sqrt(2));
            else dct_T(i,j) = 0.5*cos(((2*j+1)*i*M_PI)/16); // book page 246
        }
    }
    cvImg = cv::imread("defaultImage.png" , IMREAD_COLOR);
    QImage qImage = MatRGB2QImage(cvImg);
    ui->label->setPixmap(QPixmap::fromImage(qImage));
    // title
    setWindowTitle(tr("Image Converter"));

    connect(ui->pushButton, SIGNAL(clicked()),
            this, SLOT (loadImage()));


}

MainWindow::~MainWindow()
{
    delete ui;
}

// slot to open a file dialog and load an image to a class member cvImg and display it on a QLabel
void MainWindow::loadImage()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Load Image"), "",
        tr("All Files (*)"));
    cvImg = cv::imread(fileName.toStdString(), IMREAD_COLOR);
    QImage qImage = MatRGB2QImage(cvImg);
    ui->label->setPixmap(QPixmap::fromImage(qImage));
    QString string = QString("Original dimensions: %1 x %2")
                    .arg(cvImg.cols).arg(cvImg.rows);
    ui->label_5->setText(string);

    int n_rows = floor((cvImg.rows)/8) * 8;
    int n_cols = floor((cvImg.cols)/8) * 8;

    QString string2 = QString("Updated dimensions: %1 x %2")
                    .arg(n_cols).arg(n_rows);
    ui->label_6->setText(string2);
    double f1 = ((650*100/n_cols));
    double f2 = ((650*100/n_rows));
    QString string3 = QString("Zoom: %1 % x %2 %")
                    .arg(f1).arg(f2);
    ui->label_8->setText(string3);


}

// slot to calculate Y channel of cvImg and display it on a QLabel
void MainWindow::convertImage()
{
        if (cvImg.empty())
        {
            return;
        }
        convertedImg.create(cvImg.size(), CV_8UC3);

        convertRGBToYUV(convertedImg, cvImg);

        if(!chromaSubsampling(convertedImg, cSampleA, cSampleB)) printf("Error: Invalid subsampling values!\n");

        if(isDCTOn) DCT_start(convertedImg);

        if(isOutputRGB) convertYUVToRGB(convertedImg);

        QImage qImage = MatConverted2QImage(convertedImg);
        ui->label->setPixmap(QPixmap::fromImage(qImage));
}

void MainWindow::DCT_start(cv::Mat &src) {
    int n_rows = floor((src.rows)/8) * 8;
    int n_cols = floor((src.cols)/8) * 8;

    if(isDCTOn) {
        for(int i = 0; i < n_rows; i = i+8) {
            for(int j = 0; j < n_cols; j = j+8) {
                cv::Matx<double,8,8> matrixY;
                cv::Matx<double,8,8> matrixU;
                cv::Matx<double,8,8> matrixV;
                for(int u = 0; u < 8; u++) {
                    for(int x = 0; x < 8; x++) {
                        matrixY(u, x) = src.at<cv::Vec3b>(i+u, j+x)[0];
                        matrixU(u, x) = src.at<cv::Vec3b>(i+u, j+x)[1];
                        matrixV(u, x) = src.at<cv::Vec3b>(i+u, j+x)[2];
                    }
                 }
                 cv::Matx<double,8,8> newY = applyDCT(matrixY);
                 cv::Matx<double,8,8> newU = applyDCT(matrixU);
                 cv::Matx<double,8,8> newV = applyDCT(matrixV);
                 if(isQuantizationOn) {
                     for(int u = 0; u < 8; u++) {
                         for(int x = 0; x < 8; x++) {
                             newY(u,x) = newY(u,x)/(lquantization[u][x] * quantizationCoefficient/10);
                             newY(u,x) = round(newY(u,x));
                             newY(u,x) = newY(u,x)*(lquantization[u][x] * quantizationCoefficient/10);

                             newU(u,x) = newU(u,x)/(cquantization[u][x] * quantizationCoefficient/10);
                             newU(u,x) = round(newU(u,x));
                             newU(u,x) = newU(u,x)*(cquantization[u][x] * quantizationCoefficient/10);

                             newV(u,x) = newV(u,x)/(cquantization[u][x] * quantizationCoefficient/10);
                             newV(u,x) = round(newV(u,x));
                             newV(u,x) = newV(u,x)*(cquantization[u][x] * quantizationCoefficient/10);
                         }
                     }
                 }
                 if(isIDCTOn) {
                     cv::Matx<double,8,8> newYY = applyIDCT(newY);
                     cv::Matx<double,8,8> newUU = applyIDCT(newU);
                     cv::Matx<double,8,8> newVV = applyIDCT(newV);
                     for(int u = 0; u < 8; u++) {
                         for(int x = 0; x < 8; x++) {
                            src.at<cv::Vec3b>(i+u,j+x)[0] = newYY(u, x);
                            src.at<cv::Vec3b>(i+u,j+x)[1] = newUU(u, x);
                            src.at<cv::Vec3b>(i+u,j+x)[2] = newVV(u, x);
                         }
                      }
                 }
                 else
                 {
                     cv::Matx<double,8,8> newYY = newY;
                     cv::Matx<double,8,8> newUU = newU;
                     cv::Matx<double,8,8> newVV = newV;
                     for(int u = 0; u < 8; u++) {
                         for(int x = 0; x < 8; x++) {
                            src.at<cv::Vec3b>(i+u,j+x)[0] = newYY(u, x);
                            src.at<cv::Vec3b>(i+u,j+x)[1] = newUU(u, x);
                            src.at<cv::Vec3b>(i+u,j+x)[2] = newVV(u, x);
                         }
                      }
                 }
            }
        }

    }
}
void MainWindow::convertYUVToRGB(cv::Mat &src) {

    const int nWidth = floor((src.cols)/8) * 8;//src.cols;
    const int nHeight = floor((src.rows)/8) * 8;//.rows;
    const float fWeightY[3] = {1, 1, 1};
    const float fWeightU[3] = {0, -0.39465, 2.03211};
    const float fWeightV[3] = {1.13983, -0.58060, 0};

    for (int iterW = 0; iterW<nWidth; iterW++)
    {
        for (int iterR = 0; iterR<nHeight; iterR++)
        {
            cv::Vec3b originalRGB = src.at<cv::Vec3b>(iterR, iterW);
            int uModifiedRGB;

            uModifiedRGB = fWeightY[0]*originalRGB[0] + fWeightU[0]*(originalRGB[1]-111.17745) + fWeightV[0]*(originalRGB[2]-156.825) ;
            src.at<cv::Vec3b>(iterR, iterW)[0] = clamp(uModifiedRGB, 0, 255);

            uModifiedRGB = fWeightY[1]*originalRGB[0] + fWeightU[1]*(originalRGB[1]-111.17745) + fWeightV[1]*(originalRGB[2]-156.825) ;
            src.at<cv::Vec3b>(iterR, iterW)[1] = clamp(uModifiedRGB, 0, 255);

            uModifiedRGB = fWeightY[2]*originalRGB[0] + fWeightU[2]*(originalRGB[1]-111.17745) + fWeightV[2]*(originalRGB[2]-156.825) ;
            src.at<cv::Vec3b>(iterR, iterW)[2] = clamp(uModifiedRGB, 0, 255);
        }
    }
}

void MainWindow::convertRGBToYUV(cv::Mat &src, cv::Mat &original) {

    const int nWidth = floor((src.cols)/8) * 8;
    const int nHeight = floor((src.rows)/8) * 8;
    const float fWeightR[3] = {0.299, -0.14713, 0.615};
    const float fWeightG[3] = {0.587, -0.28886, -0.51499};
    const float fWeightB[3] = {0.114, 0.436, -0.10001};

    for (int iterW = 0; iterW<nWidth; iterW++)
    {
        for (int iterR = 0; iterR<nHeight; iterR++)
        {
            // get the RGB values from the input image
            cv::Vec3b modifiedRGB = original.at<cv::Vec3b>(iterR, iterW);
            cv::Vec3b originalRGB = original.at<cv::Vec3b>(iterR, iterW);

            // Conversion to YUV
            modifiedRGB[0] = fWeightR[0]*originalRGB[0] + fWeightG[0]*originalRGB[1] + fWeightB[0]*originalRGB[2];
            modifiedRGB[1] = fWeightR[1]*originalRGB[0] + fWeightG[1]*originalRGB[1] + fWeightB[1]*originalRGB[2] + 111.17745;
            modifiedRGB[2] = fWeightR[2]*originalRGB[0] + fWeightG[2]*originalRGB[1] + fWeightB[2]*originalRGB[2] + 156.825;
            src.at<cv::Vec3b>(iterR, iterW) = modifiedRGB;
        }
    }
}

bool MainWindow::chromaSubsampling(cv::Mat &src, const int a, const int b) {

    if( a != b && b != 0) return false;
    const int nWidth = floor((src.cols)/8) * 8;
    const int nHeight = floor((src.rows)/8) * 8;
    for (int iterW = 0; iterW<nWidth; iterW+=4)
    {
        if(iterW+3 >= nWidth) continue; // No horizontal block of 4 pixels left

        for (int iterR = 0; iterR<nHeight; iterR+=2)
        {
            if(iterR+1 >= nHeight) continue; // No vertical block of 2 pixels left

            cv::Vec3b referencePixel[a+b];

            for(int i = 0; i < a; i++) {
                referencePixel[i] = src.at<cv::Vec3b>(iterR, iterW+(4/a)*i);
            }

            for(int i = 0; i < b; i++) {
                referencePixel[a+i] = src.at<cv::Vec3b>(iterR+1, iterW+(4/a)*i);
            }

            if(b == 0) {
                for(int c = 0; c < a+b; c++) {
                    for(int i = 1; i < 3; i++) {
                        src.at<cv::Vec3b>(iterR, iterW+1+(4/a)*c)[i] = referencePixel[c][i];
                        src.at<cv::Vec3b>(iterR+1, iterW+(4/a)*c)[i] = referencePixel[c][i];
                        src.at<cv::Vec3b>(iterR+1, iterW+1+(4/a)*c)[i] = referencePixel[c][i];
                    }
                }
            }
            else {
                for(int c = 0; c < a; c++) {
                    for(int i = 0; i < 4/a-1; i++) {
                        for(int x = 1; x < 3; x++) {
                            src.at<cv::Vec3b>(iterR, iterW+1+i+(4/a)*c)[x] = referencePixel[c][x];
                            src.at<cv::Vec3b>(iterR+1, iterW+1+i+(4/a)*c)[x] = referencePixel[c+a][x];
                        }
                    }
                }
            }
        }
    }
    return true;
}

// convert a opencv Mat containing RGB data to QImage
QImage MainWindow::MatRGB2QImage(const cv::Mat3b &src) {
        QImage dest(floor((src.cols)/8) * 8, floor((src.rows)/8) * 8, QImage::Format_ARGB32);
        for (int y = 0; y < floor((src.rows)/8) * 8; ++y)
        {
                const cv::Vec3b *srcrow = src[y];
                QRgb *destrow = (QRgb*)dest.scanLine(y);
                for (int x = 0; x < floor((src.cols)/8) * 8; ++x)
                {
                        destrow[x] = qRgba(srcrow[x][2], srcrow[x][1], srcrow[x][0], 255);
                }
        }
        return dest;
}


// convert a opencv Mat containing RGB data to QImage
QImage MainWindow::MatConverted2QImage(const cv::Mat3b &src) {
        QImage dest(floor((src.cols)/8) * 8, floor((src.rows)/8) * 8, QImage::Format_ARGB32);
        for (int y = 0; y < floor((src.rows)/8) * 8; ++y)
        {
                const cv::Vec3b *srcrow = src[y];
                QRgb *destrow = (QRgb*)dest.scanLine(y);
                for (int x = 0; x < floor((src.cols)/8) * 8; ++x)
                {
                    if(isOutputRGB) {
                        if(colorIsolationStatus == 4) destrow[x] = qRgba(srcrow[x][2], srcrow[x][1], srcrow[x][0], 255);
                        else if(colorIsolationStatus == 3) destrow[x] = qRgba(0, 0, srcrow[x][2], 255);
                        else if(colorIsolationStatus == 2) destrow[x] = qRgba(0, srcrow[x][1], 0, 255);
                        else destrow[x] = qRgba(srcrow[x][0], 0, 0, 255);
                    }
                    else
                    {
                        if(colorIsolationStatus == 4) destrow[x] = qRgba(srcrow[x][2], srcrow[x][1], srcrow[x][0], 255);
                        else if(colorIsolationStatus == 3) destrow[x] = qRgba(srcrow[x][2], srcrow[x][2], srcrow[x][2], 255);
                        else if(colorIsolationStatus == 2) destrow[x] = qRgba(srcrow[x][1], srcrow[x][1], srcrow[x][1], 255);
                        else destrow[x] = qRgba(srcrow[x][0],srcrow[x][0], srcrow[x][0], 255);
                    }

                }
        }
        return dest;
}

void MainWindow::on_dial_sliderMoved(int position)
{
    switch(position) {
        case 0:
        {
            ui->label_2->setText("Quality Level: 4:1:1");
            cSampleA = 1;
            cSampleB = 1;
            break;
        }
        case 1:
        {
            ui->label_2->setText("Quality Level: 4:2:0");
            cSampleA = 2;
            cSampleB = 0;
            break;
        }
        case 2:
        {
            ui->label_2->setText("Quality Level: 4:2:2");
            cSampleA = 2;
            cSampleB = 2;
            break;
        }
        case 3:
        {
            ui->label_2->setText("Quality Level: 4:4:4");
            cSampleA = 4;
            cSampleB = 4;
            break;
        }
    }
}

void MainWindow::on_horizontalSlider_sliderMoved(int position)
{
    switch(position)
    {
        case 0:
        {
            ui->label_3->setText("Color output: RGB");
            ui->pushButton_6->setText("G");
            ui->pushButton_7->setText("B");
            ui->pushButton_3->setText("R");
            isOutputRGB = true;
            break;
        }
        case 1:
        {
            ui->label_3->setText("Color output: YUV");
            isOutputRGB = false;
            ui->pushButton_6->setText("U");
            ui->pushButton_7->setText("V");
            ui->pushButton_3->setText("Y");
            break;
        }
    }
}

void MainWindow::on_pushButton_3_clicked(bool checked)
{
    if(checked) {
        ui->pushButton_4->setChecked(false);
        ui->pushButton_6->setChecked(false);
        ui->pushButton_7->setChecked(false);
        colorIsolationStatus = 1;
    } else {
        ui->pushButton_4->setChecked(true);
        ui->pushButton_6->setChecked(false);
        ui->pushButton_7->setChecked(false);
        colorIsolationStatus = 4;
    }

}

void MainWindow::on_pushButton_6_clicked(bool checked)
{
    if(checked) {
        ui->pushButton_4->setChecked(false);
        ui->pushButton_3->setChecked(false);
        ui->pushButton_7->setChecked(false);
        colorIsolationStatus = 2;
    } else {
        ui->pushButton_4->setChecked(true);
        ui->pushButton_3->setChecked(false);
        ui->pushButton_7->setChecked(false);
        colorIsolationStatus = 4;
    }
}

void MainWindow::on_pushButton_7_clicked(bool checked)
{
    if(checked) {
        ui->pushButton_4->setChecked(false);
        ui->pushButton_6->setChecked(false);
        ui->pushButton_3->setChecked(false);
        colorIsolationStatus = 3;
    } else {
        ui->pushButton_4->setChecked(true);
        ui->pushButton_6->setChecked(false);
        ui->pushButton_3->setChecked(false);
        colorIsolationStatus = 4;
    }
}

void MainWindow::on_pushButton_4_clicked(bool checked)
{
    if(checked) {
        ui->pushButton_4->setChecked(true);
        ui->pushButton_3->setChecked(false);
        ui->pushButton_6->setChecked(false);
        ui->pushButton_7->setChecked(false);
        colorIsolationStatus = 4;
    } else {
        ui->pushButton_3->setChecked(false);
        ui->pushButton_6->setChecked(false);
        ui->pushButton_7->setChecked(false);
        colorIsolationStatus = 4;
    }
}

void MainWindow::on_buttonBox_accepted()
{
    convertImage();
}

void MainWindow::on_buttonBox_rejected()
{
    /* Reset color to " All " and RGB */
    ui->pushButton_4->setChecked(true);
    ui->pushButton_3->setChecked(false);
    ui->pushButton_6->setChecked(false);
    ui->pushButton_7->setChecked(false);
    colorIsolationStatus = 4;

    ui->label_3->setText("Color output: RGB");
    ui->pushButton_6->setText("G");
    ui->pushButton_7->setText("B");
    ui->pushButton_3->setText("R");
    ui->horizontalSlider->setValue(0);
    isOutputRGB = true;

    // Reset the sliding dial
    ui->label_2->setText("Quality Level: 4:2:0");
    cSampleA = 2;
    cSampleB = 0;
    ui->dial->setValue(1);

    ui->checkBox->setChecked(true);
    ui->checkBox_2->setChecked(true);
    ui->checkBox_3->setChecked(true);
    isIDCTOn = true;
    isDCTOn = true;
    isQuantizationOn = true;
    ui->checkBox->setText("DCT (enabled)");
    ui->checkBox_2->setText("IDCT (enabled)");
    ui->checkBox_3->setText("Quantization (enabled)");
    ui->horizontalSlider_2->setValue(10);
    quantizationCoefficient = 1;
    convertImage();
}

void MainWindow::on_checkBox_clicked(bool checked)
{
    if(!checked) {
        isDCTOn = false;
        ui->checkBox->setText("DCT (disabled)");
    }
    else {
        isDCTOn = true;
        ui->checkBox->setText("DCT (enabled)");
    }
}

void MainWindow::on_checkBox_2_clicked(bool checked)
{
    if(!checked) {
        isIDCTOn = false;
        ui->checkBox_2->setText("IDCT (disabled)");
    }
    else {
        isIDCTOn = true;
        ui->checkBox_2->setText("IDCT (enabled)");
    }
}

void MainWindow::on_checkBox_3_clicked(bool checked)
{
    if(!checked) {
        isQuantizationOn = false;
        ui->checkBox_3->setText("Quantization (disabled)");
    }
    else {
        isQuantizationOn = true;
        ui->checkBox_3->setText("Quantization (enabled)");
    }
}


void MainWindow::on_listWidget_currentRowChanged(int currentRow)
{
    presetRow = currentRow+1;
}

void MainWindow::on_pushButton_5_clicked()
{
    if(presetRow == 1) {
        on_buttonBox_rejected();
        ui->label_3->setText("Color output: YUV");
        ui->pushButton_6->setText("U");
        ui->pushButton_7->setText("V");
        ui->pushButton_3->setText("Y");
        ui->horizontalSlider->setValue(1);
        isOutputRGB = false;

        ui->pushButton_4->setChecked(false);
        ui->pushButton_3->setChecked(false);
        ui->pushButton_6->setChecked(false);
        ui->pushButton_7->setChecked(true);
        colorIsolationStatus = 3;

        ui->label_2->setText("Quality Level: 4:1:1");
        cSampleA = 1;
        cSampleB = 1;
        ui->dial->setValue(0);
        convertImage();
    }
    else if(presetRow == 2) {
        on_buttonBox_rejected();
        ui->label_3->setText("Color output: YUV");
        ui->pushButton_6->setText("U");
        ui->pushButton_7->setText("V");
        ui->pushButton_3->setText("Y");
        ui->horizontalSlider->setValue(1);
        isOutputRGB = false;

        ui->pushButton_4->setChecked(false);
        ui->pushButton_3->setChecked(true);
        ui->pushButton_6->setChecked(false);
        ui->pushButton_7->setChecked(false);
        colorIsolationStatus = 1;

        ui->label_2->setText("Quality Level: 4:1:1");
        cSampleA = 1;
        cSampleB = 1;
        ui->dial->setValue(0);
        convertImage();
    }
    else if(presetRow == 3) {
        on_buttonBox_rejected();
        ui->label_3->setText("Color output: YUV");
        ui->pushButton_6->setText("U");
        ui->pushButton_7->setText("V");
        ui->pushButton_3->setText("Y");
        ui->horizontalSlider->setValue(1);
        isOutputRGB = false;

        ui->pushButton_4->setChecked(true);
        ui->pushButton_3->setChecked(false);
        ui->pushButton_6->setChecked(false);
        ui->pushButton_7->setChecked(false);
        colorIsolationStatus = 4;

        ui->label_2->setText("Quality Level: 4:2:0");
        cSampleA = 2;
        cSampleB = 0;
        ui->dial->setValue(1);
        convertImage();
    }
    else if(presetRow == 4) {
        on_buttonBox_rejected();

        ui->label_2->setText("Quality Level: 4:2:2");
        cSampleA = 2;
        cSampleB = 2;
        ui->dial->setValue(2);
        convertImage();
    }
    else if(presetRow == 5) {
        on_buttonBox_rejected();
        convertImage();
    }
}

void MainWindow::on_horizontalSlider_2_sliderMoved(int position)
{
    quantizationCoefficient = position;
}
