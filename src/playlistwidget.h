#ifndef PLAYLISTWIDGET_H
#define PLAYLISTWIDGET_H

#include <QtCore>
#include <QtGui>

#include "trackinfo.h"

class PlayListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit PlayListWidget(QWidget *parent = 0);

    int getSelectedItemIndex();

private:
    QList<QListWidgetItem*> tracksItems;

    int currentPlayingIndex;

protected:
    void keyPressEvent(QKeyEvent * e);

signals:
    void trackSelected(int);
    void keyPressed(QKeyEvent *e);
    
public slots:
    void updateList(QList<TrackInfo> info);
    void setCurrentPlayingTrackIndex(int index);
    void setCurrentTrackState(bool isPlaying);

private slots:
    void itemClicked(QListWidgetItem* item);
    
};

#endif // PLAYLISTWIDGET_H
