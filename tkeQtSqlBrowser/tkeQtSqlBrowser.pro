CONFIG		+= release
CONFIG		-= debug debug_and_release debug_and_release_target

TEMPLATE        = app
TARGET          = tkeQtSqlBrowser

DEPENDPATH +=   . $$[QT_INSTALL_PREFIX]/demos/sqlbrowser
INCLUDEPATH +=  . $$[QT_INSTALL_PREFIX]/demos/sqlbrowser

QT              += sql

HEADERS         = browser.h connectionwidget.h qsqlconnectiondialog.h
SOURCES         = ./main.cpp browser.cpp connectionwidget.cpp qsqlconnectiondialog.cpp

FORMS           = browserwidget.ui qsqlconnectiondialog.ui

DESTDIR =   ../bin

OBJECTS_DIR = ../tmp/tkeQtSqlBrowser
MOC_DIR = ../tmp/tkeQtSqlBrowser
UI_HEADERS_DIR = ../tmp/tkeQtSqlBrowser
UI_SOURCES_DIR = ../tmp/tkeQtSqlBrowser
RCC_DIR = ../tmp/tkeQtSqlBrowser