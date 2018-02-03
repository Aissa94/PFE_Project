#-------------------------------------------------
#
# Project created by QtCreator 2013-01-14T16:15:08
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FeaturePointsComparison
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

VERSION = 1.0

RC_ICONS = me.ico

win32 {
    INCLUDEPATH += "C:\\Program Files\\opencv\\build\\include" \

    CONFIG(debug,debug|release) {
        LIBS += -L"C:\\Program Files\\opencv\\build\\x86\\vc12\\lib" \
            -lopencv_core2411d \
            -lopencv_imgproc2411d \
            -lopencv_features2d2411d \
            -lopencv_flann2411d \
            -lopencv_nonfree2411d \
            -lopencv_highgui2411d \
            -lopencv_calib3d2411d
    }

    CONFIG(release,debug|release) {
        DEFINES += QT_NO_WARNING_OUTPUT QT_NO_DEBUG_OUTPUT
        LIBS += -L"C:\\Program Files\\opencv\\build\\x86\\vc12\\lib" \
            -lopencv_core2411 \
            -lopencv_imgproc2411 \
            -lopencv_features2d2411 \
            -lopencv_flann2411 \
            -lopencv_nonfree2411 \
            -lopencv_highgui2411 \
            -lopencv_calib3d2411
    }
}

win64 {
    INCLUDEPATH += "C:\\Program Files\\opencv\\build\\include" \

    CONFIG(debug,debug|release) {
        LIBS += -L"C:\\Program Files\\opencv\\build\\x64\\vc12\\lib" \
            -lopencv_core2411d \
            -lopencv_imgproc2411d \
            -lopencv_features2d2411d \
            -lopencv_flann2411d \
            -lopencv_nonfree2411d \
            -lopencv_highgui2411d \
            -lopencv_calib3d2411d
    }

    CONFIG(release,debug|release) {
        DEFINES += QT_NO_WARNING_OUTPUT QT_NO_DEBUG_OUTPUT
        LIBS += -L"C:\\Program Files\\opencv\\build\\x64\\vc12\\lib" \
            -lopencv_core2411 \
            -lopencv_imgproc2411 \
            -lopencv_features2d2411 \
            -lopencv_flann2411 \
            -lopencv_nonfree2411 \
            -lopencv_highgui2411 \
            -lopencv_calib3d2411
    }
}

