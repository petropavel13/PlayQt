SOURCES += \
    main.cpp \
    mainwindow.cpp \
    playlistwidget.cpp \
    tracksloader.cpp \
    trackinfo.cpp \
    player.cpp \
    phononplayer.cpp



HEADERS += \
    mainwindow.h \
    playlistwidget.h \
    tracksloader.h \
    trackinfo.h \
    player.h \
    phononplayer.h \



QT += phonon

symbian: {
# lock keypad
    LIBS += -lavkon\
# minimize
            -lws32\
            -lcone\
# media keys
            -lremconcoreapi\
            -lremconinterfacebase\
# folder dialog
            -lcommondialogs\
            -lplatformenv\
# phone calls
            -letel3rdparty

SOURCES += \
    phone.cpp \
    symbian/symbianphone.cpp \
    symbian/activebatterylistener.cpp \
    symbian/activecallslistener.cpp

HEADERS += \
    phone.h \
    symbian/symbianphone.h \
    symbian/activebatterylistener.h \
    symbian/activecallslistener.h
}

RESOURCES += \
    icons.qrc
