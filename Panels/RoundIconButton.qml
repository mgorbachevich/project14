import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import "../constants.js" as Constants
import RegisteredTypes

RoundButton
{
    width: app.buttonSize()
    height: app.buttonSize()
    radius: app.spacer()
    leftInset: app.spacer() / 4
    topInset: app.spacer() / 4
    rightInset: app.spacer() / 4
    bottomInset: app.spacer() / 4
    horizontalPadding: app.spacer() / 4
    verticalPadding: app.spacer() / 4
    flat: true
    icon.source: "../Icons/empty_48"
    Material.background: Material.color(Material.BlueGrey, Material.Shade200)
    Material.foreground: Material.color(Material.BlueGrey, Material.Shade900)
}
