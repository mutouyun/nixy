#-------------------------------------------------
#
# Project created by QtCreator 2013-09-25T10:38:15
#
#-------------------------------------------------

QT      += core
QT      += script
QT      += xml
QT      -= gui

TARGET  = nixyx
CONFIG  += console
CONFIG  -= app_bundle

TEMPLATE = app

DESTDIR = ../build
OBJECTS_DIR = ../build/!tmp/nixyx

SOURCES += main.cpp \
    config.cpp \
    platform.cpp

HEADERS += \
    config.h \
    platform.h \
    platform_base.h \
    platform_gcc.h \
    solution.h \
    config_base.h \
    config_ini.h \
    config_js.h \
    platform_vc.h \
    tools.h \
    policy_vc10.h \
    policy_vc8.h \
    policy_vc9.h \
    policy_vc_base.h \
    policy_vc11.h
