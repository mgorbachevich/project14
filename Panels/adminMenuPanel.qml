import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import "../constants.js" as Constants
import RegisteredTypes

Column
{
    anchors.horizontalCenter: parent.horizontalCenter
    topPadding: app.spacer()
    spacing: app.spacer()

    RoundIconButton
    {
        anchors.horizontalCenter: parent.horizontalCenter
        icon.source: "../Icons/settings_black_48"
        onClicked:
        {
            app.onUserAction(); // AppManager's slot
            app.onAdminSettingsClicked() // AppManager's slot
        }
    }

    RoundIconButton
    {
        anchors.horizontalCenter: parent.horizontalCenter
        icon.source: "../Icons/lock_black_48"
        onClicked:
        {
            app.onUserAction(); // AppManager's slot
            app.onLockClicked() // AppManager's slot
        }
    }

    RoundIconButton
    {
        anchors.horizontalCenter: parent.horizontalCenter
        icon.source: "../Icons/log_black_48"
        onClicked:
        {
            app.onUserAction(); // AppManager's slot
            app.onViewLogClicked() // AppManager's slot
        }
    }
}
