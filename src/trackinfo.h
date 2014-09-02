#ifndef TRACKINFO_H
#define TRACKINFO_H

#include <QString>

class TrackInfo
{
public:
    TrackInfo(QString filePath);

    QString getTrackName();
    QString getAlbumName();
    QString getArtistName();
    quint64 getTotalTime();

    // FIXME START

    void setTime(quint64 time);

    // FIXME END

private:
    QString filePath;

    QString trackName;
    QString albumName;
    QString artistName;
    quint64 totalTime;
};

#endif // TRACKINFO_H
