#include "video.h"
using namespace cv;

Video::Video(const char* inputPath)
{
    m_inputPath = inputPath;
    m_capture = VideoCapture(inputPath);
    if (!m_capture.isOpened())
    {
        throw std::runtime_error("Error: Failed to open video file: " + std::string(inputPath));
        return;
    }
    m_height = m_capture.get(CAP_PROP_FRAME_HEIGHT);
    m_width = m_capture.get(CAP_PROP_FRAME_WIDTH);
    m_fps = m_capture.get(CAP_PROP_FPS);
    m_count = m_capture.get(CAP_PROP_FRAME_COUNT);

    while (true) {
        Mat frame;
        m_capture >> frame;
        if (frame.empty())  break;
        m_frames.push_back(std::move(frame));
    }
}

Video::~Video()
{
    m_capture.release();
}

uint Video::count() const 
{
    return m_count;
}

uint Video::height() const 
{
    return m_height;
}

uint Video::width() const 
{
    return m_width;
}

ushort Video::fps() const 
{
    return m_fps;
}

std::vector<cv::Mat> Video::frames() const 
{
    return m_frames;
}