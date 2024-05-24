#include "videostabilizer.h"
using namespace cv;
using namespace std;
inline bool compareKeyPoint(const KeyPoint& kp1, const KeyPoint& kp2);

VideoStabilizer::VideoStabilizer(QObject* parent) : QObject (parent)
{
    m_orb = ORB::create(NFEATURES);
    m_flann = makePtr<FlannBasedMatcher>(makePtr<flann::KDTreeIndexParams>(5), makePtr<cv::flann::SearchParams>(50));
    m_videoWriter = makePtr<VideoWriter>();
}

VideoStabilizer::~VideoStabilizer()
{
    m_videoWriter->release();
}

void VideoStabilizer::stableVideo(const Video& video)
{
    if (m_outputPath.empty())
    {
        throw std::runtime_error("Error: Output path is empty");
        return;
    }
    emit processProgress(10);
    Ptr<Mat> stabilizedFrames = Ptr<Mat>(new Mat[video.count()]);
    std::vector<KeyPoint> firstKeyPoints, lastKeyPoints;
    Mat firstDescriptor, lastDescriptor;
    //关键点检测
    m_orb->detect(video.frames()[0], firstKeyPoints);
    m_orb->detect(video.frames()[video.count() - 1], lastKeyPoints);
    //生成描述符
    m_orb->compute(video.frames()[0], firstKeyPoints, firstDescriptor);
    m_orb->compute(video.frames()[video.count() - 1], lastKeyPoints, lastDescriptor);
    //转换为32位浮点数
    firstDescriptor.convertTo(firstDescriptor, CV_32F);
    lastDescriptor.convertTo(lastDescriptor, CV_32F);
    //Flann法匹配特征点
    vector<vector<DMatch>> match;
    m_flann->knnMatch(firstDescriptor, lastDescriptor, match, 2);
    //Lowe's算法筛选匹配点
    vector<DMatch> goodMatch;
    for (size_t i = 0; i < match.size(); ++i) {
        if (match[i][0].distance < RATIO * match[i][1].distance) {
            goodMatch.push_back(move(match[i][0]));
        }
    }
    //生成特征点模板
    vector<KeyPoint> kpTemplate;
    for (size_t i = 0; i < goodMatch.size(); ++i) {
        kpTemplate.push_back(move(firstKeyPoints[goodMatch[i].queryIdx]));
    }
    //逐帧处理
    stabilizedFrames[0] = video.frames()[0];
    for (int index = 1; index < video.count(); ++index)
    {
        Mat frame = video.frames()[index];
        std::vector<KeyPoint> keyPoints;
        Mat descriptors;
        m_orb->detect(video.frames()[index], keyPoints);
        m_orb->compute(video.frames()[index], keyPoints, descriptors);
        descriptors.convertTo(descriptors, CV_32F);
        vector<vector<DMatch>> curMatch;
        m_flann->knnMatch(firstDescriptor, descriptors, curMatch, 2);
        vector<DMatch> curGoodMatch;
        for (size_t i = 0; i < curMatch.size(); ++i) {
            if (curMatch[i][0].distance < RATIO * curMatch[i][1].distance) {
                curGoodMatch.push_back(move(curMatch[i][0]));
            }
        }
        //通过模板筛选特征点
        vector<Point2f> firstFramePoints, curFramePoints;
        for (const DMatch& m: curGoodMatch) {
            for (const KeyPoint& p: kpTemplate)
            {
                if (bool equal = compareKeyPoint(firstKeyPoints[m.queryIdx], p))
                {
                    firstFramePoints.push_back(move(firstKeyPoints[m.queryIdx].pt));
                    curFramePoints.push_back(move(keyPoints[m.trainIdx].pt));
                }
            }
        }
        //根据特征点获取变换矩阵
        Mat H = findHomography(curFramePoints, firstFramePoints, RHO);
        //透视变换，使当前帧背景与模板对齐
        Mat stabilizedFrame;
        warpPerspective(frame, stabilizedFrame, H, Size(frame.cols, frame.rows), INTER_LINEAR, BORDER_REPLICATE);
        stabilizedFrames[index] = stabilizedFrame;
        //发送进度信号（最高90%）
        int progress = index * 90 / video.count();
        emit processProgress(progress);
    }
    //打开视频写入器
    bool success = m_videoWriter->open(m_outputPath, VideoWriter::fourcc('M', 'P', '4', 'V'), video.fps(), Size(video.width(), video.height()));
    if (!success)
    {
        throw std::runtime_error("Error: Failed to open video writer");
        return;
    }
    // 写入图像帧到视频
    for (int i = 0; i < video.count(); ++i) {
        m_videoWriter->write(stabilizedFrames[i]);
    }
    //发送完成信号
    emit processProgress(100);
}

void VideoStabilizer::setOutputPath(const char* path)
{
    m_outputPath = path;
}

inline bool compareKeyPoint(const KeyPoint &kp1, const KeyPoint& kp2)
{
    if (kp1.pt != kp2.pt)   return false;
    if (kp1.size != kp2.size)   return false;
    if (kp1.angle != kp2.angle)   return false;
    if (kp1.response != kp2.response)   return false;
    if (kp1.octave != kp2.octave)   return false;
    if (kp1.class_id != kp2.class_id)   return false;
    return true;
}