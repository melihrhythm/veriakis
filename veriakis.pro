QT += core gui widgets serialport printsupport


TARGET = SerialPortExample
TEMPLATE = app

SOURCES += main.cpp \
           mainwindow.cpp \
           serialmodel.cpp

HEADERS += mainwindow.h \
           serialmodel.h

FORMS += mainwindow.ui
