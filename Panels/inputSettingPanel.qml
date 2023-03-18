import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import "../constants.js" as Constants
import RegisteredTypes 1.0

Popup
{
    id: inputSettingPanel
    objectName: "inputSettingPanel"
    padding : 0
    //closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
    closePolicy: Popup.CloseOnEscape
    focus: true
    modal: true
    dim: true
    property string titleText: "Title"
    property string inputText: "Input"
    property int settingItemCode: 0

    onOpened: app.onPopupOpened()
    onClosed: app.onPopupClosed()

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

            Text
            {
                id: inputPanelTitle
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                font { pointSize: Constants.normalFontSize; family: 'Roboto'; styleName:'Bold' }
                wrapMode: Text.WordWrap
                text: titleText
            }
        }

        Button
        {
            Layout.column: 2
            Layout.row: 0
            Layout.alignment: Qt.AlignTop | Qt.AlignRigth
            Layout.preferredWidth: Constants.buttonSize
            Layout.preferredHeight: Constants.buttonSize
            icon { width: Constants.iconSize; height: Constants.iconSize; source: "../Icons/close_black_48" }
            leftInset: 0
            topInset: 0
            rightInset: 0
            bottomInset: 0
            Material.background: Material.primary
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

            TextField
            {
                id: inputSettingPanelText
                anchors.horizontalCenter: parent.horizontalCenter
                font { pointSize: Constants.normalFontSize; family: 'Roboto'; styleName:'Regular' }
                text: inputText

                focus: true
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
                            app.onSettingInputClosed(settingItemCode, inputSettingPanelText.text)
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
