#pragma once
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>

class Video
{
public:
    explicit Video(const char* inputPath);
    ~Video();
    uint count() const;
    uint height() const;
    uint width() const;
    ushort fps() const;
    std::vector<cv::Mat> frames() const;
    
private:
    std::vector<cv::Mat> m_frames;
    std::string m_inputPath;
    cv::VideoCapture m_capture;
    uint m_count;
    uint m_height;
    uint m_width;
    ushort m_fps;
};

