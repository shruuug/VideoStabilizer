#pragma once
#include <QRunnable>
#include <opencv2/opencv.hpp>
class StabilizationTask : public QRunnable
{
public:
    void run() override
    {

    }
private:
    cv::Ptr<cv::ORB> m_orb;
    cv::Ptr<cv::FlannBasedMatcher> m_flann;
};

