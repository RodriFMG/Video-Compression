//
// Created by RODRIGO on 2/11/2025.
//

#include "GOP.h"

GOP::GOP(size_t tam) : tam(tam){
    group.reserve(tam);
}

void GOP::addFrame(std::unique_ptr<Encoding> frame_enc) {

    if(group.size() == tam)
        throw std::runtime_error( "ERROR: Cantidad maxima del grupo alcanzada...");


    group.push_back( std::move(frame_enc) );
}

std::vector<cv::Mat> GOP::decode() {
    std::vector<cv::Mat> group_decode;
    group_decode.reserve(group_decode.size());

    for(auto& frame_enc : group)
        group_decode.push_back( frame_enc->decode() );

    return group_decode;
}