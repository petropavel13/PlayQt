#ifndef TRACKSLOADER_H
#define TRACKSLOADER_H

#include <QThread>
#include <QFileInfo>
#include <QDir>

#include "trackinfo.h"

#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

class TracksLoader : public QThread
{
    Q_OBJECT
public:
    TracksLoader(QObject* parent);

    void run();
    void setDir(QDir dir);
    static QString getFirst(QDir dir);

    QStringList getTrackList();
    QList<TrackInfo> getTracksInfo();

private:
    void loadDir(QDir dir);
    static bool isValidFile(QFileInfo file);

    int getFilesCount(QDir dir);

    QDir dirForLoad;

    static QStringList audioTypes;

    QStringList filesPaths;

    QList<TrackInfo> tracksInfo;

    int totalFilesCount;
    int notLoaddedFilesCount;
    int lastState;

    int tmp;

    QString tfn;

    QString currentFolderName;
    QString currentFileName;

signals:
    void loadStateChanged(int);
    void loadComplete();
    
};

#endif // TRACKSLOADER_H
