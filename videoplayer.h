#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QObject>

class VideoPlayer : public QObject
{
    Q_OBJECT
public:
    explicit VideoPlayer(QObject *parent = nullptr,const QStringList &videoPathList = QStringList());
    ~VideoPlayer();

    void playSingle();
    void playMain();
    void playGlobal();

signals:

private:
    QStringList videoList();
};

#endif // VIDEOPLAYER_H
