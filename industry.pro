#-------------------------------------------------
#
# Project created by QtCreator 2016-03-09T15:35:32
#
#-------------------------------------------------




QT       += core gui
QT       += sql


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET =  PKU-DREAM
TEMPLATE = app

QMAKE_CXXFLAGS_RELEASE = -MD
QMAKE_CXXFLAGS_DEBUG = -MDd

INCLUDEPATH += $$quote(C:\software\coding toools\opencv\build\include)
INCLUDEPATH += $$quote(C:\software\coding toools\opencv\build\include\opencv)
INCLUDEPATH += $$quote(C:\software\coding toools\opencv\build\include\opencv2)
INCLUDEPATH += $$quote(C:\software\small tools\tiff\include)
INCLUDEPATH += $$quote(C:\Program Files (x86)\DCMTK\include)

LIBS+=  wsock32.lib \
        NetAPI32.Lib \
        AdvAPI32.Lib \
        zlib.lib

#LIBS += $$quote(C:\software\coding toools\opencv\build\x86\vc12\lib\opencv_core2410d.lib)
#LIBS += $$quote(C:\software\coding toools\opencv\build\x86\vc12\lib\opencv_highgui2410d.lib)
#LIBS += $$quote(C:\software\coding toools\opencv\build\x86\vc12\lib\opencv_imgproc2410d.lib)
#LIBS += $$quote(C:\software\coding toools\opencv\build\x86\vc12\lib\opencv_stitching2410d.lib)
#LIBS += $$quote(C:\software\coding toools\opencv\build\x86\vc12\lib\opencv_legacy2410d.lib)
#LIBS += $$quote(C:\software\small tools\tiff\lib\libtiff.lib)
#LIBS+=  $$quote(C:\Program Files (x86)\DCMTK_DEBUG\lib\ofstd.lib) \
#        $$quote(C:\Program Files (x86)\DCMTK_DEBUG\lib\oflog.lib) \
#        $$quote(C:\Program Files (x86)\DCMTK_DEBUG\lib\dcmdata.lib)

LIBS += $$quote(C:\software\coding toools\opencv\build\x86\vc12\lib\opencv_core2410.lib)
LIBS += $$quote(C:\software\coding toools\opencv\build\x86\vc12\lib\opencv_highgui2410.lib)
LIBS += $$quote(C:\software\coding toools\opencv\build\x86\vc12\lib\opencv_imgproc2410.lib)
LIBS += $$quote(C:\software\coding toools\opencv\build\x86\vc12\lib\opencv_stitching2410.lib)
LIBS += $$quote(C:\software\coding toools\opencv\build\x86\vc12\lib\opencv_legacy2410.lib)
LIBS += $$quote(C:\software\small tools\tiff\lib\libtiff.lib)
LIBS+=  $$quote(C:\Program Files (x86)\DCMTK\lib\ofstd.lib) \
        $$quote(C:\Program Files (x86)\DCMTK\lib\oflog.lib) \
        $$quote(C:\Program Files (x86)\DCMTK\lib\dcmdata.lib)




SOURCES += main.cpp \
        mainwindow.cpp \
        hist.cpp \
        c_gradation.cpp \
    myslider.cpp \
    ui_hist_hdr.cpp \
    ui_local_hdr.cpp \
    parameter.cpp \
    ui_imageinfo.cpp \
    ui_contrast.cpp \
    ui_denoise.cpp \
    ui_about.cpp \
    processing.cpp \
    algorithm.cpp \
    dcmtkfile.cpp \
    recdcmtkfile.cpp \
    database.cpp \
    buttonactionadapter.cpp \
    ui_sliderchoose.cpp \
    ui_emboss.cpp \
    homo_clip.cpp \
    showimgwindow.cpp \
    ui_compare_show.cpp


HEADERS  += dcmtkfile.h \
            mainwindow.h \
            hist.h \
            c_gradation.h \
    myslider.h \
    ui_hist_hdr.h \
    ui_local_hdr.h \
    ui_imageinfo.h \
    ui_contrast.h \
    ui_denoise.h \
    ui_about.h \
    processing.h \
    algorithm.h \
    recdcmtkfile.h \
    database.h \
    buttonactionadapter.h \
    ui_sliderchoose.h \
    ui_emboss.h \
    homo_clip.h \
    ZQ_CubicInterpolation.h \
    ZQ_DoubleImage.h \
    ZQ_ImageIO.h \
    ZQ_ImageProcessing.h \
    ZQ_MathBase.h \
    ZQ_PCGSolver.h \
    ZQ_SparseMatrix.h \
    ZQ_StructureFromTexture.h \
    ZQ_StructureFromTextureOptions.h \
    ZQ_taucs.h \
    ZQ_TaucsBase.h \
    showimgwindow.h \
    ui_compare_show.h


FORMS    += \
            c_gradation.ui \
    ui_hist_hdr.ui \
    ui_local_hdr.ui \
    ui_imageinfo.ui \
    ui_contrast.ui \
    ui_denoise.ui \
    ui_about.ui \
    processing.ui \
    mainwindow.ui \
    ui_sliderchoose.ui \
    ui_emboss.ui \
    homo_clip.ui \
    showimgwindow.ui \
    ui_compare_show.ui

RESOURCES += \
    src.qrc





