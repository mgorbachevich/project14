import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import RegisteredTypes

Label
{
    horizontalAlignment: Text.AlignHCenter
    verticalAlignment: Text.AlignVCenter
    leftPadding: screenManager.spacer()
    rightPadding: screenManager.spacer()
    wrapMode: Text.Wrap
    maximumLineCount: 1
    text: ""
    background: Rectangle
    {
        color: "#B0FFFFFF"
        radius: screenManager.normalFontSize() / 2
    }
}
