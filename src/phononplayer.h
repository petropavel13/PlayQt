#ifndef PHONONPLAYER_H
#define PHONONPLAYER_H

#include <phonon/MediaObject>
#include <phonon/AudioOutput>
#include <phonon/BackendCapabilities>

#include "player.h"

using namespace Phonon;

class PhononPlayer : public Player
{
    Q_OBJECT
public:
    PhononPlayer(QObject* parent = 0);

    void loadSources(QStringList paths);
    void addAndLoadSources(QStringList paths);

    void setVolume(qreal vol);

    qreal getVolume();

    void setSeek(quint64 seek);

    quint64 getSeek();

    bool isPlaying();

//    TrackInfo getTrackInfo(QString filePath);

    TrackInfo getCurrentTrackInfo();

    static AudioOutput* getAudioOutput();

    static MediaObject* getCurrentPlaying();

    void seekWhenLoad(quint64 time);

    void clear();

    void shuffleCurrentPlaylist();
    void unshuffleCurrentPlaylist();

public slots:
    void play();
    void pause();
    void stop();

private slots:
    void playingStateChanged(Phonon::State newstate, Phonon::State oldstate);
    void currentSourceChanged(Phonon::MediaSource ms);
    void playlistFinished();

    void outputVolumeChanged(qreal newVolume);

protected:
    void loadTrack(int trackIndex);
    void removeTrack(int trackIndex);

    void updatePlaylist();

private:
    static AudioOutput* output;

    static MediaObject* currentPlaying;

    QList<MediaSource> sources;

    QList<MediaSource> shuffleSources(const QList<MediaSource> sources);

    bool needSeek;

    quint64 seekTo;
};

#endif // PHONONPLAYER_H
