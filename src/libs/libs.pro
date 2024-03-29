TEMPLATE  = subdirs
CONFIG   += ordered
QT += core gui

# aggregation and extensionsystem are directly in src.pro
# because of dependencies of app
SUBDIRS   = \
    3rdparty \
    utils \
    utils/process_stub.pro \
    languageutils \
    symbianutils \
    cplusplus \
    qmljs \
    qmldebug \
    glsl \
    qmleditorwidgets \
    qtcomponents/styleitem \
    zeroconf
win32:SUBDIRS += utils/process_ctrlc_stub.pro

# Windows: Compile Qt Creator CDB extension if Debugging tools can be detected.    
win32 {
    include(qtcreatorcdbext/cdb_detect.pri)
    exists($$CDB_PATH):SUBDIRS += qtcreatorcdbext
}
