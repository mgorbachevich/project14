QT += quick sql widgets core gui httpserver serialport core5compat network multimedia

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DEFINES += ZINT_NO_PNG

INCLUDEPATH += \
    DB \
    Net \
    Managers \
    Models \
    Settings \
    Equipment/Wm100_Slpa100u_2 \

HEADERS += \
    constants.h \
    tools.h \
    appinfo.h \
    DB/productdbtable.h \
    DB/dbtable.h \
    DB/dbtablecolumn.h \
    DB/database.h \
    DB/jsonparser.h \
    DB/labelformatdbtable.h \
    DB/logdbtable.h \
    DB/resourcedbtable.h \
    DB/settingdbtable.h \
    DB/showcasedbtable.h \
    DB/transactiondbtable.h \
    DB/userdbtable.h \
    Equipment/Wm100_Slpa100u_2/IO/RequestSender.h \
    Equipment/Wm100_Slpa100u_2/IO/iobase.h \
    Equipment/Wm100_Slpa100u_2/IO/iocom.h \
    Equipment/Wm100_Slpa100u_2/IO/iohttp.h \
    Equipment/Wm100_Slpa100u_2/IO/request.h \
    Equipment/Wm100_Slpa100u_2/Label/backend/aztec.h \
    Equipment/Wm100_Slpa100u_2/Label/backend/big5.h \
    Equipment/Wm100_Slpa100u_2/Label/backend/bmp.h \
    Equipment/Wm100_Slpa100u_2/Label/backend/channel_precalcs.h \
    Equipment/Wm100_Slpa100u_2/Label/backend/code1.h \
    Equipment/Wm100_Slpa100u_2/Label/backend/code128.h \
    Equipment/Wm100_Slpa100u_2/Label/backend/code49.h \
    Equipment/Wm100_Slpa100u_2/Label/backend/common.h \
    Equipment/Wm100_Slpa100u_2/Label/backend/composite.h \
    Equipment/Wm100_Slpa100u_2/Label/backend/dmatrix.h \
    Equipment/Wm100_Slpa100u_2/Label/backend/dmatrix_trace.h \
    Equipment/Wm100_Slpa100u_2/Label/backend/eci.h \
    Equipment/Wm100_Slpa100u_2/Label/backend/eci_sb.h \
    Equipment/Wm100_Slpa100u_2/Label/backend/emf.h \
    Equipment/Wm100_Slpa100u_2/Label/backend/fonts/normal_ttf.h \
    Equipment/Wm100_Slpa100u_2/Label/backend/fonts/normal_woff2.h \
    Equipment/Wm100_Slpa100u_2/Label/backend/fonts/upcean_ttf.h \
    Equipment/Wm100_Slpa100u_2/Label/backend/fonts/upcean_woff2.h \
    Equipment/Wm100_Slpa100u_2/Label/backend/gb18030.h \
    Equipment/Wm100_Slpa100u_2/Label/backend/gb2312.h \
    Equipment/Wm100_Slpa100u_2/Label/backend/gbk.h \
    Equipment/Wm100_Slpa100u_2/Label/backend/general_field.h \
    Equipment/Wm100_Slpa100u_2/Label/backend/gridmtx.h \
    Equipment/Wm100_Slpa100u_2/Label/backend/gs1.h \
    Equipment/Wm100_Slpa100u_2/Label/backend/gs1_lint.h \
    Equipment/Wm100_Slpa100u_2/Label/backend/hanxin.h \
    Equipment/Wm100_Slpa100u_2/Label/backend/iso3166.h \
    Equipment/Wm100_Slpa100u_2/Label/backend/iso4217.h \
    Equipment/Wm100_Slpa100u_2/Label/backend/ksx1001.h \
    Equipment/Wm100_Slpa100u_2/Label/backend/large.h \
    Equipment/Wm100_Slpa100u_2/Label/backend/maxicode.h \
    Equipment/Wm100_Slpa100u_2/Label/backend/output.h \
    Equipment/Wm100_Slpa100u_2/Label/backend/pcx.h \
    Equipment/Wm100_Slpa100u_2/Label/backend/pdf417.h \
    Equipment/Wm100_Slpa100u_2/Label/backend/pdf417_tabs.h \
    Equipment/Wm100_Slpa100u_2/Label/backend/pdf417_trace.h \
    Equipment/Wm100_Slpa100u_2/Label/backend/qr.h \
    Equipment/Wm100_Slpa100u_2/Label/backend/raster_font.h \
    Equipment/Wm100_Slpa100u_2/Label/backend/reedsol.h \
    Equipment/Wm100_Slpa100u_2/Label/backend/reedsol_logs.h \
    Equipment/Wm100_Slpa100u_2/Label/backend/rss.h \
    Equipment/Wm100_Slpa100u_2/Label/backend/sjis.h \
    Equipment/Wm100_Slpa100u_2/Label/backend/tif.h \
    Equipment/Wm100_Slpa100u_2/Label/backend/tif_lzw.h \
    Equipment/Wm100_Slpa100u_2/Label/backend/zfiletypes.h \
    Equipment/Wm100_Slpa100u_2/Label/backend/zint.h \
    Equipment/Wm100_Slpa100u_2/Label/backend/zintconfig.h \
    Equipment/Wm100_Slpa100u_2/Label/labelcreator.h \
    Equipment/Wm100_Slpa100u_2/Label/labelobject.h \
    Equipment/Wm100_Slpa100u_2/Label/qzint.h \
    Equipment/Wm100_Slpa100u_2/Slpa100u.h \
    Equipment/Wm100_Slpa100u_2/Slpa100uProtocol.h \
    Equipment/Wm100_Slpa100u_2/Slpa100uProtocolCom.h \
    Equipment/Wm100_Slpa100u_2/Slpa100uProtocolDemo.h \
    Equipment/Wm100_Slpa100u_2/Slpa100uProtocolHttp.h \
    Equipment/Wm100_Slpa100u_2/Slpa100uProtocolUsb.h \
    Equipment/Wm100_Slpa100u_2/Wm100Protocol.h \
    Equipment/Wm100_Slpa100u_2/Wm100ProtocolCom.h \
    Equipment/Wm100_Slpa100u_2/Wm100ProtocolDemo.h \
    Equipment/Wm100_Slpa100u_2/Wm100ProtocolHttp.h \
    Equipment/Wm100_Slpa100u_2/wm100.h \
    Managers/keyemitter.h \
    Managers/printmanager.h \
    Managers/weightmanager.h \
    Managers/appmanager.h \
    Managers/printstatus.h \
    Managers/screenmanager.h \
    Models/settingitemlistmodel.h \
    Models/baselistmodel2.h \
    Models/baselistmodel.h \
    Models/productpanelmodel.h \
    Models/searchpanelmodel.h \
    Models/showcasepanelmodel2.h \
    Models/tablepanelmodel.h \
    Models/searchfiltermodel.h \
    Models/settingspanelmodel.h \
    Models/usernamemodel.h \
    Models/viewlogpanelmodel.h \
    Net/netparams.h \
    Net/netserver.h \
    Net/requestparser.h \
    Settings/settings.h \

SOURCES += \
    DB/database.cpp \
    DB/dbtable.cpp \
    DB/jsonparser.cpp \
    DB/logdbtable.cpp \
    DB/productdbtable.cpp \
    DB/settingdbtable.cpp \
    DB/transactiondbtable.cpp \
    DB/userdbtable.cpp \
    Equipment/Wm100_Slpa100u_2/IO/RequestSender.cpp \
    Equipment/Wm100_Slpa100u_2/IO/iobase.cpp \
    Equipment/Wm100_Slpa100u_2/IO/iocom.cpp \
    Equipment/Wm100_Slpa100u_2/IO/iohttp.cpp \
    Equipment/Wm100_Slpa100u_2/IO/request.cpp \
    Equipment/Wm100_Slpa100u_2/Label/backend/2of5.c \
    Equipment/Wm100_Slpa100u_2/Label/backend/auspost.c \
    Equipment/Wm100_Slpa100u_2/Label/backend/aztec.c \
    Equipment/Wm100_Slpa100u_2/Label/backend/bc412.c \
    Equipment/Wm100_Slpa100u_2/Label/backend/bmp.c \
    Equipment/Wm100_Slpa100u_2/Label/backend/codablock.c \
    Equipment/Wm100_Slpa100u_2/Label/backend/code.c \
    Equipment/Wm100_Slpa100u_2/Label/backend/code1.c \
    Equipment/Wm100_Slpa100u_2/Label/backend/code128.c \
    Equipment/Wm100_Slpa100u_2/Label/backend/code16k.c \
    Equipment/Wm100_Slpa100u_2/Label/backend/code49.c \
    Equipment/Wm100_Slpa100u_2/Label/backend/common.c \
    Equipment/Wm100_Slpa100u_2/Label/backend/composite.c \
    Equipment/Wm100_Slpa100u_2/Label/backend/dllversion.c \
    Equipment/Wm100_Slpa100u_2/Label/backend/dmatrix.c \
    Equipment/Wm100_Slpa100u_2/Label/backend/dotcode.c \
    Equipment/Wm100_Slpa100u_2/Label/backend/eci.c \
    Equipment/Wm100_Slpa100u_2/Label/backend/emf.c \
    Equipment/Wm100_Slpa100u_2/Label/backend/general_field.c \
    Equipment/Wm100_Slpa100u_2/Label/backend/gif.c \
    Equipment/Wm100_Slpa100u_2/Label/backend/gridmtx.c \
    Equipment/Wm100_Slpa100u_2/Label/backend/gs1.c \
    Equipment/Wm100_Slpa100u_2/Label/backend/hanxin.c \
    Equipment/Wm100_Slpa100u_2/Label/backend/imail.c \
    Equipment/Wm100_Slpa100u_2/Label/backend/large.c \
    Equipment/Wm100_Slpa100u_2/Label/backend/library.c \
    Equipment/Wm100_Slpa100u_2/Label/backend/mailmark.c \
    Equipment/Wm100_Slpa100u_2/Label/backend/maxicode.c \
    Equipment/Wm100_Slpa100u_2/Label/backend/medical.c \
    Equipment/Wm100_Slpa100u_2/Label/backend/output.c \
    Equipment/Wm100_Slpa100u_2/Label/backend/pcx.c \
    Equipment/Wm100_Slpa100u_2/Label/backend/pdf417.c \
    Equipment/Wm100_Slpa100u_2/Label/backend/plessey.c \
    Equipment/Wm100_Slpa100u_2/Label/backend/png.c \
    Equipment/Wm100_Slpa100u_2/Label/backend/postal.c \
    Equipment/Wm100_Slpa100u_2/Label/backend/ps.c \
    Equipment/Wm100_Slpa100u_2/Label/backend/qr.c \
    Equipment/Wm100_Slpa100u_2/Label/backend/raster.c \
    Equipment/Wm100_Slpa100u_2/Label/backend/reedsol.c \
    Equipment/Wm100_Slpa100u_2/Label/backend/rss.c \
    Equipment/Wm100_Slpa100u_2/Label/backend/svg.c \
    Equipment/Wm100_Slpa100u_2/Label/backend/telepen.c \
    Equipment/Wm100_Slpa100u_2/Label/backend/tif.c \
    Equipment/Wm100_Slpa100u_2/Label/backend/ultra.c \
    Equipment/Wm100_Slpa100u_2/Label/backend/upcean.c \
    Equipment/Wm100_Slpa100u_2/Label/backend/vector.c \
    Equipment/Wm100_Slpa100u_2/Label/labelcreator.cpp \
    Equipment/Wm100_Slpa100u_2/Label/labelobject.cpp \
    Equipment/Wm100_Slpa100u_2/Label/qzint.cpp \
    Equipment/Wm100_Slpa100u_2/Slpa100u.cpp \
    Equipment/Wm100_Slpa100u_2/Slpa100uProtocol.cpp \
    Equipment/Wm100_Slpa100u_2/Slpa100uProtocolCom.cpp \
    Equipment/Wm100_Slpa100u_2/Slpa100uProtocolDemo.cpp \
    Equipment/Wm100_Slpa100u_2/Slpa100uProtocolHttp.cpp \
    Equipment/Wm100_Slpa100u_2/Slpa100uProtocolUsb.cpp \
    Equipment/Wm100_Slpa100u_2/Wm100Protocol.cpp \
    Equipment/Wm100_Slpa100u_2/Wm100ProtocolCom.cpp \
    Equipment/Wm100_Slpa100u_2/Wm100ProtocolDemo.cpp \
    Equipment/Wm100_Slpa100u_2/Wm100ProtocolHttp.cpp \
    Equipment/Wm100_Slpa100u_2/wm100.cpp \
    Managers/appmanager.cpp \
    Managers/keyemitter.cpp \
    Managers/screenmanager.cpp \
    Models/baselistmodel2.cpp \
    Models/searchfiltermodel.cpp \
    Models/settingspanelmodel.cpp \
    Models/usernamemodel.cpp \
    Models/viewlogpanelmodel.cpp \
    Net/netserver.cpp \
    Net/requestparser.cpp \
    main.cpp \
    tools.cpp \
    Models/baselistmodel.cpp \
    Models/productpanelmodel.cpp \
    Models/searchpanelmodel.cpp \
    Models/showcasepanelmodel2.cpp \
    Models/tablepanelmodel.cpp \
    Managers/printmanager.cpp \
    Managers/weightmanager.cpp \
    Settings/settings.cpp \

RESOURCES += \
    constants.js \
    main.qml \
    Fonts/LeagueGothic-CondensedRegular.otf \
    Fonts/LeagueGothic-Regular.otf \
    Fonts/Roboto-Bold.ttf \
    Fonts/Roboto-Condensed.ttf \
    Fonts/Roboto-Italic.ttf \
    Fonts/Roboto-Regular.ttf \
    Fonts/arial.ttf \
    Fonts/arialbd.ttf \
    Fonts/arialbi.ttf \
    Fonts/ariali.ttf \
    Fonts/arialn.ttf \
    Fonts/arialnb.ttf \
    Fonts/arialnbi.ttf \
    Fonts/arialni.ttf \
    Fonts/ariblk.ttf \
    Fonts/impact.ttf \
    Fonts/sserifer.fon \
    Fonts/times.ttf \
    Icons/arrow_left.png \
    Icons/arrow_up.png \
    Icons/auto_flag_green.png \
    Icons/auto_flag_orange.png \
    Icons/backspace.png \
    Icons/bluetooth.png \
    Icons/close.png \
    Icons/empty.png \
    Icons/empty_flag.png \
    Icons/error_flag.png \
    Icons/info_outline.png \
    Icons/keyboard.png \
    Icons/lock.png \
    Icons/log.png \
    Icons/menu.png \
    Icons/ok_outline.png \
    Icons/print.png \
    Icons/red.png \
    Icons/sdcard.png \
    Icons/settings.png \
    Icons/tare_flag.png \
    Icons/usb.png \
    Icons/wifi.png \
    Icons/zero_flag.png \
    Icons/a_down.png \
    Icons/a_up.png \
    Icons/folder.png \
    Icons/n_down.png \
    Icons/n_up.png \
    Icons/star.png \
    Images/image_dummy.png \
    Images/logo.png \
    Labels/60x30.dat/0.bmp \
    Labels/60x30.dat/60x30.par \
    Labels/60x30.dat/description \
    Labels/60x30.dat/print.dat \
    Labels/60x30.dat/strings \
    Labels/60x30.lpr \
    Labels/60x40.dat/0.bmp \
    Labels/60x40.dat/60x40.par \
    Labels/60x40.dat/description \
    Labels/60x40.dat/print.dat \
    Labels/60x40.dat/strings \
    Labels/60x40.lpr \
    Labels/60x50.dat/0.bmp \
    Labels/60x50.dat/60x50.par \
    Labels/60x50.dat/description \
    Labels/60x50.dat/print.dat \
    Labels/60x50.dat/strings \
    Labels/60x50.lpr \
    Labels/60x60.dat/0.bmp \
    Labels/60x60.dat/60x60.par \
    Labels/60x60.dat/description \
    Labels/60x60.dat/print.dat \
    Labels/60x60.dat/strings \
    Labels/60x60.lpr \
    Panels/KeyboardButton.qml \
    Panels/KeyboardCharButton.qml \
    Panels/adminMenuPanel.qml \
    Panels/authorizationPanel.qml \
    Panels/CardText.qml \
    Panels/CardTitleText.qml \
    Panels/comboSettingPanel.qml \
    Panels/sliderSettingPanel.qml \
    Panels/confirmationPanel.qml \
    Panels/EmptyButton.qml \
    Panels/inputPiecesPanel.qml \
    Panels/inputSettingPanel.qml \
    Panels/KeyboardIconButton.qml \
    Panels/messagePanel.qml \
    Panels/productPanel.qml \
    Panels/RoundIconButton.qml \
    Panels/RoundTextButton.qml \
    Panels/searchPanel.qml \
    Panels/settingsPanel.qml \
    Panels/showcasePanel.qml \
    Panels/tablePanel.qml \
    Panels/viewLogPanel.qml \
    Panels/VirtualKeyboardNumeric.qml \
    Panels/VirtualKeyboardCyrillic.qml \
    Panels/VirtualKeyboardLatin.qml \
    Panels/weightPanel.qml \
    Sound/KeypressStandard.wav \
    Sound/KeypressInvalid.wav \
    Sound/KeypressStandard.mp3 \
    Sound/KeypressInvalid.mp3 \
    Text/json_default_settings.txt \
    Text/json_default_equipment_config.txt \
    Text/json_settings.txt \
    Text/json_products.txt \
    Text/json_users.txt \
    Text/json_showcase.txt \
    Text/json_pictures.txt \
    Text/json_messages.txt \

android {
    QT += core-private

    # SSL for android https://github.com/KDAB/android_openssl
    include(Android/android_openssl-master/openssl.pri)

    DISTFILES += \
        Android/AndroidManifest.xml \
        Android/src/ru/shtrih_m/project14/AndroidNative.java

    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/Android
}


















