#-------------------------------------------------
#
# Project created by QtCreator 2016-09-11T12:31:25
#
#-------------------------------------------------

QT       += core gui widgets multimedia

include(../common.pri)

TARGET = custom_signal

SOURCES += \
    core/signal_item.cpp \
    gui/settings_dialog.cpp \
    custom_signal_plugin.cpp \
    core/custom_signal_settings.cpp \
    gui/signal_config_dialog.cpp

HEADERS += \
    core/signal_item.h \
    gui/settings_dialog.h \
    custom_signal_plugin.h \
    core/custom_signal_settings.h \
    gui/signal_config_dialog.h

FORMS += \
    gui/settings_dialog.ui \
    gui/signal_config_dialog.ui

TRANSLATIONS += \
    custom_signal_ru.ts

include(../../qm_gen.pri)

DISTFILES += custom_signal.json

RESOURCES += \
    custom_signal.qrc

win32:RC_FILE = custom_signal.rc
