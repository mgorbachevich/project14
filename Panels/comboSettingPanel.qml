import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import RegisteredTypes


Popup
{
    id: comboSettingPanel
    padding : 0
    //closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
    closePolicy: Popup.CloseOnEscape
    focus: true
    modal: true
    dim: true
    Material.background: "transparent"
    property string titleText: "Title"
    property int settingItemCode: 0
    property int comboIndex: 0
    property string comboText: ""
    property string commentText: ""
    onOpened: app.onPopupOpened(true)
    onClosed:
    {
        //app.debugLog("@@@@@ comboSettingPanel onClosed %1".arg(settingItemComboBox.currentIndex))
        app.onSettingInputClosed(settingItemCode, settingItemComboBox.currentIndex)
        app.onPopupOpened(false)
    }

    Rectangle
    {
        anchors.fill: parent
        radius: screenManager.spacer()
        Material.background: Material.color(Material.Grey, Material.Shade100)
        color: Material.color(Material.Grey, Material.Shade100)

        GridLayout
        {
            anchors.fill: parent
            anchors.margins: screenManager.spacer()
            columnSpacing: 0
            rowSpacing: 0
            focus: true
            Keys.onPressed: (event) =>
            {
                //app.debugLog("@@@@@ comboSettingPanel Keys.onPressed %1".arg(JSON.stringify(event)))
                event.accepted = true;
                app.clickSound();
                app.onUserAction();
                switch (event.key)
                {
                    case Qt.Key_F10: // Промотка
                        app.onRewind()
                        break
                    default:
                        comboSettingPanel.close()
                        break
                }
            }

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
                Layout.preferredHeight: screenManager.buttonSize()
                color: "transparent"

                CardTitleText { text: titleText }
            }

            RoundIconButton
            {
                Layout.column: 2
                Layout.row: 0
                Layout.alignment: Qt.AlignTop | Qt.AlignRigth
                icon.source: "../Icons/close"
                onClicked:
                {
                    app.onUserAction();
                    comboSettingPanel.close()
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

                ComboBox
                {
                    id: settingItemComboBox
                    width: screenManager.editWidth()
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.horizontalCenter: parent.horizontalCenter
                    editable: false
                    popup.modal: true
                    currentIndex: comboIndex
                    displayText: comboText

                    model: settingItemModel
                    delegate: Text
                    {
                        padding: screenManager.spacer()
                        font { pointSize: screenManager.normalFontSize() }
                        color: Material.color(Material.BlueGrey, Material.Shade900)
                        text: model.value // Roles::ValueRole

                        MouseArea
                        {
                            anchors.fill: parent
                            onClicked:
                            {
                                //app.debugLog("@@@@@ comboSettingPanel.settingItemComboBox.onClicked %1".arg(index))
                                app.onUserAction();
                                settingItemComboBox.currentIndex = index
                                settingItemComboBox.displayText = text
                                settingItemComboBox.popup.close()
                            }
                        }
                    }
                }
            }

            Rectangle
            {
                Layout.column: 0
                Layout.row: 2
                Layout.columnSpan: 3
                Layout.fillWidth: parent
                Layout.fillHeight: parent
                color: "transparent"
                visible: commentText != ""

                CardText { text: commentText }
            }
        }
    }
}

