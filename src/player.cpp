#include "player.h"

Player::Player(QObject *parent) :
    QObject(parent)
{
    currentTrackIndex = -1;
    isRepeat = false;
    isShuffle = false;

    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
}

void Player::setCurrentTrackIndex(int index, bool playImmediately)
{
    if(sourcesPaths.size() <= 0 || index < 0)
        return;

    if(getCurrentTrackIndex() == index)
    {
        playPause();
        return;
    }

    stop();

    setAndLoadTrack(index);

    if(playImmediately)
    {
        play();
    }
}

void Player::removeTrackFromPlaylist(int index)
{
    if(sourcesPaths.size() <= 0 || index < 0)
        return;

    if(getCurrentTrackIndex() == index)
    {
        bool wasPlaying = isPlaying();
        int newTrackIndex = -1;

        if((currentTrackIndex + 1) < sourcesPaths.size())
        {
            newTrackIndex = currentTrackIndex;
        }
        else
        {
            newTrackIndex = 0;
        }

        stop();

        sourcesPaths.removeAt(index);

        removeTrack(index);

        updatePlaylist();

        setAndLoadTrack(index);

        if(wasPlaying)
        {
            play();
        }
    }
    else
    {
        sourcesPaths.removeAt(index);

        removeTrack(index);

        updatePlaylist();

        if(index < getCurrentTrackIndex())
        {
            currentTrackIndex--;
        }
    }
}

void Player::setReapeat(bool on)
{
    isRepeat = on;
}

void Player::setShuffle(bool on)
{
    isShuffle = on;

    if(on)
    {
        if(sourcesPaths.size() > 0)
        {
            shuffleCurrentPlaylist();
        }
    }
    else
    {
        if(sourcesPaths.size() > 0)
        {
            unshuffleCurrentPlaylist();
        }
    }
}

QStringList Player::getSourcesPaths()
{
    return sourcesPaths;
}

int Player::getCurrentTrackIndex()
{
    return currentTrackIndex;
}

bool Player::getIsRepeat() const
{
    return isRepeat;
}

bool Player::getIsShuffle() const
{
    return isShuffle;
}

TrackInfo Player::getTrackInfo(QString filePath)
{
    return TrackInfo(filePath);
}

void Player::playPause()
{
    if(isPlaying())
    {
        pause();
    }
    else
    {
        play();
    }
}

void Player::next()
{
    if(getIsShuffle())
    {
        setCurrentTrackIndex(getRandomBetween(0, sourcesPaths.size() - 1), isPlaying());
    }
    else
    {
        if((currentTrackIndex + 1) < sourcesPaths.size())
        {
            setCurrentTrackIndex(currentTrackIndex + 1, isPlaying());
        }
        else
        {
            setCurrentTrackIndex(0, isPlaying());
        }
    }
}

void Player::prev()
{
    if(getIsShuffle())
    {
        setCurrentTrackIndex(getRandomBetween(0, sourcesPaths.size() - 1), isPlaying());
    }
    else
    {
        if(currentTrackIndex > 0)
        {
            setCurrentTrackIndex(currentTrackIndex - 1, isPlaying());
        }
        else
        {
            setCurrentTrackIndex(sourcesPaths.size() - 1, isPlaying());
        }
    }
}

void Player::volumeUp(qreal by)
{
    qreal currentVolume = getVolume();

    if(currentVolume >= 1.0)
        return;

    if(currentVolume >= 0.9)
    {
        currentVolume = 1.0;
    }
    else
    {
        currentVolume += by;
    }

    setVolume(currentVolume);
}

void Player::volumeDown(qreal by)
{
    qreal currentVolume = getVolume();

    if(currentVolume < 0.0)
    {
        currentVolume = 0.0;
        return;
    }

    if(currentVolume <= 0.1)
    {
        currentVolume = 0.0;
    }
    else
    {
        currentVolume -= by;
    }

    setVolume(currentVolume);
}

void Player::seekForward(quint64 time)
{
    const quint64 totalTrackTime = getCurrentTrackInfo().getTotalTime();

    const quint64 newSeek = getSeek() + time;

    if((newSeek) < totalTrackTime)
    {
        setSeek(newSeek);
    }
    else
    {
        setSeek(totalTrackTime);
    }
}

void Player::seekBackward(quint64 time)
{
    const quint64 currentSeek = getSeek();

    if((currentSeek - time) > 0)
    {
        setSeek(currentSeek - time);
    }
    else
    {
        setSeek(0);
    }
}

int Player::getRandomBetween(int start, int end)
{
    Q_ASSERT(start <= end);

    return qrand() % (end - start + 1) + start;
}

void Player::setAndLoadTrack(int trackIndex)
{
    currentTrackIndex = trackIndex < sourcesPaths.size() ? trackIndex : sourcesPaths.size() - 1; // WTF? CHECK ME!

    loadTrack(currentTrackIndex);
}
