import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import "../constants.js" as Constants
import RegisteredTypes

RoundTextButton
{
    width: screenManager.buttonSize()
    Layout.preferredWidth: screenManager.buttonSize()
    Layout.preferredHeight: screenManager.buttonSize()
    focusPolicy: Qt.NoFocus
    font { pointSize: screenManager.largeFontSize() }
    background: Rectangle
    {
        color: Material.color(Material.BlueGrey, Material.Shade200)
        radius: screenManager.spacer()
    }
}
