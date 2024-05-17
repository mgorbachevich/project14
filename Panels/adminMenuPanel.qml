import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import RegisteredTypes


Column
{
    anchors.horizontalCenter: parent.horizontalCenter
    spacing: 0

    Spacer {}

    RoundIconButton
    {
        anchors.horizontalCenter: parent.horizontalCenter
        icon.source: "../Icons/settings"
        onClicked: app.onAdminSettingsClicked()
    }

    Spacer {}

    RoundIconButton
    {
        anchors.horizontalCenter: parent.horizontalCenter
        icon.source: "../Icons/lock"
        onClicked: app.onLockClicked()
    }

    Spacer {}

    RoundIconButton
    {
        anchors.horizontalCenter: parent.horizontalCenter
        icon.source: "../Icons/log"
        onClicked: app.onViewLogClicked()
    }
}

