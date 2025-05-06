#include "videoplayer.h"
#include "QDebug"

VideoPlayer::VideoPlayer(QObject *parent,const QStringList &videoPathList)
    : QObject{parent}, m_layout(nullptr), m_currentLayout(0)
{
    m_videoList = videoPathList;
}

void VideoPlayer::initialize(QGridLayout *layout){
    m_layout = layout;
}


void VideoPlayer::stopAllVideos()
{
    for (auto player : m_players) {
        player->stop();
    }
}

VideoPlayer::~VideoPlayer()
{
    stopAllVideos();

    // 释放资源
    for (auto player : m_players) {
        player->deleteLater();
    }

    for (auto widget : m_videoWidgets) {
        widget->deleteLater();
    }

    m_players.clear();
    m_videoWidgets.clear();
}

void VideoPlayer::playSingle(int index){

    if (m_videoList.isEmpty() || index >= m_videoList.size()) {
        qDebug() << "没有可播放的视频文件或索引无效";
        return;
    }

    stopAllVideos();
    clearLayout();
    m_currentLayout = 1;

    // 创建单个播放器
    createPlayer(index);

    // 添加到布局
    addPlayerToLayout(m_videoWidgets.last(), 0, 0);

    // 开始播放
    m_players.last()->play();
    m_players.last()->stop();
}

void VideoPlayer::playMain(){

}

void VideoPlayer::playGlobal(){

}

void VideoPlayer::createPlayer(int index)
{
    if (index >= m_videoList.size()) {
        return;
    }

    // 创建媒体播放器
    QMediaPlayer *player = new QMediaPlayer(this);

    // 创建视频窗口
    QVideoWidget *videoWidget = new QVideoWidget;
    videoWidget->setAspectRatioMode(Qt::KeepAspectRatio);

    // 设置播放器和视频窗口的关联
    player->setVideoOutput(videoWidget);

    // 设置媒体源
    player->setMedia(QUrl::fromLocalFile(m_videoList.at(index)));

    // 添加到列表
    m_players.append(player);
    m_videoWidgets.append(videoWidget);
}

void VideoPlayer::clearLayout()
{
    if (!m_layout) {
        return;
    }

    // 从布局中移除所有项
    while (m_layout->count() > 0) {
        QLayoutItem *item = m_layout->takeAt(0);
        if (item->widget()) {
            item->widget()->hide();
        }
        delete item;
    }

    // 清除播放器和视频窗口
    for (auto player : m_players) {
        player->stop();
        player->deleteLater();
    }

    for (auto widget : m_videoWidgets) {
        widget->deleteLater();
    }

    m_players.clear();
    m_videoWidgets.clear();
}

void VideoPlayer::addPlayerToLayout(QVideoWidget *videoWidget, int row, int col)
{
    if (!m_layout || !videoWidget) {
        qDebug() <<"没有关联的布局以放置视频";
        return;
    }

    videoWidget->show();
    m_layout->addWidget(videoWidget, row, col);
}

