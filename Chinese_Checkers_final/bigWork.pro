QT       += core gui
QT       +=network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    board.cpp \
    game2player.cpp \
    game3play.cpp \
    gameover.cpp \
    gameplay.cpp \
    gameserver.cpp \
    iserror.cpp \
    issuccessful.cpp \
    joinroomclient.cpp \
    main.cpp \
    mainwindow.cpp \
    networkdata.cpp \
    networkserver.cpp \
    networksocket.cpp \
    piece2.cpp \
    players.cpp \
    regulation.cpp \
    serverorclient.cpp \
    waitingroom.cpp

HEADERS += \
    board.h \
    game2player.h \
    game3play.h \
    gameover.h \
    gameplay.h \
    gameserver.h \
    iserror.h \
    issuccessful.h \
    joinroomclient.h \
    mainwindow.h \
    networkdata.h \
    networkserver.h \
    networksocket.h \
    piece2.h \
    players.h \
    regulation.h \
    serverorclient.h \
    waitingroom.h

FORMS += \
    game2player.ui \
    game3play.ui \
    gameover.ui \
    gameplay.ui \
    gameserver.ui \
    iserror.ui \
    issuccessful.ui \
    joinroomclient.ui \
    mainwindow.ui \
    regulation.ui \
    serverorclient.ui \
    waitingroom.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    res/Red-help-book.png \
    res/chess.png \
    res/chess2.png \
    res/chess2s.png \
    res/chess3.png \
    res/chess3s.png \
    res/chess4.png \
    res/chess4s.png \
    res/chess5.png \
    res/chess5s.png \
    res/chess.png \
    res/chess6s.png \
    res/chessboard.jpg \
    res/chesss.png \
    res/exit_power_quit.png \
    res/fail.png \
    res/pic1.png \
    res/pic2.png \
    res/pic3.png \
    res/pic4.png \
    res/regulation.png \
    res/win.png

RESOURCES += \
    res.qrc
