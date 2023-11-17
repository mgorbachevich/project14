import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import "../constants.js" as Constants
import RegisteredTypes

RoundTextButton
{
    width: app.buttonSize()
    focusPolicy: Qt.NoFocus
    font { pointSize: app.largeFontSize() }
}
