APP_NAME = Jukebox

CONFIG += qt warn_on cascades10
LIBS += -lCBHelper -lQtLocationSubset
LIBS += -L ../libs/ -L../libs/cloudbase-libs -lQtXml

INCLUDEPATH += ../libs/include/

include(config.pri)
