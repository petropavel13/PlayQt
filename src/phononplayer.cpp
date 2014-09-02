#include "phononplayer.h"

AudioOutput* PhononPlayer::output = 0;

MediaObject* PhononPlayer::currentPlaying = 0;

PhononPlayer::PhononPlayer(QObject *parent) :
    Player(parent)
{
    needSeek = false;

    getAudioOutput()->setParent(this);

    getCurrentPlaying()->setParent(this);

    createPath(getCurrentPlaying(), getAudioOutput());

    connect(getCurrentPlaying(), SIGNAL(stateChanged(Phonon::State,Phonon::State)), this, SLOT(playingStateChanged(Phonon::State,Phonon::State)));
    connect(getCurrentPlaying(), SIGNAL(finished()), this, SLOT(playlistFinished()));
    connect(getAudioOutput(), SIGNAL(volumeChanged(qreal)), this, SLOT(outputVolumeChanged(qreal)));
    connect(getCurrentPlaying(), SIGNAL(currentSourceChanged(Phonon::MediaSource)), this, SLOT(currentSourceChanged(Phonon::MediaSource)));
}

void PhononPlayer::loadSources(QStringList paths)
{
    clear();

    sourcesPaths = paths;

    foreach (QString path, sourcesPaths) {
        sources.append(MediaSource(path));
    }

    getCurrentPlaying()->enqueue(sources);

    currentTrackIndex = -1;
}

void PhononPlayer::addAndLoadSources(QStringList paths)
{
    QList<MediaSource> tempSources;

    foreach (QString path, paths) {
        sourcesPaths.append(path);

        tempSources.append(MediaSource(path));
    }

    sources.append(tempSources);

    if(getIsShuffle())
    {
        getCurrentPlaying()->setQueue(shuffleSources(tempSources));
    }
    else
    {
        getCurrentPlaying()->setQueue(tempSources);
    }
}

void PhononPlayer::setVolume(qreal vol)
{
    if(getAudioOutput()->volume() != vol)
    {
        getAudioOutput()->setVolume(vol);
        emit volumeChanged(vol);
    }
}

qreal PhononPlayer::getVolume()
{
    return getAudioOutput()->volume();
}

void PhononPlayer::setSeek(quint64 seek)
{
    getCurrentPlaying()->seek(seek);
}

quint64 PhononPlayer::getSeek()
{
    return getCurrentPlaying()->currentTime();
}

bool PhononPlayer::isPlaying()
{
    return getCurrentPlaying()->state() == PlayingState;
}

TrackInfo PhononPlayer::getCurrentTrackInfo()
{
    if(getCurrentTrackIndex() < 0)
        return TrackInfo("/no/tracks/loaded.mp3");

    // FIXME

    TrackInfo ti = getTrackInfo(sources.at(getCurrentTrackIndex()).fileName());

    ti.setTime(currentPlaying->totalTime());

    return ti;

    // FIXME
}

AudioOutput *PhononPlayer::getAudioOutput()
{
    if(output == 0)
    {
        output = new AudioOutput(0);
    }

    return output;
}

MediaObject *PhononPlayer::getCurrentPlaying()
{
    if(currentPlaying == 0)
    {
        currentPlaying = new MediaObject(0);
    }

    return currentPlaying;
}

void PhononPlayer::seekWhenLoad(quint64 time)
{
    needSeek = true;
    seekTo = time;
}

void PhononPlayer::clear()
{
    sources.clear();
    sourcesPaths.clear();
    getCurrentPlaying()->clearQueue();

    currentTrackIndex = -1;
}

void PhononPlayer::shuffleCurrentPlaylist()
{
    getCurrentPlaying()->clearQueue();
    getCurrentPlaying()->enqueue(shuffleSources(sources));
}

void PhononPlayer::unshuffleCurrentPlaylist()
{
    getCurrentPlaying()->clearQueue();

    if(getCurrentTrackIndex() < sources.size() - 1)
    {
        getCurrentPlaying()->enqueue(sources.mid(getCurrentTrackIndex() + 1));
    }
    else
    {
        getCurrentPlaying()->enqueue(shuffleSources(sources));
    }
}

void PhononPlayer::play()
{
    getCurrentPlaying()->play();
}

void PhononPlayer::pause()
{
    getCurrentPlaying()->pause();
}

void PhononPlayer::stop()
{
    getCurrentPlaying()->stop();
}

void PhononPlayer::playingStateChanged(State newstate, State oldstate)
{
    if(newstate == PlayingState)
    {
        volumeDown(); // bad fix for
        volumeUp(); // symbian

        emit playing();
    }

    if(newstate == PausedState)
    {
        emit paused();
    }

    if(needSeek)
    {
        setSeek(seekTo);
        needSeek = false;
    }
}

void PhononPlayer::currentSourceChanged(MediaSource ms)
{
    currentTrackIndex = sources.indexOf(ms);

    emit trackChanged(getCurrentTrackInfo(), currentTrackIndex);
}

void PhononPlayer::playlistFinished()
{
    if(getIsRepeat())
    {
        setCurrentTrackIndex(0, true);
    }
}

void PhononPlayer::outputVolumeChanged(qreal newVolume)
{
    emit volumeChanged(newVolume);
}

void PhononPlayer::loadTrack(int trackIndex)
{
    Q_ASSERT(trackIndex >= 0 && trackIndex < sources.size());

    getCurrentPlaying()->setCurrentSource(sources.at(trackIndex));
    getCurrentPlaying()->clearQueue();

    const bool isLastTrack = trackIndex < sources.size() - 1;

    if(isLastTrack)
    {
        if(getIsShuffle())
        {
            getCurrentPlaying()->enqueue(shuffleSources(sources.mid(trackIndex + 1)));
        }
        else
        {
            getCurrentPlaying()->enqueue(sources.mid(trackIndex + 1));
        }
    }
    else
    {
        if(getIsShuffle())
        {
            getCurrentPlaying()->enqueue(shuffleSources(sources));
        }
        else
        {
            getCurrentPlaying()->enqueue(sources);
        }
    }
}

void PhononPlayer::removeTrack(int trackIndex)
{
    Q_ASSERT(trackIndex >= 0 && trackIndex < sources.size());

    sources.removeAt(trackIndex);
}

void PhononPlayer::updatePlaylist()
{
    getCurrentPlaying()->clearQueue();

    if(getCurrentTrackIndex() < sources.size() - 1)
    {

        if(getIsShuffle())
        {
            getCurrentPlaying()->enqueue(shuffleSources(sources.mid(getCurrentTrackIndex() + 1)));
        }
        else
        {
            getCurrentPlaying()->enqueue(sources.mid(getCurrentTrackIndex() + 1));
        }
    }
    else
    {
        if(getIsShuffle())
        {
            getCurrentPlaying()->enqueue(shuffleSources(sources));
        }
        else
        {
            getCurrentPlaying()->enqueue(sources);
        }
    }
}

QList<MediaSource> PhononPlayer::shuffleSources(const QList<MediaSource> sources)
{
    QList<MediaSource> copy = sources;
    QList<MediaSource> shuffleSources;

    do
    {
        shuffleSources.append(copy.takeAt(getRandomBetween(0, copy.size() - 1)));
    }
    while(copy.size() > 0);

    return shuffleSources;
}
