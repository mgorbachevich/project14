import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import RegisteredTypes


RoundButton
{
    property bool marked: false

    width: screenManager.buttonSize()
    height: screenManager.buttonSize()
    Layout.preferredWidth: width
    Layout.preferredHeight: height
    radius: screenManager.spacer()
    leftInset: screenManager.spacer() / 4
    topInset: screenManager.spacer() / 4
    rightInset: screenManager.spacer() / 4
    bottomInset: screenManager.spacer() / 4
    horizontalPadding: screenManager.spacer() / 4
    verticalPadding: screenManager.spacer() / 4
    flat: true
    icon.source: "../Icons/empty"
    Material.background: marked ? Material.accent : Material.color(Material.BlueGrey, Material.Shade100)
    Material.foreground: Material.color(Material.BlueGrey, enabled ? Material.Shade900 : Material.Shade300)
    background: Rectangle
    {
        color: marked ? Material.accent : Material.color(Material.BlueGrey, Material.Shade100)
        radius: screenManager.spacer()
    }
}
