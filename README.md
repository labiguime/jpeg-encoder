## JPEG Encoder/Decoder with Qt Creator

This is a software that achieves JPEG Compression but it is made in a way that allows you to learn about the algorithms. You can choose from many settings to try to understand the logic behind, namely RGB to YUV conversation, quantization matrix, chromatic sub-sampling and discrete cosine transform and it's inverse.
I used Qt Creator which is a cross-platform GUI framework for C++. Click [here](https://www.qt.io/download) to download Qt. I also make use of OpenCV which you might have to configure to make it work with Qt. To do so, take a look at the following links:

[macOS ](https://www.learnopencv.com/configuring-qt-for-opencv-on-osx/)

[Windows](https://wiki.qt.io/How_to_setup_Qt_and_openCV_on_Windows)

[Ubuntu](http://rodrigoberriel.com/2014/11/using-opencv-3-qt-creator-3-2-qt-5-3/ )
> Note: 
>you may need to type in the following command after you successfully
installed OpenCV. It will create a link for opencv4.pc
ln /usr/local/Cellar/opencv/4.0.1/lib/pkgconfig/opencv4.pc /usr/
local/Cellar/opencv/4.0.1/lib/pkgconfig/opencv.pc

![Demonstration](/image/software-demo.gif)
