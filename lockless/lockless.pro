#-------------------------------------------------
#
# Project created by QtCreator 2014-09-22T13:30:03
#
#-------------------------------------------------

TARGET = lockless
TEMPLATE = lib

QT -= core gui

CONFIG += lib_bundle
CONFIG += c++11

QMAKE_CXXFLAGS += -std=c++11

DEFINES += ENABLE_AUTO_TRACE

INCLUDEPATH += /usr/include
INCLUDEPATH += /usr/local/include

SOURCES += \
    executor.cpp \
    server_bootstrap.cpp \
    server_socket_channel.cpp \
    object.cpp \
    command.cpp \
    runtime_context.cpp \
    logger.cpp \
    channel_handler.cpp \
    channel_data.cpp \
    client_bootstrap.cpp \
    client_socket_channel.cpp \
    channel_handler_context.cpp \
    channel_pipeline.cpp \
    channel_pipeline_context.cpp

HEADERS += \
    defines.h \
    executor.h \
    server_bootstrap.h \
    server_socket_channel.h \
    object.h \
    command.h \
    runtime_context.h \
    logger.h \
    channel_handler.h \
    channel_data.h \
    client_bootstrap.h \
    client_socket_channel.h \
    channel_handler_context.h \
    channel_pipeline.h \
    channel_pipeline_context.h \
    channel_pipeline_initializer.h \
    publish_subscribe.h \
    lockless.h


LIBS += -L/usr/local/lib
LIBS += -lboost_system
LIBS += -lboost_thread

macx {
    DESTDIR = $$(HOME)/Library/Frameworks
    FRAMEWORK_HEADERS.version = Versions
    FRAMEWORK_HEADERS.files += \
        defines.h \
        executor.h \
        server_bootstrap.h \
        server_socket_channel.h \
        object.h \
        command.h \
        runtime_context.h \
        logger.h \
        channel_handler.h \
        channel_data.h \
        client_bootstrap.h \
        client_socket_channel.h \
        channel_handler_context.h \
        channel_pipeline.h \
        channel_pipeline_context.h \
        channel_pipeline_initializer.h \
        publish_subscribe.h \
        lockless.h
    FRAMEWORK_HEADERS.path = Headers/lockless
    QMAKE_BUNDLE_DATA += FRAMEWORK_HEADERS
}
else:unix {
    YP_BASE_DIR = $$(HOME)/local
    DESTDIR = $$YP_BASE_DIR/lib
    system(mkdir -p $$YP_BASE_DIR/include/lockless)
    system(cp -f $$HEADERS $$YP_BASE_DIR/include/lockless)
}

OTHER_FILES +=

