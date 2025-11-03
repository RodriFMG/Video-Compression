//
// Created by RODRIGO on 2/11/2025.
//

#ifndef VIDEO_COMPRESSION_ENCODING_H
#define VIDEO_COMPRESSION_ENCODING_H

#include <iostream>
#include <opencv2/opencv.hpp>
#include <utility>
#include <vector>

// Luego organizo mejor esto xd (si me acuerdo)
struct PointMotion{
    int dx, dy;
    std::pair<size_t, size_t> pos;

    PointMotion(std::pair<size_t, size_t> pos, int dx, int dy)
            : pos(pos), dx(dx), dy(dy){}
};

void SetBlockRef(cv::Mat& ref, PointMotion pm, size_t tam_block) {
    cv::Rect srcBlock(pm.pos.first, pm.pos.second, tam_block, tam_block);
    cv::Rect dstBlock(pm.pos.first + pm.dx, pm.pos.second + pm.dy, tam_block, tam_block);

    // Mover el bloque a la zona que más se parezca.
    ref(dstBlock) = ref(srcBlock).clone();
}

// Interpolacion Lineal simple
cv::Mat interpolation(const cv::Mat& prev, const cv::Mat& next, double alpha = 0.5) {
    cv::Mat blended;
    cv::addWeighted(prev, 1.0 - alpha, next, alpha, 0.0, blended);
    return blended;
}

class Encoding{
public:
    virtual cv::Mat decode() = 0;
    virtual ~Encoding() = 0;
};

class IFrame : public Encoding{
private:
    cv::Mat ref;
    size_t tam_block;
public:

    explicit IFrame(cv::Mat reference, size_t tam_block);

    void SetBlock(PointMotion pm);
    cv::Mat decode() override;
    ~IFrame() override;

};

class PFrame : public Encoding{
private:

    IFrame* ref;
    cv::Mat res;
    std::vector<PointMotion> MV;

public:

    PFrame(IFrame* reference, cv::Mat residual, std::vector<PointMotion> MotionVector);

    void SetBlock(PointMotion pm);
    cv::Mat decode() override;
    ~PFrame() override;
};

class BFrame : public Encoding{
private:

    // Maybe ambas deban de ser tipo Encoding para evitar fallos en el encode.
    IFrame* prevRef;
    PFrame* nextRef;

    std::vector<PointMotion> MVprev;
    std::vector<PointMotion> MVnext;

    cv::Mat res;

public:

    BFrame(IFrame* prevReference, PFrame* nextReference, std::vector<PointMotion> MotionVectorPrev,
           std::vector<PointMotion> MotionVectorNext, cv::Mat residual);

    cv::Mat decode() override;
    ~BFrame() override;
};


#endif //VIDEO_COMPRESSION_ENCODING_H
