import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import "../constants.js" as Constants
import RegisteredTypes

Image
{
    width: screenManager.flagSize()
    height: screenManager.flagSize()
    Layout.preferredWidth: width
    Layout.preferredHeight: height
    source: "../Icons/empty"
    Material.background: "transparent"
    Material.foreground: Material.color(Material.BlueGrey, Material.Shade900)
}
