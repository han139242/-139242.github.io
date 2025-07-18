SOURCES += \
    $$PWD/logindialog.cpp \
    $$PWD/main.cpp \
    $$PWD/propertymanager.cpp \
    $$PWD/propertyworker.cpp \
    $$PWD/resident.cpp \
    $$PWD/settings.cpp

FORMS += \
    $$PWD/logindialog.ui \
    $$PWD/settings.ui

HEADERS += \
    $$PWD/logindialog.h \
    $$PWD/propertymanager.h \
    $$PWD/propertyworker.h \
    $$PWD/resident.h \
    $$PWD/settings.h

include($$PWD/propertyworkertool/propertyworkertool.pri)
include($$PWD/residenttool/residenttool.pri)
include($$PWD/propertymanagertool/propertymanagertool.pri)
