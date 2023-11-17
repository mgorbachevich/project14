import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import "../constants.js" as Constants
import RegisteredTypes

RoundButton
{
    height: app.buttonSize()
    radius: app.spacer()
    leftInset: app.spacer() / 4
    topInset: app.spacer() / 4
    rightInset: app.spacer() / 4
    bottomInset: app.spacer() / 4
    horizontalPadding: app.spacer() * 2
    verticalPadding: app.spacer() / 4
    Material.background: Material.color(Material.BlueGrey, Material.Shade200)
    Material.foreground: Material.color(Material.BlueGrey, Material.Shade900)
    font { pointSize: app.normalFontSize() }
    flat: true
}
