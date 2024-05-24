#pragma once
#include <QtWidgets/QMainWindow>
#include <QVideoWidget>
#include <QMediaPlayer>
#include <QPushButton>
#include <QProgressDialog>
#include <QThread>
#include "videostabilizer.h"
#include "ui_player.h"

class Player : public QMainWindow
{
    Q_OBJECT

public:
    Player(QWidget* parent = Q_NULLPTR);
    void onStabilizeVideo();
    
private:
    Ui::PlayerClass ui;
    QProgressDialog* m_progressDialog;
    //cv::Ptr<Video> m_video;
    cv::Ptr<VideoStabilizer> m_stabilizer;
    QThread* m_stabilizationThread;
};
