#include <iostream>
#include <opencv2/opencv.hpp>
#include "GOP.h"
using namespace std;

void VideoCompress(const cv::VideoCapture& cap, size_t step = 5){

}

int main(){



    cv::VideoCapture cap("C:/Users/RODRIGO/Video-Compression/test_avi.avi");

    if(!cap.isOpened()){
        cerr << "Ruta incorrecta!\n";
        exit(0);
    }

    vector<cv::Mat> frames;
    cv::Mat frame;
    while(cap.read(frame)) frames.push_back(frame);

    // Datos del video:
    cout << "Se abrio correctamente el video!\n";
    cout << "Resolucion: " << cap.get(cv::CAP_PROP_FRAME_WIDTH)
    << " x " << cap.get(cv::CAP_PROP_FRAME_HEIGHT) << endl;
    cout << "FPS: " << cap.get(cv::CAP_PROP_FPS) << endl;
    cout << "Numero de fotogramas: " << frames.size() << endl;


}