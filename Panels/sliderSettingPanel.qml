import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import "../constants.js" as Constants
import RegisteredTypes

Popup
{
    id: sliderSettingPanel
    padding : 0
    //closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
    closePolicy: Popup.CloseOnEscape
    focus: true
    modal: true
    dim: true
    Material.background: "transparent"
    property string titleText: "Title"
    property int settingItemCode: 0
    property int sliderFrom: 0
    property int sliderTo: 1
    property int sliderStep: 1
    property int sliderValue: 1
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
            rows: 3
            focus: true
            Keys.onPressed: (event) =>
            {
                app.debugLog("@@@@@ sliderSettingPanel Keys.onPressed %1".arg(JSON.stringify(event)))
                event.accepted = true;
                app.clickSound();
                app.onUserAction();
                app.onSettingInputClosed(settingItemCode, settingItemSlider.value)
                sliderSettingPanel.close()
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
                    app.onSettingInputClosed(settingItemCode, settingItemSlider.value)
                    sliderSettingPanel.close()
                }
            }

            Text
            {
                id: sliderValueText
                Layout.column: 1
                Layout.row: 1
                Layout.alignment: Qt.AlignHCenter
                color: Material.color(Material.BlueGrey, Material.Shade600)
                font { pointSize: screenManager.largeFontSize() }
                text: settingItemSlider.value
            }

            Slider
            {
                id: settingItemSlider
                Layout.column: 0
                Layout.row: 2
                Layout.columnSpan: 3
                Layout.preferredWidth: screenManager.editWidth()
                Layout.alignment: Qt.AlignHCenter
                Layout.bottomMargin: screenManager.buttonSize() * 3 / 4
                from: sliderFrom
                to: sliderTo
                stepSize: sliderStep
                value: sliderValue
            }
        }
    }
}

