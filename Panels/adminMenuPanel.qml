import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import "../constants.js" as Constants
import RegisteredTypes

Column
{
    anchors.horizontalCenter: parent.horizontalCenter
    spacing: screenManager.spacer() / 4

    RoundIconButton
    {
        anchors.horizontalCenter: parent.horizontalCenter
        icon.source: "../Icons/settings_black_48"
        onClicked: app.onAdminSettingsClicked()
    }

    RoundIconButton
    {
        anchors.horizontalCenter: parent.horizontalCenter
        icon.source: "../Icons/lock_black_48"
        onClicked: app.onLockClicked()
    }

    RoundIconButton
    {
        anchors.horizontalCenter: parent.horizontalCenter
        icon.source: "../Icons/log_black_48"
        onClicked: app.onViewLogClicked()
    }
}
