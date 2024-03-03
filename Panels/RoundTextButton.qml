import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import "../constants.js" as Constants
import RegisteredTypes

RoundButton
{
    height: screenManager.buttonSize()
    Layout.preferredHeight: screenManager.buttonSize()
    radius: screenManager.spacer()
    leftInset: screenManager.spacer() / 4
    topInset: screenManager.spacer() / 4
    rightInset: screenManager.spacer() / 4
    bottomInset: screenManager.spacer() / 4
    horizontalPadding: screenManager.spacer() * 2
    verticalPadding: screenManager.spacer() / 4
    Material.background: Material.color(Material.BlueGrey, Material.Shade200)
    Material.foreground: Material.color(Material.BlueGrey, enabled ? Material.Shade900 : Material.Shade50)
    font { pointSize: screenManager.normalFontSize() }
    flat: true
}
