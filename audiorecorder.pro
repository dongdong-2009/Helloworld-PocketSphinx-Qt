TEMPLATE = app
TARGET = audiorecorder

QT += multimedia

win32:INCLUDEPATH += $$PWD

HEADERS = \
    audiorecorder.h \
    qaudiolevel.h \
    recorder.h

SOURCES = \
    main.cpp \
    audiorecorder.cpp \
    qaudiolevel.cpp \
    recorder.cpp

FORMS += audiorecorder.ui

target.path = $$[QT_INSTALL_EXAMPLES]/multimedia/audiorecorder
INSTALLS += target

QT+=widgets
include(../../shared/shared.pri)
