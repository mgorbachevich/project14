import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import "../constants.js" as Constants
import RegisteredTypes 1.0

Popup
{
    id: inputSettingPanel
    padding : 0
    //closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
    closePolicy: Popup.CloseOnEscape
    focus: true
    modal: true
    dim: true
    Material.background: "transparent"
    property string titleText: "Title"
    property string inputText: "Input"
    property int settingItemCode: 0
    onOpened: app.onPopupOpened()
    onClosed: app.onPopupClosed()

    Rectangle
    {
        anchors.fill: parent
        radius: Constants.margin
        Material.background: Material.color(Material.Grey, Material.Shade100)
        color: Material.background

        GridLayout
        {
            anchors.fill: parent
            anchors.margins: Constants.margin
            columnSpacing: Constants.margin
            rowSpacing: Constants.margin
            columns: 3
            rows: 2

            Rectangle
            {
                Layout.column: 0
                Layout.row: 0
                Layout.alignment: Qt.AlignTop | Qt.AlignLeft
                Layout.preferredWidth: Constants.buttonSize
                Layout.preferredHeight: Constants.buttonSize
                color: "transparent"
            }

            Rectangle
            {
                Layout.column: 1
                Layout.row: 0
                Layout.fillWidth: parent
                Layout.preferredHeight: Constants.buttonSize
                color: "transparent"

                CardTitleText
                {
                    text: titleText
                }
            }

            RoundIconButton
            {
                Layout.column: 2
                Layout.row: 0
                Layout.alignment: Qt.AlignTop | Qt.AlignRigth
                icon.source: "../Icons/close_black_48"
                onClicked:
                {
                    app.onSettingInputClosed(settingItemCode, inputSettingPanelText.text) // AppManager's slot
                    inputSettingPanel.close()
                }
            }

            Rectangle
            {
                Layout.column: 0
                Layout.row: 1
                Layout.columnSpan: 3
                Layout.fillWidth: parent
                Layout.fillHeight: parent
                Layout.bottomMargin: Constants.buttonSize * 3 / 4
                color: "transparent"

                TextField
                {
                    id: inputSettingPanelText
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: Constants.inputFieldWidth
                    font { pointSize: Constants.normalFontSize }
                    Material.accent: Material.Orange
                    color: Material.color(Material.BlueGrey, Material.Shade900)
                    focus: true
                    text: inputText

                    Keys.onPressed: (event) =>
                    {
                        console.debug("@@@@@ inputSettingPanelText Keys.onPressed ", JSON.stringify(event))
                        event.accepted = true;
                        switch (event.key)
                        {
                            case Qt.Key_0:
                            case Qt.Key_1:
                            case Qt.Key_2:
                            case Qt.Key_3:
                            case Qt.Key_4:
                            case Qt.Key_5:
                            case Qt.Key_6:
                            case Qt.Key_7:
                            case Qt.Key_8:
                            case Qt.Key_9:
                                text += event.text
                                break;
                            case Qt.Key_C:
                                text = text.substring(0, text.length - 1);
                                break;
                            case Qt.Key_Escape:
                                text = ""
                                break;
                            case Qt.Key_Enter:
                                app.onSettingInputClosed(settingItemCode, text)
                                inputSettingPanel.close()
                                break
                             default:
                                app.onBeep();
                                break
                        }
                    }
                }
            }
        }
    }
}
