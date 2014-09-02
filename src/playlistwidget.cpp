#include "playlistwidget.h"

PlayListWidget::PlayListWidget(QWidget *parent) :
    QListWidget(parent)
{
    currentPlayingIndex = -1;

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setStyleSheet(QString::fromUtf8("QListWidget {"
                                    "background: qradialgradient(cx: 0.5, cy: 0.4, radius: 0.5, fx: 0.5, fy: 0.4,"
                                    "stop: 0 #605e65, stop: 1 #323232, spread: 2 );"
                                    "}"));

    connect(this, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(itemClicked(QListWidgetItem*))); // NEED CHECK
}

int PlayListWidget::getSelectedItemIndex()
{
    return tracksItems.indexOf(currentItem());
}

void PlayListWidget::keyPressEvent(QKeyEvent *e)
{

#ifdef Q_OS_SYMBIAN
    if(e->nativeVirtualKey() != 63554 && e->nativeVirtualKey() != 63555) // left or right soft key
    {
        QListWidget::keyPressEvent(e);
    }
#endif

    emit keyPressed(e);
}

void PlayListWidget::updateList(QList<TrackInfo> info)
{
    clear();

    tracksItems.clear();

    QString pastFolderName;

    QString currentFolderName;
    QString currentFileName;

    QListWidgetItem* tFolderNameItem;
    QListWidgetItem* tTrackItem;

    foreach(TrackInfo tInfo, info)
    {
        currentFolderName = tInfo.getAlbumName();

        if(pastFolderName != currentFolderName)
        {
            tFolderNameItem = new QListWidgetItem(currentFolderName, this);
            tFolderNameItem->setFlags(Qt::NoItemFlags);
            tFolderNameItem->setForeground(QBrush(QColor(255, 255, 255)));
            insertItem(count(), tFolderNameItem);
        }

        currentFileName = tInfo.getTrackName();

        tTrackItem = new QListWidgetItem(currentFileName, this);
        tTrackItem->setForeground(QBrush(QColor(255, 255, 255)));
        insertItem(count(), tTrackItem);
        tracksItems.append(tTrackItem);


        pastFolderName = currentFolderName;
    }

    QIcon playIcon = style()->standardIcon(QStyle::SP_MediaPlay);
    QPixmap emptyPixmap = QPixmap(playIcon.pixmap(64, 64).width(), playIcon.pixmap(64, 64).height());
    emptyPixmap.fill(QColor(0, 0, 0, 0));
    QIcon emptyIcon = QIcon(emptyPixmap);

    foreach(QListWidgetItem* item, tracksItems)
    {
        item->setIcon(emptyIcon);
    }
}

void PlayListWidget::setCurrentPlayingTrackIndex(int index)
{
    if(index >= tracksItems.size() || index < 0)
    {
        return;
    }

    currentPlayingIndex = index;

    QIcon playIcon(":/icons/play.svg");
    QPixmap emptyPixmap = QPixmap(playIcon.pixmap(64, 64).width(), playIcon.pixmap(64, 64).height());
    emptyPixmap.fill(QColor(0, 0, 0, 0));
    QIcon emptyIcon = QIcon(emptyPixmap);

    QFont normalFont;

    foreach(QListWidgetItem* item, tracksItems)
    {
        item->setIcon(emptyIcon);
        item->setFont(normalFont);
    }

    tracksItems.at(index)->setIcon(playIcon);
    QFont boldFont;
    boldFont.setBold(true);
    tracksItems.at(index)->setFont(boldFont);
    tracksItems.at(index)->setForeground(QBrush(QColor(255, 255, 255)));


    scrollToItem(tracksItems.at(index), QAbstractItemView::PositionAtCenter);

    setCurrentItem(tracksItems.at(index));
}

void PlayListWidget::setCurrentTrackState(bool isPlaying)
{
    if(currentPlayingIndex < 0 || currentPlayingIndex > tracksItems.size() - 1)
        return;

    if(isPlaying)
    {
        QIcon playIcon(":/icons/play.svg");
        tracksItems.at(currentPlayingIndex)->setIcon(playIcon);
    }
    else
    {
        QIcon pauseIcon(":/icons/pause.svg");
        tracksItems.at(currentPlayingIndex)->setIcon(pauseIcon);
    }
}

void PlayListWidget::itemClicked(QListWidgetItem *item)
{
    int res = tracksItems.indexOf(item);

    if(res != - 1)
    {
        emit trackSelected(res);
    }
}
