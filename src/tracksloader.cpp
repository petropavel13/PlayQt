#include "tracksloader.h"

QStringList TracksLoader::audioTypes = QStringList() << ".mp3" << ".flac" << ".m4a" << ".acc" << ".ogg";

TracksLoader::TracksLoader(QObject *parent)
    : QThread(parent)
{
}

void TracksLoader::run()
{
    totalFilesCount = getFilesCount(dirForLoad);
    notLoaddedFilesCount = totalFilesCount;

    lastState = (int)((totalFilesCount - notLoaddedFilesCount) / ((double)totalFilesCount / 100.0));

    filesPaths.clear();
    tracksInfo.clear();

    loadDir(dirForLoad);

    emit loadStateChanged(100);

    emit loadComplete();
}

void TracksLoader::setDir(QDir dir)
{
    dirForLoad = dir;
}

QStringList TracksLoader::getTrackList()
{
    return filesPaths;
}

QList<TrackInfo> TracksLoader::getTracksInfo()
{
    return tracksInfo;
}

void TracksLoader::loadDir(QDir dir)
{
    QFileInfoList filesAndFolders = dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot | QDir::Readable);

    foreach(QFileInfo fi, filesAndFolders)
    {
        notLoaddedFilesCount--;
        if(fi.isFile())
        {
            if(isValidFile(fi))
            {
                tfn = fi.filePath();

                filesPaths.append(tfn);

                tracksInfo.append(TrackInfo(tfn));

                tmp = lastState;
                lastState = (int)((totalFilesCount - notLoaddedFilesCount) / ((double)totalFilesCount / 100.0));

                if(lastState > tmp)
                {
                    emit loadStateChanged(lastState);
                }
#ifndef QT_NO_DEBUG
                qDebug() << "loadDir" << tfn;
#endif
            }
        }
        else if(fi.isDir())
        {
            loadDir(QDir(fi.filePath()));
        }
    }
}

QString TracksLoader::getFirst(QDir dir)
{
    QFileInfoList filesAndFolders = dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot | QDir::Readable);

    foreach(QFileInfo fi, filesAndFolders)
    {
        if(fi.isFile())
        {
            if(isValidFile(fi))
            {
                return fi.filePath();
            }
        }
        else if(fi.isDir())
        {
            QString result = getFirst(QDir(fi.filePath()));

            if(!result.isEmpty())
                return result;
        }
    }

    return QString();
}

bool TracksLoader::isValidFile(QFileInfo file)
{
    foreach(QString str, audioTypes)
    {
        if(file.fileName().toUpper().endsWith(str.toUpper()))
        {
            return true;
        }
    }

    return false;
}

int TracksLoader::getFilesCount(QDir dir)
{
    int count = 0;

    QFileInfoList filesAndFolders = dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot | QDir::Readable);

    foreach(QFileInfo fi, filesAndFolders)
    {
        if(fi.isFile())
        {
            count++;
        }
        else if(fi.isDir())
        {
            count += getFilesCount(QDir(fi.filePath()));
        }
    }

    return count;
}
