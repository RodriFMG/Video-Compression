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
    std::vector<std::unique_ptr<Encoding>> group;
    size_t tam;
public:

    explicit GOP(size_t tam);
    void addFrame(std::unique_ptr<Encoding> frame_enc);
    std::vector<cv::Mat> decode();
    bool is_full();

};


#endif //VIDEO_COMPRESSION_GOP_H
