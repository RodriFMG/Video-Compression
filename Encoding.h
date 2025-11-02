//
// Created by RODRIGO on 2/11/2025.
//

#ifndef VIDEO_COMPRESSION_ENCODING_H
#define VIDEO_COMPRESSION_ENCODING_H

#include <iostream>
#include <opencv2/opencv.hpp>
#include <utility>
#include <vector>

struct PointMotion{
    int dx, dy;
    std::pair<size_t, size_t> pos;

    PointMotion(std::pair<size_t, size_t> pos, int dx, int dy)
            : pos(pos), dx(dx), dy(dy){}
};


class Enconding{
public:
    virtual cv::Mat decode() = 0;
    virtual ~Enconding() = 0;
};

class IFrame : Enconding{
private:
    cv::Mat ref;
public:

    explicit IFrame(cv::Mat reference);

    void SetPos(PointMotion pm);
    cv::Mat decode() override;
    ~IFrame() override;

};

class PFrame : Enconding{
private:

    IFrame* ref;
    cv::Mat res;
    std::vector<PointMotion> MV;

public:

    PFrame(IFrame* reference, cv::Mat residual, std::vector<PointMotion> MotionVector);

    void SetPos(PointMotion pm);
    cv::Mat decode() override;
    ~PFrame() override;
};

class BFrame : Enconding{
private:

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
