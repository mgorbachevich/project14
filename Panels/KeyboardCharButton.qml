import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import RegisteredTypes


Label
{
    width: screenManager.buttonSize()
    height: screenManager.buttonSize()
    Layout.preferredWidth: screenManager.buttonSize()
    Layout.preferredHeight: screenManager.buttonSize()
    leftInset: screenManager.spacer() / 4
    topInset: screenManager.spacer() / 4
    rightInset: screenManager.spacer() / 4
    bottomInset: screenManager.spacer() / 4
    color: Material.color(Material.BlueGrey, Material.Shade900)
    font { pointSize: screenManager.largeFontSize() }
    horizontalAlignment: Text.AlignHCenter
    verticalAlignment: Text.AlignVCenter
    background: Rectangle
    {
        color: Material.color(Material.BlueGrey, Material.Shade100)
        radius: screenManager.spacer()
    }
    MouseArea
    {
        anchors.fill: parent
        onClicked: keyEmitter.emitChar(text)
    }
}
