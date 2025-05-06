#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QObject>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QFileInfoList>
#include <QDir>
#include <QGridLayout>
#include <QtMath>

class VideoPlayer : public QObject
{
    Q_OBJECT
public:
    explicit VideoPlayer(QObject *parent = nullptr,const QStringList &videoPathList = QStringList());
    ~VideoPlayer();

    void playSingle(int index = 0);
    void playMain();
    void playGlobal();

    //把网格布局赋予videoPlayer
    void initialize(QGridLayout *layout);

    // 获取视频文件数量
    int getVideoCount() const;

signals:

private:
    // 创建视频播放器和视频窗口
    void createPlayer(int index);

    void stopAllVideos();

    // 清除布局
    void clearLayout();

    // 添加播放器到布局
    void addPlayerToLayout(QVideoWidget *videoWidget, int row, int col);

    QStringList m_videoList;
    QGridLayout *m_layout;                   // 网格布局
    QList<QMediaPlayer*> m_players;          // 媒体播放器列表
    QList<QVideoWidget*> m_videoWidgets;     // 视频窗口列表

    int m_currentLayout;                     // 当前布局类型
};

#endif // VIDEOPLAYER_H
