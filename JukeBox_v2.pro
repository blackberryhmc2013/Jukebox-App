APP_NAME = JukeBox_v2

CONFIG += qt warn_on cascades10

LIBS += -lbbnetwork -lbbsystem -lcurl -lbb -lQtLocationSubset -lbbdevice 
LIBS += -L../libs/cloudbase-libs -lCBHelper -lQtXml -lbtapi

include(config.pri)
