import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import "../constants.js" as Constants
import RegisteredTypes

Column
{
    anchors.horizontalCenter: parent.horizontalCenter
    topPadding: Constants.margin
    spacing: Constants.margin

    RoundIconButton
    {
        anchors.horizontalCenter: parent.horizontalCenter
        icon.source: "../Icons/settings_black_48"
        onClicked: app.onAdminSettingsClicked() // AppManager's slot
    }

    RoundIconButton
    {
        anchors.horizontalCenter: parent.horizontalCenter
        icon.source: "../Icons/lock_black_48"
        onClicked: app.onLockClicked() // AppManager's slot
    }

    RoundIconButton
    {
        anchors.horizontalCenter: parent.horizontalCenter
        icon.source: "../Icons/log_black_48"
        onClicked: app.onViewLogClicked() // AppManager's slot
    }
}
