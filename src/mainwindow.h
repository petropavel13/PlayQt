#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <QtCore>

#include <phonon/SeekSlider>
#include <phonon/VolumeSlider>

#include "phononplayer.h"

#include "playlistwidget.h"
#include "tracksloader.h"
#include "trackinfo.h"

#ifdef Q_OS_SYMBIAN

#include "symbianphone.h"

/* Folder dialog */

#include <AknCommonDialogs.h>
#include <CAknMemorySelectionDialog.h>
#include <CAknFileSelectionDialog.h>
#include <PathInfo.h>

#endif

#define PLAYER_SCREEN_ACTIVE 0
#define PLAYLIST_SCREEN_ACTIVE 1

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

private:
    QWidget* rootWidget;

    QVBoxLayout* rootWidgetLayout;

    QStackedWidget* headerStackedWidget;

    QWidget* headerWidget;
    QHBoxLayout* headerWidgetLayout;

    QProgressBar* loadProgressBar;

    QLabel* volumeLabel;
    QLabel* timeLabel;

#ifdef Q_OS_SYMBIAN
    QLabel* batteryLabel;
#endif

    QStackedWidget* centerStackedWidget;

    QWidget* playerWidget;
    PlayListWidget* playListWidget;

    QVBoxLayout* playerLayout;

    QHBoxLayout* centerlayout;

    QHBoxLayout* optionslayout;

    QPushButton* repeatButton;
    QPushButton* shuffleButton;

#ifndef Q_OS_SYMBIAN
    QFileDialog* openFolderDialog;
#endif

    Phonon::SeekSlider* seekSlider;
    Phonon::VolumeSlider* volumeSlider;

    QVBoxLayout* trackInfoLayout;
    QLabel* artistDirLabel;
    QLabel* albumDirLabel;
    QLabel* trackFileLabel;

    QVBoxLayout* controlsLayout;
    QHBoxLayout* prevPlayNextLayout;
    QPushButton* playPauseButton;
    QPushButton* nextTrackButton;
    QPushButton* previousTrackButton;

    QHBoxLayout* menuLayout;
    QPushButton* listScreenOpenButton;
    QPushButton* exitBackButton;

    TracksLoader* tracksLoader;

    QSettings* settings;

    QTimer* minuteTimer;

#ifdef Q_OS_SYMBIAN
    bool wasPlayingBeforeCall;

    qint64 currentSeek;
#endif

    bool playListKeyEventPriority;

    void init();
    void createWidgets();
    void connectSignalsAndSlots();
    void arrangeWidgets();

    void save();
    void load();

    static QList<TrackInfo> getTracksInfo(QStringList tracksPaths);
    static void resetMargins(QLayout* l);

    Player* player;

#ifdef Q_OS_SYMBIAN
    Phone* phone;
#endif

protected:
    void resizeEvent(QResizeEvent *);
    void keyPressEvent(QKeyEvent *);

signals:

public slots:
    void loadSelectedDirectory(QString folderPath = QString());

    void quickSave();

#ifdef Q_OS_SYMBIAN
    void minimize();
#endif

private slots:
    void showPlaylistOrOpenFolder();
    void exitOrBack();

    void appyStyles();
    void updatePlayPause();

    void handleKeyEvent(QKeyEvent* e);

    void handleDeleteKeyPress();

    void isRepeatChanged(bool isRepeat);
    void isShuffleChanged(bool isShuffle);

    void loadComplete();

#ifdef Q_OS_SYMBIAN
    void phoneCall();
    void callFinished();
#endif

    void updateCurrentPlayingTrackInfo();

    void updateVolumeInHeader();
    void updateTimeInHeader();

#ifdef Q_OS_SYMBIAN
    void updateBatteryLevelInHeader(uint level);
#endif
};

#endif // MAINWINDOW_H
