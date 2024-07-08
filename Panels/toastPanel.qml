import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import RegisteredTypes

Popup
{
    id: toastPanel
    padding : 0
    //closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
    closePolicy: Popup.CloseOnEscape
    focus: false
    modal: false
    dim: false
    Material.background: "transparent"
    property string messageText: "Message"

    enter: Transition { enabled: false }
    exit: Transition { enabled: false }

    Connections // Slot for signal AppManager::hideToastBox
    {
        target: app
        function onHideToastBox() { toastPanel.close() }
    }

    Rectangle
    {
        width: screenManager.screenWidth() / 2
        height: screenManager.normalFontSize() * 3
        x: (screenManager.screenWidth() - width) / 2
        y: screenManager.screenHeight() - height - screenManager.spacer()
        radius: height / 2
        color: Material.color(Material.Orange, Material.Shade100)
        //border.color: Material.color(Material.Grey, Material.Shade400)
        //border.width: 1

        CardText { text: messageText }
    }
}

