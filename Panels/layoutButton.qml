import QtQuick
import QtQuick.Controls.Material
import QtQuick.Layouts
import "../constants.js" as Constants

Button
{
    Layout.preferredWidth: Constants.buttonSize
    Layout.preferredHeight: Constants.buttonSize
    icon { width: Constants.iconSize; height: Constants.iconSize }
    leftInset: 0
    topInset: 0
    rightInset: 0
    bottomInset: 0
    background: Rectangle { color: Constants.buttonBackColor; radius: Constants.buttonRadius }
}
