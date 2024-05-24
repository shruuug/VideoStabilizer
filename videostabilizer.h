#pragma once
#include <opencv2/opencv.hpp>
#include <QObject>
#include "video.h"
#define NFEATURES 5000
#define RATIO 0.5

class VideoStabilizer : public QObject
{
    Q_OBJECT

public:
    VideoStabilizer(QObject* parent = Q_NULLPTR);
    ~VideoStabilizer();
    VideoStabilizer(const VideoStabilizer& other) = delete;
    VideoStabilizer(VideoStabilizer && other) = delete;
    VideoStabilizer& operator=(const VideoStabilizer & other) = delete;
    VideoStabilizer& operator=(VideoStabilizer && other) = delete;

    void stableVideo(const Video& video);
    void setOutputPath(const char* path);

signals:
    void processProgress(int progress);

private:
    cv::Ptr<cv::VideoWriter> m_videoWriter;
    cv::Ptr<cv::ORB> m_orb;
    cv::Ptr<cv::FlannBasedMatcher> m_flann;
    std::string m_outputPath;
};

