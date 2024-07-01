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
    Equipment/Wm100_Slpa100u \

HEADERS += \
    DB/dbindexdescriptor.h \
    DB/showcase.h \
    Models/edituserspanelmodel3.h \
    Models/inputproductcodepanelmodel3.h \
    Models/searchpanelmodel3.h \
    Models/settingspanelmodel3.h \
    Models/showcasepanelmodel3.h \
    Net/netactionresult.h \
    constants.h \
    tools.h \
    appinfo.h \
    DB/jsonarrayfile.h \
    DB/jsonfile.h \
    DB/scaleconfig.h \
    DB/users.h \
    DB/productdbtable.h \
    DB/dbtable.h \
    DB/dbtablecolumn.h \
    DB/database.h \
    DB/logdbtable.h \
    DB/resourcedbtable.h \
    DB/transactiondbtable.h \
    DB/settings.h \
    Equipment/Wm100_Slpa100u/IO/RequestSender.h \
    Equipment/Wm100_Slpa100u/IO/iobase.h \
    Equipment/Wm100_Slpa100u/IO/iocom.h \
    Equipment/Wm100_Slpa100u/IO/iohttp.h \
    Equipment/Wm100_Slpa100u/IO/request.h \
    Equipment/Wm100_Slpa100u/Label/backend/aztec.h \
    Equipment/Wm100_Slpa100u/Label/backend/big5.h \
    Equipment/Wm100_Slpa100u/Label/backend/bmp.h \
    Equipment/Wm100_Slpa100u/Label/backend/channel_precalcs.h \
    Equipment/Wm100_Slpa100u/Label/backend/code1.h \
    Equipment/Wm100_Slpa100u/Label/backend/code128.h \
    Equipment/Wm100_Slpa100u/Label/backend/code49.h \
    Equipment/Wm100_Slpa100u/Label/backend/common.h \
    Equipment/Wm100_Slpa100u/Label/backend/composite.h \
    Equipment/Wm100_Slpa100u/Label/backend/dmatrix.h \
    Equipment/Wm100_Slpa100u/Label/backend/dmatrix_trace.h \
    Equipment/Wm100_Slpa100u/Label/backend/eci.h \
    Equipment/Wm100_Slpa100u/Label/backend/eci_sb.h \
    Equipment/Wm100_Slpa100u/Label/backend/emf.h \
    Equipment/Wm100_Slpa100u/Label/backend/fonts/normal_ttf.h \
    Equipment/Wm100_Slpa100u/Label/backend/fonts/normal_woff2.h \
    Equipment/Wm100_Slpa100u/Label/backend/fonts/upcean_ttf.h \
    Equipment/Wm100_Slpa100u/Label/backend/fonts/upcean_woff2.h \
    Equipment/Wm100_Slpa100u/Label/backend/gb18030.h \
    Equipment/Wm100_Slpa100u/Label/backend/gb2312.h \
    Equipment/Wm100_Slpa100u/Label/backend/gbk.h \
    Equipment/Wm100_Slpa100u/Label/backend/general_field.h \
    Equipment/Wm100_Slpa100u/Label/backend/gridmtx.h \
    Equipment/Wm100_Slpa100u/Label/backend/gs1.h \
    Equipment/Wm100_Slpa100u/Label/backend/gs1_lint.h \
    Equipment/Wm100_Slpa100u/Label/backend/hanxin.h \
    Equipment/Wm100_Slpa100u/Label/backend/iso3166.h \
    Equipment/Wm100_Slpa100u/Label/backend/iso4217.h \
    Equipment/Wm100_Slpa100u/Label/backend/ksx1001.h \
    Equipment/Wm100_Slpa100u/Label/backend/large.h \
    Equipment/Wm100_Slpa100u/Label/backend/maxicode.h \
    Equipment/Wm100_Slpa100u/Label/backend/output.h \
    Equipment/Wm100_Slpa100u/Label/backend/pcx.h \
    Equipment/Wm100_Slpa100u/Label/backend/pdf417.h \
    Equipment/Wm100_Slpa100u/Label/backend/pdf417_tabs.h \
    Equipment/Wm100_Slpa100u/Label/backend/pdf417_trace.h \
    Equipment/Wm100_Slpa100u/Label/backend/qr.h \
    Equipment/Wm100_Slpa100u/Label/backend/raster_font.h \
    Equipment/Wm100_Slpa100u/Label/backend/reedsol.h \
    Equipment/Wm100_Slpa100u/Label/backend/reedsol_logs.h \
    Equipment/Wm100_Slpa100u/Label/backend/rss.h \
    Equipment/Wm100_Slpa100u/Label/backend/sjis.h \
    Equipment/Wm100_Slpa100u/Label/backend/tif.h \
    Equipment/Wm100_Slpa100u/Label/backend/tif_lzw.h \
    Equipment/Wm100_Slpa100u/Label/backend/zfiletypes.h \
    Equipment/Wm100_Slpa100u/Label/backend/zint.h \
    Equipment/Wm100_Slpa100u/Label/backend/zintconfig.h \
    Equipment/Wm100_Slpa100u/Label/labelcreator.h \
    Equipment/Wm100_Slpa100u/Label/labelobject.h \
    Equipment/Wm100_Slpa100u/Label/qzint.h \
    Equipment/Wm100_Slpa100u/Slpa100u.h \
    Equipment/Wm100_Slpa100u/Slpa100uProtocol.h \
    Equipment/Wm100_Slpa100u/Slpa100uProtocolCom.h \
    Equipment/Wm100_Slpa100u/Slpa100uProtocolDemo.h \
    Equipment/Wm100_Slpa100u/Slpa100uProtocolHttp.h \
    Equipment/Wm100_Slpa100u/Slpa100uProtocolUsb.h \
    Equipment/Wm100_Slpa100u/Wm100Protocol.h \
    Equipment/Wm100_Slpa100u/Wm100ProtocolCom.h \
    Equipment/Wm100_Slpa100u/Wm100ProtocolDemo.h \
    Equipment/Wm100_Slpa100u/Wm100ProtocolHttp.h \
    Equipment/Wm100_Slpa100u/wm100.h \
    Managers/equipmentmanager.h \
    Managers/externalmessager.h \
    Managers/moneycalculator.h \
    Managers/keyemitter.h \
    Managers/appmanager.h \
    Managers/printstatus.h \
    Managers/screenmanager.h \
    Models/baselistmodel3.h \
    Models/baselistmodel.h \
    Models/productpanelmodel.h \
    Models/searchfiltermodel.h \
    Models/usernamemodel.h \
    Models/viewlogpanelmodel.h \
    Net/netparams.h \
    Net/netserver.h \

SOURCES += \
    DB/showcase.cpp \
    Models/edituserspanelmodel3.cpp \
    Models/inputproductcodepanelmodel3.cpp \
    Models/searchpanelmodel3.cpp \
    Models/settingspanelmodel3.cpp \
    Models/showcasepanelmodel3.cpp \
    Net/netactionresult.cpp \
    main.cpp \
    tools.cpp \
    DB/database.cpp \
    DB/dbtable.cpp \
    DB/jsonarrayfile.cpp \
    DB/jsonfile.cpp \
    DB/logdbtable.cpp \
    DB/productdbtable.cpp \
    DB/scaleconfig.cpp \
    DB/transactiondbtable.cpp \
    DB/settings.cpp \
    DB/users.cpp \
    Equipment/Wm100_Slpa100u/IO/RequestSender.cpp \
    Equipment/Wm100_Slpa100u/IO/iobase.cpp \
    Equipment/Wm100_Slpa100u/IO/iocom.cpp \
    Equipment/Wm100_Slpa100u/IO/iohttp.cpp \
    Equipment/Wm100_Slpa100u/IO/request.cpp \
    Equipment/Wm100_Slpa100u/Label/backend/2of5.c \
    Equipment/Wm100_Slpa100u/Label/backend/auspost.c \
    Equipment/Wm100_Slpa100u/Label/backend/aztec.c \
    Equipment/Wm100_Slpa100u/Label/backend/bc412.c \
    Equipment/Wm100_Slpa100u/Label/backend/bmp.c \
    Equipment/Wm100_Slpa100u/Label/backend/codablock.c \
    Equipment/Wm100_Slpa100u/Label/backend/code.c \
    Equipment/Wm100_Slpa100u/Label/backend/code1.c \
    Equipment/Wm100_Slpa100u/Label/backend/code128.c \
    Equipment/Wm100_Slpa100u/Label/backend/code16k.c \
    Equipment/Wm100_Slpa100u/Label/backend/code49.c \
    Equipment/Wm100_Slpa100u/Label/backend/common.c \
    Equipment/Wm100_Slpa100u/Label/backend/composite.c \
    Equipment/Wm100_Slpa100u/Label/backend/dllversion.c \
    Equipment/Wm100_Slpa100u/Label/backend/dmatrix.c \
    Equipment/Wm100_Slpa100u/Label/backend/dotcode.c \
    Equipment/Wm100_Slpa100u/Label/backend/eci.c \
    Equipment/Wm100_Slpa100u/Label/backend/emf.c \
    Equipment/Wm100_Slpa100u/Label/backend/general_field.c \
    Equipment/Wm100_Slpa100u/Label/backend/gif.c \
    Equipment/Wm100_Slpa100u/Label/backend/gridmtx.c \
    Equipment/Wm100_Slpa100u/Label/backend/gs1.c \
    Equipment/Wm100_Slpa100u/Label/backend/hanxin.c \
    Equipment/Wm100_Slpa100u/Label/backend/imail.c \
    Equipment/Wm100_Slpa100u/Label/backend/large.c \
    Equipment/Wm100_Slpa100u/Label/backend/library.c \
    Equipment/Wm100_Slpa100u/Label/backend/mailmark.c \
    Equipment/Wm100_Slpa100u/Label/backend/maxicode.c \
    Equipment/Wm100_Slpa100u/Label/backend/medical.c \
    Equipment/Wm100_Slpa100u/Label/backend/output.c \
    Equipment/Wm100_Slpa100u/Label/backend/pcx.c \
    Equipment/Wm100_Slpa100u/Label/backend/pdf417.c \
    Equipment/Wm100_Slpa100u/Label/backend/plessey.c \
    Equipment/Wm100_Slpa100u/Label/backend/png.c \
    Equipment/Wm100_Slpa100u/Label/backend/postal.c \
    Equipment/Wm100_Slpa100u/Label/backend/ps.c \
    Equipment/Wm100_Slpa100u/Label/backend/qr.c \
    Equipment/Wm100_Slpa100u/Label/backend/raster.c \
    Equipment/Wm100_Slpa100u/Label/backend/reedsol.c \
    Equipment/Wm100_Slpa100u/Label/backend/rss.c \
    Equipment/Wm100_Slpa100u/Label/backend/svg.c \
    Equipment/Wm100_Slpa100u/Label/backend/telepen.c \
    Equipment/Wm100_Slpa100u/Label/backend/tif.c \
    Equipment/Wm100_Slpa100u/Label/backend/ultra.c \
    Equipment/Wm100_Slpa100u/Label/backend/upcean.c \
    Equipment/Wm100_Slpa100u/Label/backend/vector.c \
    Equipment/Wm100_Slpa100u/Label/labelcreator.cpp \
    Equipment/Wm100_Slpa100u/Label/labelobject.cpp \
    Equipment/Wm100_Slpa100u/Label/qzint.cpp \
    Equipment/Wm100_Slpa100u/Slpa100u.cpp \
    Equipment/Wm100_Slpa100u/Slpa100uProtocol.cpp \
    Equipment/Wm100_Slpa100u/Slpa100uProtocolCom.cpp \
    Equipment/Wm100_Slpa100u/Slpa100uProtocolDemo.cpp \
    Equipment/Wm100_Slpa100u/Slpa100uProtocolHttp.cpp \
    Equipment/Wm100_Slpa100u/Slpa100uProtocolUsb.cpp \
    Equipment/Wm100_Slpa100u/Wm100Protocol.cpp \
    Equipment/Wm100_Slpa100u/Wm100ProtocolCom.cpp \
    Equipment/Wm100_Slpa100u/Wm100ProtocolDemo.cpp \
    Equipment/Wm100_Slpa100u/Wm100ProtocolHttp.cpp \
    Equipment/Wm100_Slpa100u/wm100.cpp \
    Managers/appmanager.cpp \
    Managers/equipmentmanager.cpp \
    Managers/externalmessager.cpp \
    Managers/keyemitter.cpp \
    Managers/moneycalculator.cpp \
    Managers/screenmanager.cpp \
    Models/baselistmodel3.cpp \
    Models/usernamemodel.cpp \
    Models/viewlogpanelmodel.cpp \
    Models/baselistmodel.cpp \
    Models/productpanelmodel.cpp \
    Net/netserver.cpp \

RESOURCES += \
    main.qml \
    Default/1.png \
    Default/2.png \
    Default/3.jpg \
    Default/4.png \
    Default/5.png \
    Default/6.png \
    Default/8.png \
    Default/9.png \
    Default/10.png \
    Default/11.png \
    Default/12.png \
    Default/15.png \
    Default/ShtrihScale.db \
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
    Icons/a.png \
    Icons/arrow_down.png \
    Icons/arrow_left.png \
    Icons/arrow_up.png \
    Icons/auto_white.png \
    Icons/backspace.png \
    Icons/bluetooth.png \
    Icons/bluetooth_orange.png \
    Icons/bluetooth_light.png \
    Icons/close.png \
    Icons/code.png \
    Icons/delete.png \
    Icons/empty.png \
    Icons/error.png \
    Icons/error_red.png \
    Icons/fix_white.png \
    Icons/folder.png \
    Icons/home.png \
    Icons/info_outline.png \
    Icons/keyboard.png \
    Icons/lock.png \
    Icons/log.png \
    Icons/menu.png \
    Icons/number.png \
    Icons/net.png \
    Icons/net_orange.png \
    Icons/net_light.png \
    Icons/net_white.png \
    Icons/ok_outline.png \
    Icons/plus.png \
    Icons/print.png \
    Icons/sdcard.png \
    Icons/sdcard_orange.png \
    Icons/sdcard_light.png \
    Icons/settings.png \
    Icons/star.png \
    Icons/tare_white.png \
    Icons/usb.png \
    Icons/usb_orange.png \
    Icons/usb_light.png \
    Icons/user.png \
    Icons/wifi.png \
    Icons/wifi_orange.png \
    Icons/wifi_light.png \
    Icons/zero_white.png \
    Images/dummy.png \
    Images/logo.png \
    Images/help.png \
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
    Panels/authorizationPanel.qml \
    Panels/calendarPanel.qml \
    Panels/CardText.qml \
    Panels/CardTitleText.qml \
    Panels/comboSettingPanel.qml \
    Panels/confirmationPanel.qml \
    Panels/downloadPanel.qml \
    Panels/editUsersPanel.qml \
    Panels/EmptyButton.qml \
    Panels/inputPasswordPanel.qml \
    Panels/inputPiecesPanel.qml \
    Panels/inputProductCodePanel.qml \
    Panels/inputSettingPanel.qml \
    Panels/inputUserPanel.qml \
    Panels/KeyboardButton.qml \
    Panels/KeyboardCharButton.qml \
    Panels/KeyboardIconButton.qml \
    Panels/messagePanel.qml \
    Panels/productPanel.qml \
    Panels/RoundIconButton.qml \
    Panels/RoundTextButton.qml \
    Panels/searchPanel.qml \
    Panels/settingsPanel.qml \
    Panels/showcasePanel3.qml \
    Panels/sliderSettingPanel.qml \
    Panels/Spacer.qml \
    Panels/SubtitleText.qml \
    Panels/Sticker.qml \
    Panels/toastPanel.qml \
    Panels/viewLogPanel.qml \
    Panels/VirtualKeyboardNumeric.qml \
    Panels/VirtualKeyboardCyrillic.qml \
    Panels/VirtualKeyboardLatin.qml \
    Panels/waitPanel.qml \
    Panels/weightPanel.qml \
    Sound/KeypressStandard.mp3 \
    Sound/KeypressInvalid.mp3 \
    Text/json_demo_equipment_config.txt \
    Text/json_default_settings.txt \

android {
    QT += core-private

    # SSL for android https://github.com/KDAB/android_openssl
    include(Android/android_openssl-master/openssl.pri)

    DISTFILES += \
        Android/AndroidManifest.xml \
        Android/src/ru/shtrih_m/shtrihprint6/AndroidNative.java

    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/Android
}




















