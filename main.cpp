#include <iostream>
#include <limits>
#include <opencv2/opencv.hpp>
#include "GOP.h"

const std::vector< std::pair<int, int> > DiamonSearch = {
        {0, 0}, {-1, 0}, {1, 0}, {0, 1}, {0, -1}
};

// 5 fors = calavera
PointMotion MotionBlock(const cv::Mat& prevFrame, const cv::Mat& Frame,
                                size_t i, size_t j, size_t tam_block){

    const size_t height = prevFrame.rows;
    const size_t width = prevFrame.cols;

    int best_x = 0;
    int best_y = 0;

    double min_sad = std::numeric_limits<double>::max();

    for(auto [dx, dy] : DiamonSearch){

        // Punto de inicio de la matriz en ese sentido.

        // ref_x y ref_y son los indices de las posiciones iniciales de la región a recorrer.
        size_t ref_x = dx * tam_block + j;
        size_t ref_y = dy * tam_block + i;

        // Se le suma +tam_block para tener el punto final de ese eje.
        if( ref_x < 0 || ref_y < 0 || ref_x + tam_block >= width && ref_y + tam_block >= height)
            continue;

        double sad = 0.f;

        for(size_t stepY = 0; stepY < tam_block; ++stepY){
            for(size_t stepX = 0; stepX < tam_block; ++stepX){
                uchar Cij = Frame.at<uchar>( i + stepY, j + stepX );
                uchar Rij = prevFrame.at<uchar>( ref_y + stepY, ref_x + stepX );
                sad += abs( static_cast<double>(Cij) - static_cast<double>(Rij));
            }

            if(min_sad > sad){
                min_sad = sad;
                best_x = dx;
                best_y = dy;
            }

        }

    }

    return {std::make_pair(i, j), best_x, best_y};
}

cv::Mat ResidualPFrame(const std::vector<PointMotion>& MotionVector, const cv::Mat& PrevFrame, const cv::Mat& Frame,
                 size_t tam_block){

    cv::Mat PredictFrame = PrevFrame.clone();
    for(auto motion : MotionVector) SetBlockRef(PredictFrame, motion, tam_block);

    // CV_16S permite adaptar una matriz de opencv, por defecto uint8 (0-255), a una con signo:
    // CV -> opencv, 16 -> 16bits, S -> signed
    cv::Mat frame16S, predicho16S, residual;
    Frame.convertTo(frame16S, CV_16S);
    PredictFrame.convertTo(predicho16S, CV_16S);
    cv::subtract(frame16S, predicho16S, residual);


    return residual;
}

cv::Mat ResidualBFrame(const std::vector<PointMotion>& MVPrev, const std::vector<PointMotion>& MVFut,
                       const cv::Mat& PrevFrame, const cv::Mat& Frame, const cv::Mat& FutFrame,
                       size_t tam_block, double alpha = 0.5){

    cv::Mat PredictFramePrev = PrevFrame.clone();
    cv::Mat PredictFrameFut = FutFrame.clone();

    for(auto motion : MVPrev) SetBlockRef(PredictFramePrev, motion, tam_block);
    for(auto motion : MVFut) SetBlockRef(PredictFrameFut, motion, tam_block);

    cv::Mat PredictFrame = interpolation(PredictFramePrev, PredictFrameFut, alpha);

    cv::Mat frame16S, predicho16S, residual;
    Frame.convertTo(frame16S, CV_16S);
    PredictFrame.convertTo(predicho16S, CV_16S);
    cv::subtract(frame16S, predicho16S, residual);

    return residual;
}

std::unique_ptr<PFrame> ComputePFrame(const cv::Mat& PrevFrame, const cv::Mat& Frame, size_t tam_block){
    const size_t height = PrevFrame.rows;
    const size_t width = PrevFrame.cols;
    std::vector<PointMotion> MotionVector;

    for(int i = 0; i < height; i+=tam_block)
        for(int j = 0; j < width; j+=tam_block)
            MotionVector.push_back(MotionBlock(PrevFrame, Frame, i, j, tam_block));

    cv::Mat residual = ResidualPFrame(MotionVector, PrevFrame, Frame, tam_block);

    // CAMBIAR ESTO, YA QUE TMB PUEDE SER BFRAME!!!!"!#!"#"!#"!#"!
    auto* ref = new IFrame(PrevFrame, tam_block);

    return std::make_unique<PFrame>(ref, residual, MotionVector);
}

std::unique_ptr<IFrame> ComputeIFrame(const cv::Mat& Frame, size_t tam_block){
    return std::make_unique<IFrame>(Frame, tam_block);
}

// En el BFrame parece que está bien todo excepto el cálculo del FRAME FUTURO <- investigar ;-;

// alpha <- Factor de interpolación lineal, que tanta importancia le quiero dar al pasado en vez de al futuro
// valor entre [0, 1]
std::unique_ptr<BFrame> ComputeBFrame(const cv::Mat& PrevFrame, const cv::Mat& Frame, const cv::Mat& FutFrame,
                                      size_t tam_block, double alpha = 0.5){

    const size_t height = PrevFrame.rows;
    const size_t width = PrevFrame.cols;
    std::vector<PointMotion> MotionVectorPrev;
    std::vector<PointMotion> MotionVectorFut;

    for(int i = 0; i < height; i+=tam_block)
        for(int j = 0; j < width; j+=tam_block)
            MotionVectorPrev.push_back(MotionBlock(PrevFrame, Frame, i, j, tam_block));

    for(int i = 0; i < height; i+=tam_block)
        for(int j = 0; j < width; j+=tam_block)
            MotionVectorPrev.push_back(MotionBlock(FutFrame, Frame, i, j, tam_block));

    cv::Mat residual = ResidualBFrame(MotionVectorPrev, MotionVectorFut, PrevFrame, Frame, FutFrame,
                                         tam_block, alpha);

    cv::Mat help;

    auto* prevRef = new IFrame(PrevFrame, tam_block);
    auto* nextRef = new PFrame(nullptr, help, MotionVectorFut);

    return std::make_unique<BFrame>(prevRef, nextRef, MotionVectorPrev, MotionVectorFut, residual);
}

void VideoCompress(const cv::VideoCapture& cap, size_t step = 5){


}

int main(){



    cv::VideoCapture cap("C:/Users/RODRIGO/Video-Compression/test_avi.avi");

    if(!cap.isOpened()){
        std::cerr << "Ruta incorrecta!\n";
        exit(0);
    }

    std::vector<cv::Mat> frames;
    cv::Mat frame;
    while(cap.read(frame)) frames.push_back(frame);

    // Datos del video:
    std::cout << "Se abrio correctamente el video!\n";
    std::cout << "Resolucion: " << cap.get(cv::CAP_PROP_FRAME_WIDTH)
    << " x " << cap.get(cv::CAP_PROP_FRAME_HEIGHT) << std::endl;
    std::cout << "FPS: " << cap.get(cv::CAP_PROP_FPS) << std::endl;
    std::cout << "Numero de fotogramas: " << frames.size() << std::endl;


}