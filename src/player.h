#ifndef PLAYER_H
#define PLAYER_H

#include <QtCore>

#include "trackinfo.h"

class Player : public QObject
{
    Q_OBJECT
public:
    Player(QObject* parent = 0);

    virtual void loadSources(QStringList paths) = 0;
    virtual void addAndLoadSources(QStringList paths) = 0;

    virtual void setVolume(qreal vol) = 0;

    virtual void setSeek(quint64 seek) = 0;

    void setReapeat(bool on);
    void setShuffle(bool on);

    QStringList getSourcesPaths();

    virtual qreal getVolume() = 0;

    virtual quint64 getSeek() = 0;

    int getCurrentTrackIndex();

    bool getIsRepeat() const;
    bool getIsShuffle() const;

    virtual bool isPlaying() = 0;

    TrackInfo getTrackInfo(QString filePath);

    virtual TrackInfo getCurrentTrackInfo() = 0;

    virtual void seekWhenLoad(quint64 time) = 0;

    virtual void shuffleCurrentPlaylist() = 0;
    virtual void unshuffleCurrentPlaylist() = 0;

    virtual void clear() = 0;

public slots:
    void playPause();
    virtual void play() = 0;
    virtual void pause() = 0;
    virtual void stop() = 0;

    void next();
    void prev();

    void setCurrentTrackIndex(int index, bool playImmediately = true);

    void removeTrackFromPlaylist(int index);

    void volumeUp(qreal by = 0.1);
    void volumeDown(qreal by = 0.1);

    void seekForward(quint64 time = 5000);
    void seekBackward(quint64 time = 5000);

signals:
    void playing();
    void paused();
    void stopped();

    void trackChanged(TrackInfo info, int index);

    void volumeChanged(qreal newVolume);

    void seekChanged(quint64 newSeek);

protected:
    QStringList sourcesPaths;

    virtual void loadTrack(int trackIndex) = 0;

    virtual void removeTrack(int trackIndex) = 0;

    virtual void updatePlaylist() = 0;

    int getRandomBetween(int start, int end);

    int currentTrackIndex;

    bool isRepeat;
    bool isShuffle;

private:
    void setAndLoadTrack(int trackIndex);
};

#endif // PLAYER_H
