import QtQuick
import QtQuick.Controls.Material
import QtQuick.Layouts
import "../constants.js" as Constants

Popup
{
    id: inputSettingPanel
    padding : 0
    //closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
    closePolicy: Popup.CloseOnEscape
    focus: true
    modal: true
    dim: true
    property string titleText: "Title"
    property string inputText: "Input"
    property int settingIndex: 0

    GridLayout
    {
        anchors.fill: parent
        anchors.margins: Constants.margin
        columnSpacing: Constants.margin
        rowSpacing: Constants.margin
        columns: 3
        rows: 2

        Button
        {
            Layout.column: 0
            Layout.row: 0
            Layout.alignment: Qt.AlignTop | Qt.AlignLeft
            Layout.preferredWidth: Constants.buttonSize
            Layout.preferredHeight: Constants.buttonSize
            icon { width: Constants.iconSize; height: Constants.iconSize; source: "../Icons/empty_48" }
            leftInset: 0
            topInset: 0
            rightInset: 0
            bottomInset: 0
            Material.background: "transparent"
        }

        Rectangle
        {
            Layout.column: 1
            Layout.row: 0
            Layout.fillWidth: parent
            Layout.preferredHeight: Constants.buttonSize
            color: "transparent"

            TextArea
            {
                id: inputPanelTitle
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                font { pointSize: Constants.normalFontSize; family: 'Roboto'; styleName:'Bold' }
                wrapMode: TextArea.WordWrap
                background: Rectangle  { color: "transparent"  }
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
                app.onSettingInputClosed(settingIndex, inputSettingPanelText.text) // AppManager's slot
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
            color: "transparent"
            //color: Material.color(Material.Grey, Material.Shade50)

            TextArea
            {
                id: inputSettingPanelText
                anchors.horizontalCenter: parent.horizontalCenter
                font { pointSize: Constants.normalFontSize; family: 'Roboto'; styleName:'Regular' }
                wrapMode: TextArea.WordWrap
                background: Rectangle  { color: "transparent"  }
                text: inputText
            }
        }
    }
}

