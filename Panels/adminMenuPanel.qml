import QtQuick
import QtQuick.Controls.Material
import QtQuick.Layouts
import "../constants.js" as Constants
import RegisteredTypes 1.0

Column
{
    width: parent.width
    height: parent.height
    topPadding: Constants.margin
    spacing: Constants.margin

    Button
    {
        anchors.horizontalCenter: parent.horizontalCenter
        width: Constants.buttonSize
        height: Constants.buttonSize
        icon { width: Constants.iconSize; height: Constants.iconSize; source: "../Icons/menu_black_48" }
        leftInset: 0
        topInset: 0
        rightInset: 0
        bottomInset: 0
        Material.background: Material.primary
        //onClicked: app.onTableBackClicked() // AppManager's slot
    }

    Button
    {
        anchors.horizontalCenter: parent.horizontalCenter
        width: Constants.buttonSize
        height: Constants.buttonSize
        icon { width: Constants.iconSize; height: Constants.iconSize; source: "../Icons/menu_black_48" }
        leftInset: 0
        topInset: 0
        rightInset: 0
        bottomInset: 0
        Material.background: Material.primary
        //onClicked: app.onTableBackClicked() // AppManager's slot
    }
}
