//
// Created by RODRIGO on 2/11/2025.
//

#include "Encoding.h"

// SobreCargas
std::pair<size_t, size_t> operator+(std::pair<size_t, size_t> pos, std::pair<int, int> motion){
    return {pos.first + motion.first, pos.second + motion.second};
}

// Destructores
Encoding::~Encoding() = default;
IFrame::~IFrame() = default;
PFrame::~PFrame() = default;
BFrame::~BFrame() = default;

// Constructores
IFrame::IFrame(cv::Mat reference) : ref( std::move(reference) ){}

PFrame::PFrame(IFrame *reference, cv::Mat residual, std::vector<PointMotion> MotionVector)
: ref(reference), res( std::move(residual) ), MV( std::move(MotionVector) ) {}


BFrame::BFrame(IFrame *prevReference, PFrame *nextReference, std::vector<PointMotion> MotionVectorPrev,
               std::vector<PointMotion> MotionVectorNext, cv::Mat residual) : prevRef(prevReference),
               nextRef(nextReference), MVprev(std::move(std::move(MotionVectorPrev))),
               MVnext(std::move(MotionVectorNext)), res(std::move(residual)){}

// Methods
void IFrame::SetPos(PointMotion pm) {
    std::pair<size_t, size_t> to_upd = pm.pos + std::make_pair(pm.dx, pm.dy);
    cv::Mat copy = ref.clone();

    copy.at<uchar>(to_upd.second, to_upd.first) = ref.at<uchar>(pm.pos.second, pm.pos.first);
    ref = std::move(copy);
}

void PFrame::SetPos(PointMotion pm) {
    ref->SetPos(pm);
}

// Decode
cv::Mat IFrame::decode() {
    return ref;
}

cv::Mat PFrame::decode() {

    // Maybe falta algo más...
    for(auto motion : MV) ref->SetPos(motion);

    cv::Mat refDecoded = ref->decode();
    cv::Mat result;
    cv::add(refDecoded, res, result);

    return result;
}

// Interpolacion Lineal simple
cv::Mat interpolation(const cv::Mat& prev, const cv::Mat& next, double alpha = 0.5) {
    cv::Mat blended;
    cv::addWeighted(prev, 1.0 - alpha, next, alpha, 0.0, blended);
    return blended;
}

cv::Mat BFrame::decode() {

    for(auto motion : MVprev) prevRef->SetPos(motion);
    for(auto motion: MVnext) nextRef->SetPos(motion);

    cv::Mat refDecodedPrev = prevRef->decode();
    cv::Mat refDecodedNext = nextRef->decode();
    cv::Mat result;

    cv::add(interpolation(refDecodedPrev, refDecodedNext), res, result);

    return result;

}