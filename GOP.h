//
// Created by RODRIGO on 2/11/2025.
//

#ifndef VIDEO_COMPRESSION_GOP_H
#define VIDEO_COMPRESSION_GOP_H

#include "Encoding.h"
#include <memory>

// Group Of Pictures
class GOP {
private:
    std::vector<std::unique_ptr<Enconding>> group;
    size_t tam;
public:

    explicit GOP(size_t tam);
    void addFrame(std::unique_ptr<Enconding> frame_enc);
    std::vector<cv::Mat> decode();

};


#endif //VIDEO_COMPRESSION_GOP_H
