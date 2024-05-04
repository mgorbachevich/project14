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
        icon.source: "../Icons/settings"
        onClicked: app.onAdminSettingsClicked()
    }

    RoundIconButton
    {
        anchors.horizontalCenter: parent.horizontalCenter
        icon.source: "../Icons/lock"
        onClicked: app.onLockClicked()
    }

    RoundIconButton
    {
        anchors.horizontalCenter: parent.horizontalCenter
        icon.source: "../Icons/log"
        onClicked: app.onViewLogClicked()
    }

    RoundIconButton
    {
        anchors.horizontalCenter: parent.horizontalCenter
        icon.source: "../Icons/user"
        onClicked: app.onEditUsersClicked()
    }
}
