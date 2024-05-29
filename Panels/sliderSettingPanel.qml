import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
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
    onOpened: app.onPopupOpened(true)
    onClosed:
    {
        app.onSettingInputClosed(settingItemCode, settingItemSlider.value)
        app.onPopupOpened(false)
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
            focus: true
            Keys.onPressed: (event) =>
            {
                //app.debugLog("@@@@@ sliderSettingPanel Keys.onPressed %1".arg(JSON.stringify(event)))
                event.accepted = true;
                app.clickSound();
                app.onUserAction();
                switch (event.key)
                {
                    case Qt.Key_F10: // Промотка
                        app.onRewind()
                        break
                    default:
                        sliderSettingPanel.close()
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
                    app.onSettingInputClosed(settingItemCode, settingItemSlider.value)
                    sliderSettingPanel.close()
                }
            }

            Rectangle
            {
                Layout.column: 0
                Layout.row: 1
                Layout.columnSpan: 3
                Layout.fillHeight: parent
                Layout.fillWidth: parent
                color: "transparent"
            }

            Rectangle
            {
                Layout.column: 0
                Layout.row: 2
                Layout.columnSpan: 3
                Layout.fillWidth: parent
                color: "transparent"

                CardTitleText
                {
                    id: sliderValueText
                    text: settingItemSlider.value
                }
            }

            Spacer
            {
                Layout.column: 0
                Layout.row: 3
            }

            Slider
            {
                id: settingItemSlider
                Layout.column: 0
                Layout.row: 4
                Layout.columnSpan: 3
                Layout.preferredWidth: screenManager.editWidth()
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                from: sliderFrom
                to: sliderTo
                stepSize: sliderStep
                value: sliderValue
            }

            Rectangle
            {
                Layout.column: 0
                Layout.row: 5
                Layout.columnSpan: 3
                Layout.fillHeight: parent
                Layout.fillWidth: parent
                color: "transparent"
            }
        }
    }
}

