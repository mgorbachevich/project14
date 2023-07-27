import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import "../constants.js" as Constants
import RegisteredTypes

RoundButton
{
    horizontalPadding: Constants.margin * 2
    height: Constants.buttonSize
    radius: Constants.margin
    leftInset: 0
    topInset: 0
    rightInset: 0
    bottomInset: 0
    Material.background: Material.color(Material.BlueGrey, Material.Shade200)
    Material.foreground: Material.color(Material.BlueGrey, Material.Shade900)
    font { pointSize: Constants.normalFontSize }
    flat: true
}
