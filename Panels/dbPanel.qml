import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import RegisteredTypes


Rectangle
{
    id: searchPanel
    color: Material.color(Material.Grey, Material.Shade100)
    property int filterWidth: width * 0.25
    property int virtualKeyboardSet: 2

    Connections // Slot for signal AppManager::showMainPage:
    {
        target: app
        function onShowMainPage(index)
        {
            if (index === 2)
            {
                app.debugLog("@@@@@ searchPanel onShowMainPage");
                searchPanelTextField.forceActiveFocus()
            }
        }
    }

    Connections // Slot for signal AppManager::showVirtualKeyboard
    {
        target: app
        function onShowVirtualKeyboard(value)
        {
            app.debugLog("@@@@@ searchPanel onShowVirtualKeyboard ".arg(value))
            virtualKeyboardSet = value
        }
    }

    Connections // Slot for signal KeyEmitter::enterChar
    {
        target: keyEmitter
        function onEnterChar(value)
        {
            app.debugLog("@@@@@ searchPanel onEnterChar %1".arg(value))
            searchPanelTextField.text += value
        }
    }

    GridLayout
    {
        id: searchPanelLayout
        anchors.fill: parent
        columnSpacing: 0
        rowSpacing: 0

        Rectangle
        {
            Layout.column: 0
            Layout.row: 0
            Layout.columnSpan: 2
            Layout.fillWidth: parent
            Layout.preferredHeight: screenManager.buttonSize()
            color: Material.color(Material.Grey, Material.Shade100)

            CardTitleText { text: qsTr("Поиск товаров") }
        }

        Rectangle
        {
            id: searchPanelResultListRectangle
            Layout.column: 0
            Layout.row: 1
            Layout.rowSpan: 2
            Layout.fillWidth: parent
            Layout.fillHeight: parent
            color: Material.color(Material.Grey, Material.Shade50)

            ListView
            {
                id: searchPanelResultList
                anchors.fill: parent
                orientation: Qt.Vertical
                clip: true
                onFlickStarted: app.onUserAction()
                /*
                ScrollBar.vertical: ScrollBar
                {
                    width: screenManager.scrollBarWidth()
                    background: Rectangle { color: "transparent" }
                    policy: ScrollBar.AlwaysOn
                }
                */
                model: searchPanelModel
                delegate: Label
                {
                    padding: screenManager.spacer()
                    width: searchPanelResultListRectangle.width
                    font { pointSize: screenManager.normalFontSize() }
                    color: Material.color(Material.Grey, Material.Shade900)
                    text: model.value // Roles::ValueRole

                    MouseArea
                    {
                        anchors.fill: parent
                        onClicked: app.onSearchResultClicked(index)
                    }
                }
            }
        }

        TextField
        {
            id: searchPanelTextField
            Layout.column: 1
            Layout.row: 1
            Layout.preferredWidth: filterWidth
            Layout.preferredHeight:  screenManager.largeFontSize() * 3 / 2
            Layout.margins: screenManager.spacer()
            Material.accent: Material.Orange
            color: Material.color(Material.Grey, Material.Shade900)
            font { pointSize: screenManager.largeFontSize() }
            placeholderText: "?????"
            //inputMethodHints: Qt.ImhDigitsOnly // Keyboard
            onTextEdited: app.onUserAction();
            onTextChanged:
            {
                app.debugLog("@@@@@ searchPanelTextField onTextChanged %1".arg(text))
                app.onSearchFilterEdited(text);
            }
            focus: true
            Keys.onPressed: (event) =>
            {
                app.debugLog("@@@@@ searchPanelTextField Keys.onPressed %1".arg(JSON.stringify(event)))
                event.accepted = true;
                app.clickSound();
                app.onUserAction();
                switch (event.key)
                {
                    case Qt.Key_0: case Qt.Key_1: case Qt.Key_2: case Qt.Key_3: case Qt.Key_4:
                    case Qt.Key_5: case Qt.Key_6: case Qt.Key_7: case Qt.Key_8: case Qt.Key_9:
                        text += event.text
                        break;
                    case Qt.Key_Backspace: case Qt.Key_Delete: case Qt.Key_C:
                        text = text.substring(0, text.length - 1);
                        break;
                    case Qt.Key_Escape:
                        text = ""
                        break;
                    case Qt.Key_Left:
                        app.onMainPageSwiped(1)
                        break;
                    case Qt.Key_F9: // Ключ
                        app.onLockClicked()
                        break;
                    case Qt.Key_T: // >T<
                        app.onTareClicked()
                        break
                    case Qt.Key_Z: // >0<
                        app.onZeroClicked()
                        break
                    case Qt.Key_Up:
                        if (!searchPanelResultList.atYBeginning) searchPanelResultList.flick(0, screenManager.flickVelocity())
                        else app.beepSound()
                        break;
                    case Qt.Key_Down:
                        if (!searchPanelResultList.atYEnd) searchPanelResultList.flick(0, -screenManager.flickVelocity())
                        else app.beepSound()
                        break;
                    case Qt.Key_F10: // Промотка
                        app.onRewind()
                        break
                    default:
                        app.beepSound();
                        break;
                }
            }
        }

        Rectangle
        {
            id: searchPanelFilterListLayout
            Layout.column: 1
            Layout.row: 2
            Layout.preferredWidth: filterWidth
            Layout.fillHeight: parent
            Layout.alignment: Qt.AlignTop
            color: "transparent"

            ListView
            {
                id: searchPanelFilterList
                anchors.fill: parent
                orientation: Qt.Vertical
                clip: true
                currentIndex: 0
                onFlickStarted: app.onUserAction()
                //highlight: Rectangle { color: Material.color(Material.BlueGrey, Material.Shade100) }
                /*
                ScrollBar.vertical: ScrollBar
                {
                    width: screenManager.scrollBarWidth()
                    background: Rectangle { color: "transparent" }
                    policy: ScrollBar.AlwaysOn
                }
                */
                model: searchFilterModel
                delegate: Label
                {
                    width: searchPanelFilterList.width
                    padding: screenManager.spacer()
                    font
                    {
                        pointSize: screenManager.normalFontSize();
                        styleName: searchPanelFilterList.currentIndex === index ? 'Bold' : 'Regular'
                    }
                    text: model.value // Roles::ValueRole
                    color: searchPanelFilterList.currentIndex === index ? "black" : Material.color(Material.BlueGrey, Material.Shade600)
                    MouseArea
                    {
                        anchors.fill: parent
                        onClicked:
                        {
                            searchPanelFilterList.currentIndex = index
                            app.onSearchFilterClicked(index)
                        }
                    }
                }
            }
        }

        Loader
        {
            Layout.column: 0
            Layout.row: 3
            Layout.columnSpan: 2
            Layout.alignment: Qt.AlignBottom | Qt.AlignHCenter
            Layout.fillWidth: parent
            Layout.preferredHeight: screenManager.keyboardHeight()
            focus: false
            source: virtualKeyboardSet === 0 ? "VirtualKeyboardLatin.qml" :
                    virtualKeyboardSet === 1 ? "VirtualKeyboardCyrillic.qml" :
                                               "VirtualKeyboardNumeric.qml"
        }

    }
}
