import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import RegisteredTypes

Popup
{
    id: inputProductCodePanel
    padding: 0
    //closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
    closePolicy: Popup.CloseOnEscape
    focus: true
    modal: true
    dim: true
    Material.background: "transparent"
    property string inputText: "Input"
    property string title: "Title"

    onOpened:
    {
        app.onPopupOpened(true)
        inputProductCodePanelModel.onStart()
    }
    onClosed: app.onPopupOpened(false)

    enter: Transition { enabled: false }
    exit: Transition { enabled: false }

    Connections // Slot for signal AppManager::closeInputProductPanel
    {
        target: app
        function onCloseInputProductPanel()
        {
            inputProductCodePanel.close()
        }
    }

    Connections // Slot for signal AppManager::enableSetProductByInputCode
    {
        target: app
        function onEnableSetProductByInputCode(value)
        {
            inputProductCodePanelContinueButton.enabled = value
        }
    }

    Rectangle
    {
        anchors.fill: parent
        radius: screenManager.spacer()
        Material.background: Material.color(Material.Grey, Material.Shade100)
        color: Material.background

        GridLayout
        {
            anchors.fill: parent
            anchors.margins: screenManager.spacer()
            columnSpacing: 0
            rowSpacing: 0

            EmptyButton
            {
                Layout.column: 0
                Layout.row: 0
                Layout.alignment: Qt.AlignTop | Qt.AlignLeft
            }

            Rectangle
            {
                Layout.column: 1
                Layout.row: 0
                Layout.fillWidth: parent
                Layout.alignment: Qt.AlignCenter
                color: "transparent"

                CardTitleText { text: title }
            }

            RoundIconButton
            {
                Layout.column: 2
                Layout.row: 0
                Layout.alignment: Qt.AlignTop | Qt.AlignRigth
                icon.source: "../Icons/close"
                onClicked: inputProductCodePanel.close()
            }

            Spacer
            {
                Layout.column: 1
                Layout.row: 1
            }

            Rectangle
            {
                Layout.column: 1
                Layout.row: 2
                Layout.fillWidth: parent
                Layout.preferredHeight: screenManager.largeFontSize() * 2
                color: "transparent"

                TextField
                {
                    id: inputProductCodePanelText
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: screenManager.editWidth() / 2
                    height: screenManager.editHeight()
                    font { pointSize: screenManager.largeFontSize() }
                    Material.accent: Material.Orange
                    color: Material.color(Material.BlueGrey, Material.Shade900)
                    focus: true
                    placeholderText: ""
                    text: inputText

                    onTextEdited: app.onUserAction();
                    onTextChanged:
                    {
                        if (parseInt(text) <= 0) text = "";
                        app.onInputProductCodeEdited(text);
                    }
                    Keys.onPressed: (event) =>
                    {
                        //app.debugLog("@@@@@ inputProductCodePanelText Keys.onPressed %1".arg(JSON.stringify(event)))
                        event.accepted = true;
                        app.onClick();
                        switch (event.key)
                        {
                            case Qt.Key_0: case Qt.Key_1: case Qt.Key_2: case Qt.Key_3: case Qt.Key_4:
                            case Qt.Key_5: case Qt.Key_6: case Qt.Key_7: case Qt.Key_8: case Qt.Key_9:
                                text += event.text
                                break;
                            case Qt.Key_Backspace: case Qt.Key_Delete: case Qt.Key_C:
                                if(text.length > 0) text = text.substring(0, text.length - 1);
                                break;
                            case Qt.Key_Escape:
                                inputProductCodePanel.close();
                                break;
                            case Qt.Key_Enter: case Qt.Key_Return:
                                if (inputProductCodePanelContinueButton.enabled) app.onClicked3(30, text);
                                break;
                            case Qt.Key_F10: // Промотка
                                app.onClicked(26)
                                break
                            default:
                                app.beepSound();
                                break;
                        }
                        inputProductCodePanelText.focus = true
                    }
                }
            }

            Spacer
            {
                Layout.column: 1
                Layout.row: 3
            }

            Rectangle
            {
                id: inputProductCodePanelListRectangle
                Layout.column: 0
                Layout.row: 4
                Layout.columnSpan: 3
                Layout.fillWidth: parent
                Layout.fillHeight: parent
                color: Material.color(Material.Grey, Material.Shade50)

                ListView
                {
                    id: inputProductCodePanelList
                    anchors.fill: parent
                    orientation: Qt.Vertical
                    clip: true
                    /*
                    ScrollBar.vertical: ScrollBar
                    {
                        width: screenManager.scrollBarWidth()
                        background: Rectangle { color: "transparent" }
                        policy: ScrollBar.AlwaysOn
                    }
                    */

                    model: inputProductCodePanelModel
                    onContentYChanged: model.onFlickTo(indexAt(0, contentY))

                    delegate: Row
                    {
                        leftPadding: screenManager.spacer()
                        rightPadding: screenManager.spacer()
                        topPadding: 0
                        bottomPadding: 0
                        spacing: screenManager.spacer() * 2

                        Label
                        {
                            width: (inputProductCodePanelList.width - screenManager.spacer() * 4) * 0.15
                            font { pointSize: screenManager.normalFontSize() }
                            color: Material.color(Material.BlueGrey, Material.Shade600)
                            horizontalAlignment: Text.AlignRight
                            leftPadding: 0
                            rightPadding: 0
                            topPadding: screenManager.spacer()
                            bottomPadding: screenManager.spacer()
                            text: model.code
                            MouseArea
                            {
                                anchors.fill: parent
                                onClicked: app.onClicked3(30, model.code)
                            }
                        }

                        Label
                        {
                            width: (inputProductCodePanelList.width - screenManager.spacer() * 4) * 0.85
                            font { pointSize: screenManager.normalFontSize() }
                            color: Material.color(Material.BlueGrey, Material.Shade900)
                            horizontalAlignment: Text.AlignLeft
                            leftPadding: 0
                            rightPadding: 0
                            topPadding: screenManager.spacer()
                            bottomPadding: screenManager.spacer()
                            text: model.name
                            MouseArea
                            {
                                anchors.fill: parent
                                onClicked: app.onClicked3(30, model.code)
                            }
                        }
                    }
                }
            }

            Spacer
            {
                Layout.column: 1
                Layout.row: 5
            }

            RoundTextButton
            {
                id: inputProductCodePanelContinueButton
                Layout.column: 1
                Layout.row: 6
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                text: qsTr("ПРОДОЛЖИТЬ")
                onClicked: app.onClicked3(30, inputProductCodePanelText.text)
            }
        }
    }
}
