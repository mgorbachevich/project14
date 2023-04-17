import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import "../constants.js" as Constants
import RegisteredTypes 1.0

Column
{
    anchors.horizontalCenter: parent.horizontalCenter
    topPadding: Constants.margin
    spacing: Constants.margin

    Button
    {
        anchors.horizontalCenter: parent.horizontalCenter
        width: Constants.buttonSize
        height: Constants.buttonSize
        icon.source: "../Icons/build_black_48"
        leftInset: 0
        topInset: 0
        rightInset: 0
        bottomInset: 0
        Material.background: Material.primary
        onClicked: app.onAdminSettingsClicked() // AppManager's slot
    }

    Button
    {
        anchors.horizontalCenter: parent.horizontalCenter
        width: Constants.buttonSize
        height: Constants.buttonSize
        icon.source: "../Icons/lock_black_48"
        leftInset: 0
        topInset: 0
        rightInset: 0
        bottomInset: 0
        Material.background: Material.primary
        onClicked: app.onLockClicked() // AppManager's slot
    }

    Button
    {
        id: viewLogButton
        anchors.horizontalCenter: parent.horizontalCenter
        width: Constants.buttonSize
        height: Constants.buttonSize
        icon.source: "../Icons/log_black_48"
        leftInset: 0
        topInset: 0
        rightInset: 0
        bottomInset: 0
        Material.background: Material.primary
        onClicked: app.onViewLogClicked() // AppManager's slot
    }
}
