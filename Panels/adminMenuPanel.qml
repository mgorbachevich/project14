import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import "../constants.js" as Constants
import RegisteredTypes

Column
{
    anchors.horizontalCenter: parent.horizontalCenter
    padding: app.spacer() / 2
    spacing: app.spacer() / 4

    RoundIconButton
    {
        anchors.horizontalCenter: parent.horizontalCenter
        icon.source: "../Icons/settings_black_48"
        onClicked:
        {
            app.onUserAction();
            app.onAdminSettingsClicked()
        }
    }

    RoundIconButton
    {
        anchors.horizontalCenter: parent.horizontalCenter
        icon.source: "../Icons/lock_black_48"
        onClicked:
        {
            app.onUserAction();
            app.onLockClicked()
        }
    }

    RoundIconButton
    {
        anchors.horizontalCenter: parent.horizontalCenter
        icon.source: "../Icons/log_black_48"
        onClicked:
        {
            app.onUserAction();
            app.onViewLogClicked()
        }
    }
}
