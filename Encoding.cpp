//
// Created by RODRIGO on 2/11/2025.
//

#include "Encoding.h"

// Destructores
Encoding::~Encoding() = default;
IFrame::~IFrame() = default;
PFrame::~PFrame() = default;
BFrame::~BFrame() = default;

// Constructores
IFrame::IFrame(cv::Mat reference, size_t tam_block) : ref( std::move(reference) ), tam_block(tam_block){}

PFrame::PFrame(IFrame *reference, cv::Mat residual, std::vector<PointMotion> MotionVector)
: ref(reference), res( std::move(residual) ), MV( std::move(MotionVector) ) {}


BFrame::BFrame(IFrame *prevReference, PFrame *nextReference, std::vector<PointMotion> MotionVectorPrev,
               std::vector<PointMotion> MotionVectorNext, cv::Mat residual) :
               prevRef(prevReference),
               nextRef(nextReference), MVprev(std::move(std::move(MotionVectorPrev))),
               MVnext(std::move(MotionVectorNext)), res(std::move(residual)){}

// Methods
void IFrame::SetBlock(PointMotion pm) {
    SetBlockRef(ref, pm, tam_block);
}

void PFrame::SetBlock(PointMotion pm) {
    ref->SetBlock(pm);
}

// Decode
cv::Mat IFrame::decode() {
    return ref;
}

cv::Mat PFrame::decode() {

    // Maybe falta algo más...
    for(auto motion : MV) ref->SetBlock(motion);

    cv::Mat refDecoded = ref->decode();
    cv::Mat result;
    cv::add(refDecoded, res, result);

    return result;
}

cv::Mat BFrame::decode() {

    // Maybe esto está mal... (ya que acá aplicamos los seteos, pero en el decode tmb hacemos los seteos)
    for(auto motion : MVprev) prevRef->SetBlock(motion);
    for(auto motion: MVnext) nextRef->SetBlock(motion);

    cv::Mat refDecodedPrev = prevRef->decode();
    cv::Mat refDecodedNext = nextRef->decode();
    cv::Mat result;

    cv::add(interpolation(refDecodedPrev, refDecodedNext), res, result);

    return result;

}