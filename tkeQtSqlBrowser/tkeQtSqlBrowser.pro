CONFIG		+= release
CONFIG		-= debug debug_and_release debug_and_release_target

CONFIG(debug, debug|release) {
    win32: TARGET = $$join(TARGET,,,d)
    unix: TARGET = $$join(TARGET,,,_debug)
}

TEMPLATE        = app
TARGET          = tkeQtSqlBrowser

CONFIG(debug, debug|release) {
    win32: TARGET = $$join(TARGET,,,d)
    unix: TARGET = $$join(TARGET,,,_debug)
}

DEPENDPATH +=   . $$[QT_INSTALL_PREFIX]/demos/sqlbrowser
INCLUDEPATH +=  . $$[QT_INSTALL_PREFIX]/demos/sqlbrowser

QT              += sql

HEADERS         = browser.h connectionwidget.h qsqlconnectiondialog.h
SOURCES         = ./main.cpp browser.cpp connectionwidget.cpp qsqlconnectiondialog.cpp

FORMS           = browserwidget.ui qsqlconnectiondialog.ui

DESTDIR =   ../bin

CONFIG(debug, debug|release) {
    OBJECTS_DIR = ../tmpd/tkeQtSqlBrowser
    MOC_DIR = ../tmpd/tkeQtSqlBrowser
    UI_HEADERS_DIR = ../tmpd/tkeQtSqlBrowser
    UI_SOURCES_DIR = ../tmpd/tkeQtSqlBrowser
    RCC_DIR = ../tmpd/tkeQtSqlBrowser
}
else {
    OBJECTS_DIR = ../tmp/tkeQtSqlBrowser
    MOC_DIR = ../tmp/tkeQtSqlBrowser
    UI_HEADERS_DIR = ../tmp/tkeQtSqlBrowser
    UI_SOURCES_DIR = ../tmp/tkeQtSqlBrowser
    RCC_DIR = ../tmp/tkeQtSqlBrowser
}
