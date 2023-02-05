import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import "../constants.js" as Constants
import RegisteredTypes 1.0

Rectangle
{
    id:  showcasePanel
    color: Material.background
    property int imageSize: (height - Constants.margin * 3) / 2

    GridView
    {
        id: showcasePanelGrid
        anchors.fill: parent
        anchors.rightMargin: Constants.margin
        leftMargin: Constants.margin
        topMargin: Constants.margin
        bottomMargin: Constants.margin * 2
        clip: true
        cellWidth: showcasePanel.imageSize + Constants.margin
        cellHeight: showcasePanel.imageSize + Constants.margin
        ScrollBar.vertical: ScrollBar
        {
            width: Constants.margin
            background: Rectangle { color: "transparent" }
            policy: ScrollBar.AlwaysOn
        }

        model: showcasePanelModel
        delegate: Image
        {
            width: showcasePanel.imageSize
            height: showcasePanel.imageSize
            source: model.value  // Roles::ValueRole
            MouseArea
            {
                anchors.fill: parent
                onClicked: app.onShowcaseClicked(index) // AppManager's slot
            }
        }
    }
}


