#include "player.h"

Player::Player(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    this->setStyleSheet(QString("QMainWindow{background: white};"));

    QVideoWidget* videoWidget_1 = new QVideoWidget(this);
    ui.gridLayout_1->addWidget(videoWidget_1);
    videoWidget_1->show();
    QMediaPlayer* player_1 = new QMediaPlayer(videoWidget_1);
    player_1->setMedia(QUrl::fromLocalFile("input.mp4"));
    player_1->setVideoOutput(videoWidget_1);
    player_1->setMuted(true);

    QVideoWidget* videoWidget_2 = new QVideoWidget(this);
    ui.gridLayout_2->addWidget(videoWidget_2);
    videoWidget_2->show();
    QMediaPlayer* player_2 = new QMediaPlayer(videoWidget_2);
    player_2->setMedia(QUrl::fromLocalFile("output.mp4"));
    player_2->setVideoOutput(videoWidget_2);
    player_2->setMuted(true);

    connect(ui.btnPlay, &QPushButton::clicked, this, [=]() {
        if (player_1->state() == QMediaPlayer::StoppedState)
        {
            player_1->play();
            player_2->play();
        }
        else if (player_1->state() == QMediaPlayer::PlayingState)
        {
            player_1->pause();
            player_2->pause();
        }
        else if (player_1->state() == QMediaPlayer::PausedState)
        {
            player_1->play();
            player_2->play();
        }
        });

    connect(ui.btnMute, &QPushButton::clicked, this, [=]() {
        if (player_1->isMuted() == true)
        {
            player_1->setMuted(false);
        }
        else
        {
            player_1->setMuted(true);
        }
        });

    connect(player_1, &QMediaPlayer::stateChanged, this, [=](QMediaPlayer::State newState) {
        if (newState == QMediaPlayer::PlayingState || newState == QMediaPlayer::PausedState)
            return;
        else if (newState == QMediaPlayer::StoppedState)
            ui.btnPlay->setChecked(false);
        });

    connect(ui.btnProcess, &QPushButton::clicked, this, &Player::onStabilizeVideo);

    m_progressDialog = new QProgressDialog("Processing...", "Cancel", 0, 100, this);
    m_progressDialog->setWindowModality(Qt::WindowModal);
    m_progressDialog->resize(500, 200);
    m_progressDialog->setAutoClose(true);
    m_progressDialog->setAutoReset(true);
    m_progressDialog->cancel();
    
    m_stabilizationThread = new QThread(this);
    m_stabilizer = cv::makePtr<VideoStabilizer>(new VideoStabilizer());
    m_stabilizer->setOutputPath("output.mp4");
    m_stabilizer->moveToThread(m_stabilizationThread);
    connect(m_progressDialog, &QProgressDialog::canceled, m_stabilizationThread, &QThread::quit);
    connect(m_stabilizer, &VideoStabilizer::processProgress, m_progressDialog, [this](int progress) { m_progressDialog->setValue(progress); });
    Video video("input.mp4");
    connect(m_stabilizationThread, &QThread::started, m_stabilizer, [=]() { m_stabilizer->stableVideo(video); });
}

void Player::onStabilizeVideo()
{
    if(!m_stabilizationThread->isRunning())
        m_stabilizationThread->start();
}