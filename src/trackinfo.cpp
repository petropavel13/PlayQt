#include "trackinfo.h"

TrackInfo::TrackInfo(QString filePath)
{
    this->filePath = filePath;
    this->totalTime = 0;
}

QString TrackInfo::getTrackName()
{
    if(trackName.isEmpty())
    {
//        trackName = filePath.right(filePath.size() - filePath.lastIndexOf("/") - 1);
        trackName = filePath.right(filePath.size() - filePath.lastIndexOf("/") - 1);
        trackName = trackName.left(trackName.lastIndexOf(".")); // removes .mp3, etc. FIXME
    }

    return trackName;
}

QString TrackInfo::getAlbumName()
{
    if(albumName.isEmpty())
    {
        QString temp = filePath.left(filePath.lastIndexOf(getTrackName()) - 1);
        albumName = temp.right(temp.size() - temp.lastIndexOf("/") - 1);
    }

    return albumName;
}

QString TrackInfo::getArtistName()
{
    if(artistName.isEmpty())
    {
        QString temp = filePath.left(filePath.lastIndexOf(getAlbumName()) - 1);
        artistName = temp.right(temp.size() - temp.lastIndexOf("/") - 1);
    }

    return artistName;
}


quint64 TrackInfo::getTotalTime()
{
    if(totalTime == 0)
    {
        totalTime = 0; // TODO read from file
    }

    return totalTime;
}

void TrackInfo::setTime(quint64 time)
{
    totalTime = time;
}
