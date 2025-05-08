#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QObject>
#include <QWidget>
#include <QVideoWidget>
#include <QMediaPlayer>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QSlider>
#include <QScrollArea>
#include <QList>

// 前向声明UI类，避免头文件循环依赖
namespace Ui {
class MainWindow;
}

class VideoPlayer : public QObject
{
    Q_OBJECT

public:
    explicit VideoPlayer(QObject *parent = nullptr, const QStringList &videoList = QStringList());
    ~VideoPlayer();

    // 初始化视频播放器
    void initialize(Ui::MainWindow *ui= nullptr);

public slots:
    // 播放模式
    void playSingle();   // 单视角播放
    void playMain();     // 主视角播放
    void playGlobal();   // 全局视角播放

    // 播放控制
    void play_pause();

private slots:
    // 内部槽函数
    void onThumbnailClicked();

private:
    // 布局
    Ui::MainWindow *m_ui =nullptr;

    // 清除当前布局
    void clearLayout(QLayout* layout);

    // 创建缩略图
    void createThumbnails();

    // 播放特定索引的视频
    void playVideoByIndex(int index);

    // 视频列表
    QStringList m_videoList;

    // 当前播放的视频索引
    int m_currentVideoIndex = 0;

    // 播放器和视频控件
    QList<QMediaPlayer*> m_mediaPlayers;
    QList<QVideoWidget*> m_videoWidgets;

    // 缩略图按钮
    QList<QPushButton*> m_thumbnailButtons;

    // 当前模式
    enum Mode { Single, Main, Global };
    Mode m_currentMode = Single;
};

#endif // VIDEOPLAYER_H
