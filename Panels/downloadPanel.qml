import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import RegisteredTypes


Popup
{
    id: downloadPanel
    padding : 0
    closePolicy: Popup.NoAutoClose
    focus: true
    modal: true
    dim: true
    Material.background: "transparent"
    onOpened: app.onPopupOpened(true)
    onClosed: app.onPopupOpened(false)

    Connections // Slot for signal AppManager::showDownloadProgress:
    {
        target: app
        function onShowDownloadProgress(value)
        {
            //app.debugLog("@@@@@ downloadPanel.onShowDownloadProgress");
            if(value >= 0 && value < 100)
            {
                downloadPanelText.text = "%1%".arg(value)
                downloadPanelProgressBar.value = value
            }
            else downloadPanel.close()
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
                        break
                }
            }

            Rectangle
            {
                Layout.column: 0
                Layout.row: 0
                Layout.fillWidth: parent
                Layout.preferredHeight: screenManager.buttonSize()
                color: "transparent"

                CardTitleText { text: "Подождите, идет загрузка товаров" }
            }

            Rectangle
            {
                Layout.column: 0
                Layout.row: 1
                Layout.fillHeight: parent
                Layout.fillWidth: parent
                color: "transparent"
            }

            Rectangle
            {
                Layout.column: 0
                Layout.row: 2
                Layout.fillWidth: parent
                color: "transparent"

                CardTitleText
                {
                    id: downloadPanelText
                    text: "0%"
                }
            }

            Spacer
            {
                Layout.column: 0
                Layout.row: 3
            }

            ProgressBar
            {
                id: downloadPanelProgressBar
                Layout.column: 0
                Layout.row: 4
                Layout.preferredWidth: screenManager.editWidth()
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                from: 0
                to: 100
                value: 0
            }

            Rectangle
            {
                Layout.column: 0
                Layout.row: 5
                Layout.fillHeight: parent
                Layout.fillWidth: parent
                color: "transparent"
            }

            RoundTextButton
            {
                Layout.column: 0
                Layout.row: 6
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                text: qsTr("ПРОДОЛЖИТЬ В ФОНОВОМ РЕЖИМЕ")
                onClicked: if(app.onBackgroundDownloadClicked()) downloadPanel.close()
            }
        }
    }
}

