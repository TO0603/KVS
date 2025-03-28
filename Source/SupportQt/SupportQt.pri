#*****************************************************************************
#  $Id: SupportQt.pri 1497 2013-04-04 07:17:54Z naohisa.sakamoto@gmail.com $
#*****************************************************************************

#=============================================================================
#  KVS directory
#=============================================================================
KVS_DIR = $$(KVS_DIR)

isEmpty( KVS_DIR ) {
    error( "KVS_DIR is not defined." )
}


#=============================================================================
#  template
#=============================================================================
TEMPLATE = lib


#=============================================================================
#  target
#=============================================================================
win32 {
    TARGET = kvsSupportQt
} else {
    TARGET = kvsSupportQt
}


#=============================================================================
#  configuration valiable
#=============================================================================
CONFIG += warn_off opengl staticlib
win32 {
    CONFIG += static_runtime
    Release:QMAKE_CXXFLAGS_RELEASE += /MT
    Release:QMAKE_LFLAGS_RELEASE += /MT
    Debug:QMAKE_CXXFLAGS_DEBUG += /MTd
    Debug:QMAKE_LFLAGS_DEBUG += /MTd
}
QT     += opengl

greaterThan( QT_MAJOR_VERSION, 5 ) {
QT += openglwidgets
}


#=============================================================================
#  definition
#=============================================================================
win32 {
    DEFINES += WIN32 _MBCS NOMINMAX _SCL_SECURE_NO_DEPRECATE _CRT_SECURE_NO_DEPRECATE _CRT_NONSTDC_NO_DEPRECATE
}

cygwin* {
    DEFINES += NOMINMAX
}

DEFINES += KVS_SUPPORT_QT
win32 {
DEFINES += KVS_ENABLE_GLEW
}

#=============================================================================
#  include path
#=============================================================================
INCLUDEPATH += ..

KVS_GLEW_DIR = $$(KVS_GLEW_DIR)
!isEmpty( KVS_GLEW_DIR )
{
    INCLUDEPATH += $$KVS_GLEW_DIR/include
}


#=============================================================================
#  install
#=============================================================================
target.path = $$KVS_DIR/lib

INSTALL_HEADERS.files = Qt.h
INSTALL_HEADERS.path  = $$KVS_DIR/include/SupportQt

INSTALLS += target INSTALL_HEADERS


#=============================================================================
#  header and source
#=============================================================================
include(Event/Event.pri)
include(Viewer/Viewer.pri)
include(Widget/Widget.pri)
