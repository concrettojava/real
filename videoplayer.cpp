#include "videoplayer.h"
#include <QDebug>
#include <QFileInfo>
#include <QMediaContent>
#include <QPixmap>
#include <QDir>
#include "ui_mainwindow.h"

VideoPlayer::VideoPlayer(QObject *parent, const QStringList &videoList)
    : QObject(parent)
    , m_videoList(videoList)
{
}

VideoPlayer::~VideoPlayer()
{
    // 清理资源
    for (QMediaPlayer *player : m_mediaPlayers) {
        player->stop();
        delete player;
    }

    for (QVideoWidget *widget : m_videoWidgets) {
        delete widget;
    }

    for (QPushButton *button : m_thumbnailButtons) {
        delete button;
    }
}

void VideoPlayer::initialize(Ui::MainWindow *ui)
{
    m_ui = ui;
    // 验证是否有视频
    if (m_videoList.isEmpty()) {
        qDebug() << "没有可播放的视频";
        return;
    }

    // 根据初始模式设置UI
    playSingle();
}

void VideoPlayer::clearLayout(QLayout* layout)
{
    if (!layout) return;

    // 暂停所有播放器
    for (QMediaPlayer *player : m_mediaPlayers) {
        player->stop();
    }

    // 清理布局中的所有项目
    while (layout->count() > 0) {
        QLayoutItem* item = layout->takeAt(0);
        if (item->layout()) {
            clearLayout(item->layout());
            delete item->layout();
        }
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }

    // 清空媒体播放器和视频控件列表
    qDeleteAll(m_mediaPlayers);
    m_mediaPlayers.clear();

    // 视频控件已在上面的循环中删除，这里只需清空列表
    m_videoWidgets.clear();
}

void VideoPlayer::createThumbnails()
{
    // 清理旧的缩略图按钮
    for (QPushButton *button : m_thumbnailButtons) {
        delete button;
    }
    m_thumbnailButtons.clear();

    if (!m_ui->scrollArea) {
        qDebug() << "无法找到滚动区域";
        return;
    }

    // 获取滚动区域内容
    QWidget *scrollContent = m_ui->scrollArea->widget();
    QHBoxLayout *scrollLayout = nullptr;

    // 检查是否已经有布局
    if (scrollContent->layout()) {
        scrollLayout = qobject_cast<QHBoxLayout*>(scrollContent->layout());

        // 清理旧布局
        while (scrollLayout->count() > 0) {
            QLayoutItem* item = scrollLayout->takeAt(0);
            if (item->widget()) {
                delete item->widget();
            }
            delete item;
        }
    } else {
        scrollLayout = new QHBoxLayout(scrollContent);
        scrollLayout->setContentsMargins(5, 5, 5, 5);
        scrollLayout->setSpacing(10);
    }

    // 为每个视频创建缩略图按钮
    for (int i = 0; i < m_videoList.size(); ++i) {
        QString videoPath = m_videoList.at(i);
        QFileInfo fileInfo(videoPath);

        // 创建按钮
        QPushButton *button = new QPushButton();
        button->setFixedSize(100, 80);
        button->setProperty("videoIndex", i);

        // 设置文本为视频文件名
        button->setText(fileInfo.fileName());
        button->setStyleSheet("QPushButton { text-align: center; padding: 2px; }");

        // 连接点击信号
        connect(button, &QPushButton::clicked, this, &VideoPlayer::onThumbnailClicked);

        // 添加到布局
        scrollLayout->addWidget(button);
        m_thumbnailButtons.append(button);
    }

    // 添加弹簧项以确保缩略图靠左对齐
    scrollLayout->addStretch();
}

void VideoPlayer::playSingle()
{
    // 切换到单视角模式
    m_currentMode = Single;

    // 清理当前布局
    clearLayout(m_ui->singleGrid);
    clearLayout(m_ui->mainGrid);
    clearLayout(m_ui->globalGrid);

    // 如果没有视频，直接返回
    if (m_videoList.isEmpty()) {
        qDebug() << "没有视频可播放";
        return;
    }

    // 创建媒体播放器和视频窗口
    QMediaPlayer *player = new QMediaPlayer(this);
    QVideoWidget *videoWidget = new QVideoWidget();

    // 设置播放器对应的视频输出窗口
    player->setVideoOutput(videoWidget);

    // 添加到布局
    m_ui->singleGrid->addWidget(videoWidget, 0, 0);

    // 存储播放器和视频窗口

    m_mediaPlayers.append(player);
    m_videoWidgets.append(videoWidget);

    // 创建缩略图
    createThumbnails();

    // 默认播放第一个视频
    playVideoByIndex(m_currentVideoIndex);
}

void VideoPlayer::playMain()
{
    // 切换到主视角模式
    m_currentMode = Main;

    // 清理当前布局
    clearLayout(m_ui->singleGrid);
    clearLayout(m_ui->mainGrid);
    clearLayout(m_ui->globalGrid);

    // 如果没有视频，直接返回
    if (m_videoList.isEmpty()) {
        qDebug() << "没有视频可播放";
        return;
    }

    // 确定要播放的视频数量（最多4个）
    int videoCount = qMin(4, m_videoList.size());

    // 创建网格布局排列视频（2x2格式）
    for (int i = 0; i < videoCount; i++) {
        // 创建媒体播放器和视频窗口
        QMediaPlayer *player = new QMediaPlayer(this);
        QVideoWidget *videoWidget = new QVideoWidget();

        // 设置播放器对应的视频输出窗口
        player->setVideoOutput(videoWidget);

        // 添加到布局（2x2网格）
        int row = i / 2;
        int col = i % 2;
        m_ui->mainGrid->addWidget(videoWidget, row, col);

        // 存储播放器和视频窗口
        m_mediaPlayers.append(player);
        m_videoWidgets.append(videoWidget);

        // 直接设置媒体并播放
        QString videoPath = m_videoList.at(i);
        player->setMedia(QUrl::fromLocalFile(videoPath));
        player->play();
    }
}

void VideoPlayer::playGlobal()
{
    // 切换到全局视角模式
    m_currentMode = Global;

    // 清理当前布局
    clearLayout(m_ui->singleGrid);
    clearLayout(m_ui->mainGrid);
    clearLayout(m_ui->globalGrid);

    // 如果没有视频，直接返回
    if (m_videoList.isEmpty()) {
        qDebug() << "没有视频可播放";
        return;
    }

    // 确定网格的行和列数（基于视频数量来确定合适的布局）
    int count = m_videoList.size();
    int cols = qMin(3, count); // 最多3列
    int rows = (count + cols - 1) / cols; // 计算需要多少行

    for (int i = 0; i < count; ++i) {
        QMediaPlayer *player = new QMediaPlayer(this);
        QVideoWidget *videoWidget = new QVideoWidget();

        // 设置播放器对应的视频输出窗口
        player->setVideoOutput(videoWidget);


        // 计算行列位置
        int row = i / cols;
        int col = i % cols;

        // 添加到布局
        m_ui->globalGrid->addWidget(videoWidget, row, col);

        // 存储播放器和视频窗口
        m_mediaPlayers.append(player);
        m_videoWidgets.append(videoWidget);

        // 设置视频路径并播放
        player->setMedia(QUrl::fromLocalFile(m_videoList.at(i)));
    }

    // 全部开始播放
    for (QMediaPlayer *player : m_mediaPlayers) {
        player->play();
    }
}

void VideoPlayer::playVideoByIndex(int index)
{
    if (index < 0 || index >= m_videoList.size()) {
        qDebug() << "无效的视频索引:" << index;
        return;
    }

    // 更新当前索引
    m_currentVideoIndex = index;

    // 获取视频路径
    QString videoPath = m_videoList.at(index);
    qDebug() << "播放视频:" << videoPath;

    // 设置媒体源并播放
    if (!m_mediaPlayers.isEmpty()) {
        QMediaPlayer *player = m_mediaPlayers.first();
        player->setMedia(QUrl::fromLocalFile(videoPath));
        player->play();
    }

    // 更新高亮显示的缩略图
    for (int i = 0; i < m_thumbnailButtons.size(); ++i) {
        QPushButton *button = m_thumbnailButtons.at(i);
        if (i == index) {
            button->setStyleSheet("QPushButton { background-color: #ADD8E6; text-align: center; padding: 2px; }");
        } else {
            button->setStyleSheet("QPushButton { text-align: center; padding: 2px; }");
        }
    }
}

void VideoPlayer::play_pause()
{
    for (QMediaPlayer *player : m_mediaPlayers) {
        // Check current state and toggle
        if (player->state() == QMediaPlayer::PlayingState) {
            player->pause();
        } else {
            player->play();
        }
    }
}

void VideoPlayer::onThumbnailClicked()
{
    // 获取发送者
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (!button) {
        return;
    }

    // 获取视频索引
    bool ok;
    int index = button->property("videoIndex").toInt(&ok);
    if (!ok) {
        return;
    }

    // 播放对应索引的视频
    playVideoByIndex(index);
}
