import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import RegisteredTypes


RoundIconButton
{
    Material.background: "transparent"
    Material.foreground: "transparent"
    background: Rectangle
    {
        radius: screenManager.spacer()
        color: "transparent"
    }
}
