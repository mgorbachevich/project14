QT += quick sql widgets core gui network

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# SSL for android https://github.com/KDAB/android_openssl
android: include(Android/android_openssl-master/openssl.pri)

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/Android

INCLUDEPATH += \
    DB \
    HTTP \
    Managers \
    Models \
    Settings \

SOURCES += \
    DB/dbtable.cpp \
    DB/jsonparser.cpp \
    DB/productdbtable.cpp \
    DB/userdbtable.cpp \
    HTTP/httpclient.cpp \
    HTTP/httpserver.cpp \
    HTTP/socketthread.cpp \
    Managers/appmanager.cpp \
    Models/baselistmodel2.cpp \
    Models/searchfiltermodel.cpp \
    Models/settingspanelmodel.cpp \
    Models/usernamemodel.cpp \
    main.cpp \
    tools.cpp \
    Models/baselistmodel.cpp \
    Models/productpanelmodel.cpp \
    Models/searchpanelmodel.cpp \
    Models/showcasepanelmodel.cpp \
    Models/tablepanelmodel.cpp \
    DB/database.cpp \
    Managers/printmanager.cpp \
    Managers/weightmanager.cpp \

HEADERS += \
    DB/jsonparser.h \
    DB/labelformatdbtable.h \
    DB/resourcedbtable.h \
    DB/settingdbtable.h \
    DB/showcasedbtable.h \
    DB/transactiondbtable.h \
    DB/userdbtable.h \
    HTTP/httpclient.h \
    HTTP/httpserver.h \
    HTTP/socketthread.h \
    Managers/appmanager.h \
    Models/baselistmodel2.h \
    Models/searchfiltermodel.h \
    Models/settingspanelmodel.h \
    Models/usernamemodel.h \
    constants.h \
    tools.h \
    Models/baselistmodel.h \
    Models/productpanelmodel.h \
    Models/searchpanelmodel.h \
    Models/showcasepanelmodel.h \
    Models/tablepanelmodel.h \
    DB/database.h \
    DB/productdbtable.h \
    DB/dbtable.h \
    DB/dbtablecolumn.h \
    Managers/printmanager.h \
    Managers/weightmanager.h \

RESOURCES += \
    constants.js \
    main.qml \
    Panels/authorizationPanel.qml \
    Panels/confirmationPanel.qml \
    Panels/inputSettingPanel.qml \
    Panels/messagePanel.qml \
    Panels/productPanel.qml \
    Panels/searchPanel.qml \
    Panels/searchOptionsPanel.qml \
    Panels/settingsPanel.qml \
    Panels/showcasePanel.qml \
    Panels/tablePanel.qml \
    Panels/tableOptionsPanel.qml \
    Panels/weightPanel.qml \
    Resources/Roboto-Regular.ttf \
    Resources/Roboto-Bold.ttf \
    Resources/Roboto-Italic.ttf \
    Resources/LeagueGothic-Regular.otf \
    Icons/arrow_left_black_48.png \
    Icons/arrow_up_black_48.png \
    Icons/build_black_48.png \
    Icons/close_black_48.png \
    Icons/info_outline_black_48.png \
    Icons/lock_black_48.png \
    Icons/menu_black_48.png \
    Icons/print_black_48.png \
    Icons/settings_black_48.png \
    Icons/0_48.png \
    Icons/tare_48.png \
    Icons/weight_48.png \
    Icons/empty_48.png \
    Images/image_dummy.png \
    Images/image_fruit1.png \
    Images/image_fruit2.png \
    Images/image_fruit3.png \
    Images/image_fruit4.png \
    Images/image_fruit5.png \
    Images/image_fruit6.png \
    Images/image_fruit7.png \
    Images/image_fruit8.png \
    Images/image_fruit9.png \
    Text/json_settings.txt \
    Text/json_products.txt \
    Text/json_users.txt \
    Text/json_showcase.txt \
    Text/json_pictures.txt \
    Text/json_messages.txt \

OTHER_FILES += \
    Android/AndroidManifest.xml \


















