import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import "../constants.js" as Constants
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
    onOpened: app.onPopupOpened()
    onClosed: app.onPopupClosed()

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
            columns: 3
            rows: 2
            focus: true
            Keys.onPressed: (event) =>
            {
                console.debug("@@@@@ comboSettingPanel Keys.onPressed ", JSON.stringify(event))
                event.accepted = true;
                app.clickSound();
                app.onUserAction();
                app.onSettingInputClosed(settingItemCode, settingItemComboBox.currentIndex)
                comboSettingPanel.close()
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
                    app.onSettingInputClosed(settingItemCode, settingItemComboBox.currentIndex)
                    comboSettingPanel.close()
                }
            }

            ComboBox
            {
                id: settingItemComboBox
                Layout.column: 0
                Layout.row: 1
                Layout.columnSpan: 3
                Layout.preferredWidth: screenManager.editWidth()
                Layout.alignment: Qt.AlignCenter
                Layout.bottomMargin: screenManager.buttonSize() * 3 / 4
                editable: false
                popup.modal: true
                currentIndex: comboIndex
                displayText: comboText

                model: settingItemListModel
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
                            console.debug("@@@@@ comboSettingPanel.settingItemComboBox.onClicked ", index)
                            app.onUserAction();
                            settingItemComboBox.currentIndex = index
                            settingItemComboBox.displayText = text
                            settingItemComboBox.popup.close()
                        }
                    }
                }
            }
        }
    }
}
