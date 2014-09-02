#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    init();
    createWidgets();
    arrangeWidgets();

    load();

#ifdef Q_OS_SYMBIAN
    showFullScreen();
#endif

#ifdef QT_SIMULATOR
    showFullScreen();
#endif

    minuteTimer->start(60000);

    appyStyles();

    connectSignalsAndSlots();

    updateVolumeInHeader();

    playPauseButton->setFocus();

//    resize(240, 320);
}

void MainWindow::init()
{
    PhononPlayer* phononPlayer = new PhononPlayer(this);

    player = phononPlayer;

    volumeSlider = new Phonon::VolumeSlider(phononPlayer->getAudioOutput());
    volumeSlider->setOrientation(Qt::Vertical);
    volumeSlider->setSingleStep(10);
    volumeSlider->setMuteVisible(false);
    volumeSlider->setFocusPolicy(Qt::NoFocus);
    volumeSlider->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    seekSlider = new Phonon::SeekSlider(phononPlayer->getCurrentPlaying());
    seekSlider->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    tracksLoader = new TracksLoader(this);

    settings = new QSettings(QString::fromUtf8("null.inc"), "PlayQt", this);

#ifdef Q_OS_SYMBIAN
    phone = new SymbianPhone(this);

    wasPlayingBeforeCall = false;
#endif

    minuteTimer = new QTimer(this);

    playListKeyEventPriority = false;
}

void MainWindow::createWidgets()
{
    rootWidget = new QWidget();

    headerStackedWidget = new QStackedWidget();
    headerStackedWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    headerWidget = new QWidget();

    loadProgressBar = new QProgressBar();
    loadProgressBar->setMinimum(0);
    loadProgressBar->setMaximum(100);
    loadProgressBar->setVisible(false);

    volumeLabel = new QLabel();
    timeLabel = new QLabel();

#ifdef Q_OS_SYMBIAN
    batteryLabel = new QLabel();
    updateBatteryLevelInHeader(phone->getCurrentBatteryCharge());
#endif

    centerStackedWidget = new QStackedWidget();

    playerWidget = new QWidget();

    playListWidget = new PlayListWidget();

#ifndef Q_OS_SYMBIAN
    openFolderDialog = new QFileDialog();
    openFolderDialog->setFileMode(QFileDialog::DirectoryOnly);
#endif

//    repeatButton = new QPushButton(QString::fromUtf8("repeat"));
    repeatButton = new QPushButton();
    repeatButton->setCheckable(true);

//    shuffleButton = new QPushButton(QString::fromUtf8("shuffle"));
    shuffleButton = new QPushButton();
    shuffleButton->setCheckable(true);


    artistDirLabel = new QLabel();

    albumDirLabel = new QLabel();

    trackFileLabel = new QLabel();


    playPauseButton = new QPushButton();
    nextTrackButton = new QPushButton();
    previousTrackButton = new QPushButton();

    listScreenOpenButton = new QPushButton(QString::fromUtf8("List"));
    listScreenOpenButton->setFocusPolicy(Qt::NoFocus);

    exitBackButton = new QPushButton(QString::fromUtf8("Exit"));
    exitBackButton->setFocusPolicy(Qt::NoFocus);

    updateVolumeInHeader();
    updateTimeInHeader();
}

void MainWindow::connectSignalsAndSlots()
{

#ifndef Q_OS_SYMBIAN
    connect(openFolderDialog, SIGNAL(accepted()), this, SLOT(loadSelectedDirectory()));
#endif

    connect(tracksLoader, SIGNAL(loadStateChanged(int)), loadProgressBar, SLOT(setValue(int)));
    connect(tracksLoader, SIGNAL(loadComplete()), this, SLOT(loadComplete()));

    connect(playPauseButton, SIGNAL(clicked()), player, SLOT(playPause()));
    connect(nextTrackButton, SIGNAL(clicked()), player, SLOT(next()));
    connect(previousTrackButton, SIGNAL(clicked()), player, SLOT(prev()));

    connect(repeatButton, SIGNAL(toggled(bool)), this, SLOT(isRepeatChanged(bool)));
    connect(shuffleButton, SIGNAL(toggled(bool)), this, SLOT(isShuffleChanged(bool)));

    connect(playListWidget, SIGNAL(trackSelected(int)), player, SLOT(setCurrentTrackIndex(int)));

    connect(player, SIGNAL(playing()), this, SLOT(updatePlayPause()));
    connect(player, SIGNAL(playing()), this, SLOT(quickSave()));
    connect(player, SIGNAL(paused()), this, SLOT(updatePlayPause()));
    connect(player, SIGNAL(paused()), this, SLOT(quickSave()));

    connect(player, SIGNAL(trackChanged(TrackInfo,int)), this, SLOT(updateCurrentPlayingTrackInfo()));
    connect(player, SIGNAL(trackChanged(TrackInfo,int)), this, SLOT(quickSave()));

    connect(player, SIGNAL(volumeChanged(qreal)), this, SLOT(updateVolumeInHeader()));
    connect(player, SIGNAL(volumeChanged(qreal)), this, SLOT(quickSave()));

    connect(listScreenOpenButton, SIGNAL(clicked()), this, SLOT(showPlaylistOrOpenFolder()));
    connect(exitBackButton, SIGNAL(clicked()), this, SLOT(exitOrBack()));

    connect(minuteTimer, SIGNAL(timeout()), this, SLOT(updateTimeInHeader()));

    connect(playListWidget, SIGNAL(keyPressed(QKeyEvent*)), this, SLOT(handleKeyEvent(QKeyEvent*)));

#ifdef Q_OS_SYMBIAN
    connect(phone, SIGNAL(idle()), this, SLOT(callFinished()));
    connect(phone, SIGNAL(ringing()), this, SLOT(phoneCall()));

    connect(phone, SIGNAL(leftSoftKeyPressed()), this, SLOT(showPlaylistOrOpenFolder()));
    connect(phone, SIGNAL(rightSoftKeyPressed()), this, SLOT(exitOrBack()));

    connect(phone, SIGNAL(deleteKeyPressed()), this, SLOT(handleDeleteKeyPress()));

    connect(phone, SIGNAL(number1KeyPressed()), player, SLOT(seekBackward()));
    connect(phone, SIGNAL(number2KeyPressed()), player, SLOT(volumeUp()));
    connect(phone, SIGNAL(number3KeyPressed()), player, SLOT(seekForward()));
    connect(phone, SIGNAL(number4KeyPressed()), player, SLOT(prev()));
    connect(phone, SIGNAL(number5KeyPressed()), player, SLOT(playPause()));
    connect(phone, SIGNAL(number6KeyPressed()), player, SLOT(next()));
    connect(phone, SIGNAL(number8KeyPressed()), player, SLOT(volumeDown()));

    connect(phone, SIGNAL(starKeyPressed()), phone, SLOT(lockKeypad()));
    connect(phone, SIGNAL(sharpKeyPressed()), this, SLOT(minimize()));

    connect(phone, SIGNAL(volumeUpKeyPressed()), player, SLOT(volumeUp()));
    connect(phone, SIGNAL(volumeDownKeyPressed()), player, SLOT(volumeDown()));

    connect(phone, SIGNAL(batteryChargeChanged(uint)), this, SLOT(updateBatteryLevelInHeader(uint)));
#endif

}

void MainWindow::arrangeWidgets()
{
    headerWidgetLayout = new QHBoxLayout();

    headerWidgetLayout->setContentsMargins(8, 0, 8, 0);
    headerWidgetLayout->setSpacing(0);

#ifdef Q_OS_SYMBIAN
    headerWidgetLayout->addWidget(batteryLabel);
    headerWidgetLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
#endif

    headerWidgetLayout->addWidget(timeLabel);
    headerWidgetLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
    headerWidgetLayout->addWidget(volumeLabel);

    headerWidget->setLayout(headerWidgetLayout);

    headerStackedWidget->addWidget(loadProgressBar);
    headerStackedWidget->addWidget(headerWidget);

    headerStackedWidget->setCurrentIndex(1);

    optionslayout = new QHBoxLayout();
    optionslayout->setContentsMargins(0, 4, 0, 0);
    optionslayout->setSpacing(16);

    optionslayout->addWidget(repeatButton);
    optionslayout->addWidget(shuffleButton);

    trackInfoLayout = new QVBoxLayout();
    trackInfoLayout->setContentsMargins(0, 8, 0, 0);
    trackInfoLayout->setSpacing(12);

    trackInfoLayout->addWidget(artistDirLabel, 0, Qt::AlignCenter);
    trackInfoLayout->addWidget(albumDirLabel, 0, Qt::AlignCenter);
    trackInfoLayout->addWidget(trackFileLabel, 0, Qt::AlignCenter);

    centerlayout = new QHBoxLayout();
    centerlayout->setContentsMargins(0, 6, 0, 12);
    centerlayout->setSpacing(0);

    // FIXME START

    QVBoxLayout* somebox = new QVBoxLayout();
    resetMargins(somebox);
    somebox->setSpacing(0);

    somebox->addLayout(optionslayout);
    somebox->addLayout(trackInfoLayout);

    QWidget* someWidget = new QWidget();
    someWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    someWidget->setStyleSheet(QString::fromUtf8("background: none;"));

    someWidget->setLayout(somebox);
    centerlayout->addWidget(someWidget);
    centerlayout->addWidget(volumeSlider);

    // FIXME END

    prevPlayNextLayout = new QHBoxLayout();
    resetMargins(prevPlayNextLayout);
    prevPlayNextLayout->setSpacing(12);

    prevPlayNextLayout->addWidget(previousTrackButton);
    prevPlayNextLayout->addWidget(playPauseButton);
    prevPlayNextLayout->addWidget(nextTrackButton);

    controlsLayout = new QVBoxLayout();
    controlsLayout->setContentsMargins(0, 0, 0, 12);
    controlsLayout->setSpacing(10);

    controlsLayout->addWidget(seekSlider);
    controlsLayout->addLayout(prevPlayNextLayout);

    playerLayout = new QVBoxLayout();
    playerLayout->setContentsMargins(6, 0, 6, 0);
    playerLayout->setSpacing(0);

    playerLayout->addLayout(centerlayout);
    playerLayout->addLayout(controlsLayout);

    playerWidget->setLayout(playerLayout);

    centerStackedWidget->addWidget(playerWidget);
    centerStackedWidget->addWidget(playListWidget);

    menuLayout = new QHBoxLayout();
    resetMargins(menuLayout);
    menuLayout->setSpacing(0);

    menuLayout->addWidget(listScreenOpenButton);
    menuLayout->addWidget(exitBackButton);

    headerWidgetLayout = new QHBoxLayout();
    resetMargins(headerWidgetLayout);
    headerWidgetLayout->setSpacing(0);

    rootWidgetLayout = new QVBoxLayout();
    resetMargins(rootWidgetLayout);
    rootWidgetLayout->setSpacing(0);

    rootWidgetLayout->addWidget(headerStackedWidget);
    rootWidgetLayout->addWidget(centerStackedWidget);
    rootWidgetLayout->addLayout(menuLayout);

    rootWidget->setLayout(rootWidgetLayout);

    setCentralWidget(rootWidget);
}

void MainWindow::appyStyles()
{
    const int w = size().width();
    const int h = size().height();

    const int maxsize = w > h ? w : h;
    const int minsize = w < h ? w : h;

    QString headerStyle = QString::fromUtf8(
        "max-height: ") + QString::number((uint)(h / 12)) + QString::fromUtf8("px;"
        "background: qlineargradient(x1: 0.5, y1: 0.0, x2: 0.5, y2: 1.0,"
        "stop: 0.0 #605e65, stop: 1.0 #010101);");

    headerStackedWidget->setStyleSheet(headerStyle);

    QString headerFont = QString::fromUtf8(
        "color: white;"
        "background: none;"
        "font-size: ") + QString::number((uint)(minsize / 16)) + QString::fromUtf8("px;"
        "font-weight: bold;");

#ifdef Q_OS_SYMBIAN
    batteryLabel->setStyleSheet(headerFont);
#endif
    volumeLabel->setStyleSheet(headerFont);
    timeLabel->setStyleSheet(headerFont);

    QString progressBarStyle = QString::fromUtf8("QProgressBar {"
        "border: 2px solid black;"
        "border-radius: 4px;"
        "color: white;"
        "font-size: ") + QString::number((uint)(minsize / 20)) + QString::fromUtf8("px;"
        "background: qlineargradient(x1: 0.5, y1: 0.0, x2: 0.5, y2: 1.0,"
        "stop: 0.0 #605e65, stop: 1.0 #010101);"
        "text-align: center;"
    "}"
    "QProgressBar::chunk {"
        "padding: 2px;"
        "background: #3670d0;"
        "border-radius: 4px;"
    "}");

    loadProgressBar->setStyleSheet(progressBarStyle);

    centerlayout->setContentsMargins(0, (int)(h / 40), 0, (int)(h / 25));

    playerLayout->setContentsMargins((int)(w / 40), 0, (int)(w / 40), 0);

    QString repeatButtonStyle = QString::fromUtf8("QPushButton {"
        "background: silver;"
        "image: url(:/icons/repeat.svg);"
        "font-size: ") + QString::number((uint)(minsize / 20)) + QString::fromUtf8("px;"
        "max-width: ") + QString::number((uint)(minsize / 12)) + QString::fromUtf8("px;"
        "max-height: ") + QString::number((uint)(minsize / 12)) + QString::fromUtf8("px;"
        "border: 2px solid black;"
        "border-radius: 4px;"
        "padding: 2px;"
    "}"
    "QPushButton:checked{"
        "background: #3063b8;"
        "color: white;"
    "}"
    "QPushButton:focus {"
        "border: 2px solid #3670d0;"
        "outline: none;"
    "}");

    QString shuffleButtonStyle = QString::fromUtf8("QPushButton {"
        "background: silver;"
        "image: url(:/icons/shuffle.svg);"
        "font-size: ") + QString::number((uint)(minsize / 20)) + QString::fromUtf8("px;"
        "max-width: ") + QString::number((uint)(minsize / 12)) + QString::fromUtf8("px;"
        "max-height: ") + QString::number((uint)(minsize / 12)) + QString::fromUtf8("px;"
        "border: 2px solid black;"
        "border-radius: 4px;"
        "padding: 2px;"
    "}"
    "QPushButton:checked{"
        "background: #3063b8;"
        "color: white;"
    "}"
    "QPushButton:focus {"
        "border: 2px solid #3670d0;"
        "outline: none;"
    "}");

    repeatButton->setStyleSheet(repeatButtonStyle);
    shuffleButton->setStyleSheet(shuffleButtonStyle);

    trackInfoLayout->setContentsMargins(0, (int)(h / 40), 0, 0);
    trackInfoLayout->setSpacing((int)(h / 25));

    QString trackInfoStyle = QString::fromUtf8(
        "color: white;"
        "background: none;"
        "font-size: ") + QString::number((uint)(minsize / 16))  + QString::fromUtf8("px;"
        "font-weight: bold;");

    trackFileLabel->setStyleSheet(trackInfoStyle);
    albumDirLabel->setStyleSheet(trackInfoStyle);
    artistDirLabel->setStyleSheet(trackInfoStyle);

    QString centerStyle = QString::fromUtf8("background: qradialgradient(cx: 0.5, cy: 0.4, radius: 0.5, fx: 0.5, fy: 0.4,"
        "stop: 0 #605e65, stop: 1 #323232, spread: 2 );");

    playerWidget->setStyleSheet(centerStyle);

    QString slidersStyle = QString::fromUtf8("QSlider {"
        "background: none;"
    "}"
    "QSlider:focus {"
        "border: 2px solid #3670d0;"
    "}"
    "QSlider:horizontal {"
        "height: ") + QString::number((uint)(maxsize / 12)) + QString::fromUtf8("px;"
    "}"
    "QSlider:vertical {"
        "width: ") + QString::number((uint)(maxsize / 12)) + QString::fromUtf8("px;"
    "}"
    "QSlider::groove:horizontal {"
        "border: 1px solid #bbb;"
        "background: white;"
        "height: ") + QString::number((uint)(minsize / 24)) + QString::fromUtf8("px;"
        "border-radius: 4px;"
        "}"
    "QSlider::sub-page:horizontal {"
        "background: qlineargradient(x1: 0, y1: 0.5, x2: 1, y2: 0.5,"
        "stop: 0 #3670d0, stop: 1 #5b5b5b);"
        "border: 1px solid #777;"
        "height: ") + QString::number((uint)(minsize / 24)) + QString::fromUtf8("px;"
        "border-radius: 4px;"
    "}"
    "QSlider::handle:horizontal {"
        "background: white;"
        "border: 2px solid black;"
        "width: ") + QString::number((uint)(minsize / 24)) + QString::fromUtf8("px;"
#if QT_VERSION < 0x040700 // Symbian 9.2 fix
        "margin-top: -") + QString::number((uint)(minsize / 60)) + QString::fromUtf8("px;"
        "margin-bottom: -") + QString::number((uint)(minsize / 60)) + QString::fromUtf8("px;"
#else
        "margin-top: -") + QString::number((uint)(minsize / 40)) + QString::fromUtf8("px;"
        "margin-bottom: -") + QString::number((uint)(minsize / 40)) + QString::fromUtf8("px;"
#endif
        "border-radius: 4px;"
    "}"
    "QSlider::groove:vertical {"
        "border: 1px solid #bbb;"
        "background: white;"
        "width: ") + QString::number((uint)(minsize / 24)) + QString::fromUtf8("px;"
        "border-radius: 4px;"
    "}"
    "QSlider::add-page:vertical {"
        "background: qlineargradient(x1: 0.5, y1: 0.0, x2: 0.5, y2: 1.0,"
        "stop: 0 #5b5b5b, stop: 1 #3670d0);"
        "border: 1px solid #777;"
        "width: ") + QString::number((uint)(minsize / 24)) + QString::fromUtf8("px;"
        "border-radius: 4px;"
    "}"
    "QSlider::handle:vertical {"
        "background: white;"
        "border: 2px solid black;"
        "height: ") + QString::number((uint)(minsize / 24)) + QString::fromUtf8("px;"
#if QT_VERSION < 0x040700 // Symbian 9.2 fix
        "margin-right: -") + QString::number((uint)(minsize / 60)) + QString::fromUtf8("px;"
        "margin-left: -") + QString::number((uint)(minsize / 60)) + QString::fromUtf8("px;"
#else
        "margin-right: -") + QString::number((uint)(minsize / 40)) + QString::fromUtf8("px;"
        "margin-left: -") + QString::number((uint)(minsize / 40)) + QString::fromUtf8("px;"
#endif
        "border-radius: 4px;"
    "}");

    seekSlider->setStyleSheet(slidersStyle);
    volumeSlider->setStyleSheet(slidersStyle);

    controlsLayout->setContentsMargins(6, 0, 6, (int)(h / 25));
    controlsLayout->setSpacing((int)(h / 32));

    prevPlayNextLayout->setSpacing((int)(w / 20));

    QString prevButtonStyle = QString::fromUtf8("QPushButton {"
        "image: url(:/icons/prev.svg);"
        "min-height: ") + QString::number((uint)(h / 10)) + QString::fromUtf8("px;"
        "max-width: ") + QString::number((uint)(w / 3) - (uint)((((w / 20) * 2) / 3))) + QString::fromUtf8("px;"

        "border: 2px solid black;"
        "border-radius: 4px;"
        "color: white;"
        "background: qlineargradient(x1: 0.0, y1: 0.5, x2: 1.0, y2: 0.5,"
        "stop: 0.0 #807f7f, stop: 0.5 #d6d5d4, stop: 1.0 #807f7f);"
    "}"
    "QPushButton:focus {"
        "border: 2px solid #3670d0;"
        "outline: none;"
    "}"
    "QPushButton:pressed {"
        "background: qlineargradient(x1: 0.0, y1: 0.5, x2: 1.0, y2: 0.5,"
        "stop: 0.0 #3670d0, stop: 0.5 #d6d5d4, stop: 1.0 #3670d0);"
    "}");

    updatePlayPause();

    QString nextButtonStyle = QString::fromUtf8("QPushButton {"
        "image: url(:/icons/next.svg);"
        "min-height: ") + QString::number((uint)(h / 10)) + QString::fromUtf8("px;"
        "max-width: ") + QString::number((uint)(w / 3) - (uint)((((w / 20) * 2) / 3))) + QString::fromUtf8("px;"
        "border: 2px solid black;"
        "border-radius: 4px;"
        "color: white;"
        "background: qlineargradient(x1: 0.0, y1: 0.5, x2: 1.0, y2: 0.5,"
        "stop: 0.0 #807f7f, stop: 0.5 #d6d5d4, stop: 1.0 #807f7f);"
    "}"
    "QPushButton:focus {"
        "border: 2px solid #3670d0;"
        "outline: none;"
    "}"
    "QPushButton:pressed {"
        "background: qlineargradient(x1: 0.0, y1: 0.5, x2: 1.0, y2: 0.5,"
        "stop: 0.0 #3670d0, stop: 0.5 #d6d5d4, stop: 1.0 #3670d0);"
    "}");

    previousTrackButton->setStyleSheet(prevButtonStyle);
    nextTrackButton->setStyleSheet(nextButtonStyle);

    QString menuLeftButtonsStyle = QString::fromUtf8("QPushButton {"
        "color: white;"
        "min-height: ") + QString::number((uint)(h / 10)) + QString::fromUtf8("px;"
        "text-align: left;"
        "padding-left: 16px;"
        "font-size: ") + QString::number((uint)(minsize / 15)) + QString::fromUtf8("px;"
        "font-weight: bold;"
        "border: 2px solid black;"
        "background: qlineargradient(x1: 0.5, y1: 0.0, x2: 0.5, y2: 1.0,"
        "stop: 0.0 #363636, stop: 1.0 #010101);"
    "}"
    "QPushButton:pressed {"
        "background: qlineargradient(x1: 0.5, y1: 0.0, x2: 0.5, y2: 1.0,"
        "stop: 0.0 #363636, stop: 1.0 #3670d0);"
    "}");

    QString menuRightButtonsStyle = QString::fromUtf8("QPushButton {"
        "color: white;"
        "min-height: ") + QString::number((uint)(h / 10)) + QString::fromUtf8("px;"
        "text-align: right;"
        "padding-right: 16px;"
        "font-size: ") + QString::number((uint)(minsize / 15)) + QString::fromUtf8("px;"
        "font-weight: bold;"
        "border: 2px solid black;"
        "background: qlineargradient(x1: 0.5, y1: 0.0, x2: 0.5, y2: 1.0,"
        "stop: 0.0 #363636, stop: 1.0 #010101);"
    "}"
    "QPushButton:pressed {"
        "background: qlineargradient(x1: 0.5, y1: 0.0, x2: 0.5, y2: 1.0,"
        "stop: 0.0 #363636, stop: 1.0 #3670d0);"
    "}");

    listScreenOpenButton->setStyleSheet(menuLeftButtonsStyle);
    exitBackButton->setStyleSheet(menuRightButtonsStyle);
}

void MainWindow::updatePlayPause()
{
    const int w = size().width();
    const int h = size().height();

    QString playPauseButtonStyle = QString::fromUtf8("QPushButton {"
        "image: url(:/icons/") + QString::fromUtf8(player->isPlaying() ? "pause" : "play" ) + QString::fromUtf8(".svg);"
        "min-height: ") + QString::number((uint)(h / 10)) + QString::fromUtf8("px;"
        "max-width: ") + QString::number((uint)(w / 3) - (uint)((((w / 20) * 2) / 3))) + QString::fromUtf8("px;"
        "border: 2px solid black;"
        "border-radius: 4px;"
        "color: white;"
        "background: qlineargradient(x1: 0.0, y1: 0.5, x2: 1.0, y2: 0.5,"
        "stop: 0.0 #807f7f, stop: 0.5 #d6d5d4, stop: 1.0 #807f7f);"
    "}"
    "QPushButton:focus {"
        "border: 2px solid #3670d0;"
        "outline: none;"
    "}"
    "QPushButton:pressed {"
        "background: qlineargradient(x1: 0.0, y1: 0.5, x2: 1.0, y2: 0.5,"
        "stop: 0.0 #3670d0, stop: 0.5 #d6d5d4, stop: 1.0 #3670d0);"
    "}");

    playPauseButton->setStyleSheet(playPauseButtonStyle);

    playListWidget->setCurrentTrackState(player->isPlaying());
}

void MainWindow::handleKeyEvent(QKeyEvent *e)
{
#ifdef Q_OS_SYMBIAN
    phone->handleKeyEventCode(e->nativeVirtualKey());
#endif
}

void MainWindow::handleDeleteKeyPress()
{
    if(centerStackedWidget->currentIndex() == PLAYLIST_SCREEN_ACTIVE)
    {
        player->removeTrackFromPlaylist(playListWidget->getSelectedItemIndex());

        playListWidget->updateList(getTracksInfo(player->getSourcesPaths()));
        playListWidget->setCurrentPlayingTrackIndex(player->getCurrentTrackIndex());
        playListWidget->setCurrentTrackState(player->isPlaying());
    }
}

void MainWindow::loadSelectedDirectory(QString folderPath)
{
#ifndef Q_OS_SYMBIAN
    QDir selectedDir = folderPath.isEmpty() ? openFolderDialog->directory() : QDir(folderPath);
#else
    QDir selectedDir = QDir(folderPath);
#endif
    QString firstTrackPath = TracksLoader::getFirst(selectedDir);

    player->stop();

    player->clear();

    if(!firstTrackPath.isEmpty())
    {
        player->loadSources(QStringList(firstTrackPath));
        player->setCurrentTrackIndex(0, true);
    }
    else
    {
        playListWidget->updateList(getTracksInfo(QStringList(firstTrackPath)));
        return;
    }

    playListWidget->updateList(getTracksInfo(QStringList(firstTrackPath)));
    playListWidget->setCurrentPlayingTrackIndex(0);

    tracksLoader->setDir(selectedDir);

    headerStackedWidget->setCurrentIndex(0);
//    loadProgressBar->setVisible(true);
    tracksLoader->start();
}

#ifdef Q_OS_SYMBIAN
void MainWindow::minimize()
{
    RWindowGroup* wg = &CCoeEnv::Static()->RootWin();
    wg->SetOrdinalPosition(-1);
}
#endif

void MainWindow::save()
{
    settings->clear();

    quickSave();

    settings->setValue(QString::fromUtf8("playList"), player->getSourcesPaths());
}

void MainWindow::quickSave()
{
    settings->setValue(QString::fromUtf8("currentPlaying"), player->getCurrentTrackIndex());
    settings->setValue(QString::fromUtf8("currentTime"), player->getSeek());

    settings->setValue(QString::fromUtf8("currentVolume"), player->getVolume());

    settings->setValue(QString::fromUtf8("repeatIsEnable"), player->getIsRepeat());
    settings->setValue(QString::fromUtf8("shuffleIsEnable"), player->getIsShuffle());
}

void MainWindow::load()
{
    QStringList tracksPaths = settings->value(QString::fromUtf8("playList"), QStringList()).toStringList();

    qreal vol = settings->value(QString::fromUtf8("currentVolume"), 0.3).toReal();

    player->setVolume(vol);

    QStringList tracksPathsClear;

    foreach(QString fi, tracksPaths)
    {
        if(QFileInfo(fi).exists())
        {
            tracksPathsClear.append(fi);
        }
    }

    if(!tracksPathsClear.isEmpty())
    {
        player->loadSources(tracksPathsClear);

        playListWidget->updateList(getTracksInfo(tracksPathsClear));

        player->setCurrentTrackIndex(settings->value(QString::fromUtf8("currentPlaying"), 0).toInt(), false);

        player->seekWhenLoad(settings->value(QString::fromUtf8("currentTime"), 0).toLongLong());
    }

    repeatButton->setChecked(settings->value(QString::fromUtf8("repeatIsEnable"), false).toBool());
    shuffleButton->setChecked(settings->value(QString::fromUtf8("shuffleIsEnable"), false).toBool());
}

QList<TrackInfo> MainWindow::getTracksInfo(QStringList tracksPaths)
{
    QList<TrackInfo> info;

    foreach(QString tfn, tracksPaths)
    {
        info.append(TrackInfo(tfn));
    }

    return info;
}

void MainWindow::resetMargins(QLayout *l)
{
    l->setContentsMargins(0, 0, 0, 0);
}

void MainWindow::resizeEvent(QResizeEvent *)
{
    updateCurrentPlayingTrackInfo();
    appyStyles();
//    qDebug() << size().width() << " " << size().height();
}

void MainWindow::showPlaylistOrOpenFolder()
{
    if(centerStackedWidget->currentIndex() == PLAYER_SCREEN_ACTIVE)
    {
        listScreenOpenButton->setText(QString::fromUtf8("Open"));
        exitBackButton->setText(QString::fromUtf8("Back"));
        centerStackedWidget->setCurrentIndex(PLAYLIST_SCREEN_ACTIVE);
        playListKeyEventPriority = true;
    }
    else
    {
#ifdef Q_OS_SYMBIAN
        TFileName folder;

        // Create select memory dialog
        CAknMemorySelectionDialog* memDlg = CAknMemorySelectionDialog::NewL(ECFDDialogTypeSelect, ETrue);
        CAknMemorySelectionDialog::TMemory memory = CAknMemorySelectionDialog::EPhoneMemory;

        // Create select folder dialog
        CAknFileSelectionDialog* dlg = CAknFileSelectionDialog::NewL(ECFDDialogTypeCopy);

        // some dialog customizations:
        dlg->SetTitleL(_L("Select folder"));
        dlg->SetLeftSoftkeyFolderL(_L("Select"));
        dlg->SetRightSoftkeyRootFolderL(_L("Back")); // for root folder

        for (;;)
        {
            if (memDlg->ExecuteL(memory) == CAknFileSelectionDialog::ERightSoftkey)
            {
                // cancel selection
                break;
            }

            if (memory == CAknMemorySelectionDialog::EMemoryCard)
            {
                folder = PathInfo::MemoryCardRootPath();
            }
            else
            {
                folder = PathInfo::PhoneMemoryRootPath();
            }

            if (dlg->ExecuteL(folder))
            {
                // we got our folder and finish loop
                QString selectedFolderPath((QChar*)folder.Ptr(),folder.Length());
                loadSelectedDirectory(selectedFolderPath);

                break;
            }
        }

        delete memDlg;
        delete dlg;
#else
        openFolderDialog->exec();
#endif
        listScreenOpenButton->setText(QString::fromUtf8("List"));
        exitBackButton->setText(QString::fromUtf8("Exit"));
        centerStackedWidget->setCurrentIndex(PLAYER_SCREEN_ACTIVE);
        playListKeyEventPriority = false;

    }
}

void MainWindow::exitOrBack()
{
    if(centerStackedWidget->currentIndex() == PLAYER_SCREEN_ACTIVE)
    {
        QMessageBox exitDialog;
        exitDialog.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        exitDialog.setText(QString::fromUtf8("Are you sure?"));

//        exitDialog.setStyleSheet(QString::fromUtf8("QMessageBox {"
//            "background: black;"
//            "border: 2px solid white;"
//            "border-radius: 4px;"
//            "font-size: ") + QString::number(size().width() / 16)  + QString::fromUtf8("px;"
//            "font-weight: bold;"
//            "color: white;"
//        "}"));

//        QSize mSize = exitDialog.sizeHint();
//        QRect screenRect = QDesktopWidget().screen()->rect();
//        exitDialog.move( QPoint( screenRect.width()/2 - mSize.width()/2, screenRect.height()/2 - mSize.height()/2 ) );

        if(exitDialog.exec() == QMessageBox::No)
            return;

        save();
        close();
    }
    else
    {
        listScreenOpenButton->setText(QString::fromUtf8("List"));
        exitBackButton->setText(QString::fromUtf8("Exit"));
        centerStackedWidget->setCurrentIndex(PLAYER_SCREEN_ACTIVE);
        playListKeyEventPriority = false;
    }
}

void MainWindow::isRepeatChanged(bool isRepeat)
{
    player->setReapeat(isRepeat);
}

void MainWindow::isShuffleChanged(bool isShuffle)
{
    player->setShuffle(isShuffle);
}

void MainWindow::loadComplete()
{
    QStringList tracksPaths = tracksLoader->getTrackList();

    if(!tracksPaths.isEmpty())
    {
        player->addAndLoadSources(tracksPaths.mid(1));
    }

    playListWidget->updateList(tracksLoader->getTracksInfo());

    headerStackedWidget->setCurrentIndex(1);
//    loadProgressBar->setVisible(false);
    loadProgressBar->setValue(0);

    playListWidget->setCurrentPlayingTrackIndex(player->getCurrentTrackIndex());
}

#ifdef Q_OS_SYMBIAN
void MainWindow::phoneCall()
{
    currentSeek = player->getSeek();
    wasPlayingBeforeCall = player->isPlaying();
    player->pause();
}

void MainWindow::callFinished()
{
    if(wasPlayingBeforeCall)
    {
        player->play();
        player->setSeek(currentSeek);
    }
}
#endif

void MainWindow::updateCurrentPlayingTrackInfo()
{
    TrackInfo trackInfo = player->getCurrentTrackInfo();

    QString trackName = trackInfo.getTrackName();

    QString albumName = trackInfo.getAlbumName();

    QString artistName = trackInfo.getArtistName();

    const int len = size().width() / 10;

    artistDirLabel->setText(artistName.size() > len ? artistName.left(len) + QString::fromUtf8("...") : artistName);
    albumDirLabel->setText(albumName.size() > len ? albumName.left(len) + QString::fromUtf8("...") : albumName);
    trackFileLabel->setText(trackName.size() > len ? trackName.left(len) + QString::fromUtf8("...") : trackName);

    playListWidget->setCurrentPlayingTrackIndex(player->getCurrentTrackIndex());
}

#ifdef Q_OS_SYMBIAN
void MainWindow::updateBatteryLevelInHeader(uint level)
{
    batteryLabel->setText(QString::fromUtf8("bat: ") + QString::number(level) + QString::fromUtf8("%"));
}
#endif

void MainWindow::updateVolumeInHeader()
{
    volumeLabel->setText(QString::fromUtf8("vol: ") + QString::number((int)(player->getVolume() * 100 + 0.1)) + QString::fromUtf8("%"));
}

void MainWindow::updateTimeInHeader()
{
    timeLabel->setText(QTime::currentTime().toString(QString::fromUtf8("hh:mm")));
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    if(!playListKeyEventPriority)
    {
        handleKeyEvent(e);
    }
}
