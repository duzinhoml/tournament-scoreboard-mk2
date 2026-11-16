QT += core gui widgets
CONFIG += c++17
TEMPLATE = app
TARGET = tournament-scoreboard

INCLUDEPATH += include

SOURCES += \
    src/main.cpp \
    src/TournamentFileManager.cpp \
    src/Tournament.cpp

HEADERS += \
    include/Tournament.h \
    include/TournamentFileManager.h
