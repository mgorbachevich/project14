import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import "../constants.js" as Constants
import RegisteredTypes

Rectangle
{
    id:  searchPanel
    color: Material.background
    property int filterWidth: width * 0.25

    Connections // Slot for signal AppManager::activateMainPage:
    {
        target: app
        function onActivateMainPage(index)
        {
            if (index === 2)
            {
                console.debug("@@@@@ searchPanel onActivateMainPage");
                searchPanelTextField.forceActiveFocus()
            }
        }
    }

    GridLayout
    {
        id: searchPanelLayout
        anchors.fill: parent
        anchors.leftMargin: Constants.margin
        anchors.rightMargin: Constants.margin
        anchors.topMargin: Constants.margin
        anchors.bottomMargin: Constants.margin * 2
        columnSpacing: Constants.margin
        rowSpacing: Constants.margin
        columns: 3
        rows: 3

        Rectangle
        {
            Layout.column: 0
            Layout.row: 0
            Layout.columnSpan: 2
            Layout.fillWidth: parent
            color: "transparent"

            CardTitleText
            {
                text: qsTr("Поиск товаров")
            }
        }

        RoundIconButton
        {
            id: searchPanelOptionsButton
            Layout.column: 2
            Layout.row: 0
            Layout.alignment: Qt.AlignTop | Qt.AlignRight
            icon.source: "../Icons/settings_black_48"
            onClicked: app.onSearchOptionsClicked() // AppManager's slot
        }

        Rectangle
        {
            id: searchPanelResultListRectangle
            Layout.column: 0
            Layout.row: 1
            Layout.rowSpan: 2
            Layout.fillWidth: parent
            Layout.fillHeight: parent
            color: "transparent"

            ListView
            {
                id: searchPanelResultList
                anchors.fill: parent
                orientation: Qt.Vertical
                clip: true

                ScrollBar.vertical: ScrollBar
                {
                    width: Constants.margin
                    background: Rectangle { color: "transparent" }
                    policy: ScrollBar.AlwaysOn
                }

                model: searchPanelModel
                delegate: Label
                {
                    padding: Constants.margin
                    width: searchPanelResultListRectangle.width
                    font { pointSize: Constants.normalFontSize }
                    color: Material.color(Material.Grey, Material.Shade900)
                    text: model.value // Roles::ValueRole
                    background: Rectangle
                    {
                        color: index % 2 === 0 ? Material.color(Material.Grey, Material.Shade50) :
                                                 Material.color(Material.Grey, Material.Shade200)
                    }

                    MouseArea
                    {
                        anchors.fill: parent
                        onClicked: app.onSearchResultClicked(index) // AppManager's slot
                    }
                }
            }
        }

        TextField
        {
            id: searchPanelTextField
            Layout.column: 1
            Layout.row: 1
            Layout.columnSpan: 2
            Layout.preferredWidth: filterWidth
            Layout.bottomMargin: Constants.margin
            Material.accent: Material.Orange
            color: Material.color(Material.Grey, Material.Shade900)
            font { pointSize: Constants.normalFontSize }
            placeholderText: "?????"
            //inputMethodHints: Qt.ImhDigitsOnly // Keyboard
            //onTextEdited: app.onSearchFilterEdited(text) // AppManager's slot

            focus: true
            Keys.onPressed: (event) =>
            {
                console.debug("@@@@@ searchPanelTextField Keys.onPressed ", JSON.stringify(event));
                event.accepted = true;
                switch (event.key)
                {
                    case Qt.Key_0:
                    case Qt.Key_1:
                    case Qt.Key_2:
                    case Qt.Key_3:
                    case Qt.Key_4:
                    case Qt.Key_5:
                    case Qt.Key_7:
                    case Qt.Key_8:
                    case Qt.Key_9:
                        text += event.text
                        app.onSearchFilterEdited(text);
                        break;
                    case Qt.Key_C:
                        text = ""
                        app.onSearchFilterEdited(text);
                        break;
                    case Qt.Key_F9: // Ключ
                        app.onLockClicked()
                        break;
                    case Qt.Key_T: // >T<
                        app.onWeightParamClicked(1);
                        break;
                    case Qt.Key_Z: // >0<
                        app.onWeightParamClicked(0);
                        break;
                    case Qt.Key_Left:
                        app.onShowMainPage(1)
                        break;
                    case Qt.Key_Up:
                        if (!searchPanelResultList.atYBeginning) searchPanelResultList.flick(0, Constants.flickVelocity)
                        else app.onBeep()
                        break;
                    case Qt.Key_Down:
                        if (!searchPanelResultList.atYEnd) searchPanelResultList.flick(0, -Constants.flickVelocity)
                        else app.onBeep()
                        break;
                    default:
                        app.onBeep();
                        break;
                }
            }
        }

        Rectangle
        {
            Layout.column: 1
            Layout.row: 2
            Layout.columnSpan: 2
            Layout.preferredWidth: filterWidth
            Layout.fillHeight: parent
            Layout.alignment: Qt.AlignTop
            color: "transparent"

            ListView
            {
                id: searchPanelFilterList
                anchors.fill: parent
                orientation: Qt.Vertical
                clip: true
                currentIndex: 0
                //highlight: Rectangle { color: Material.color(Material.BlueGrey, Material.Shade100) }

                ScrollBar.vertical: ScrollBar
                {
                    width: Constants.margin
                    background: Rectangle { color: "transparent" }
                    policy: ScrollBar.AlwaysOn
                }

                model: searchFilterModel
                delegate: Label
                {
                    width: parent.width
                    padding: Constants.margin
                    font
                    {
                        pointSize: Constants.normalFontSize;
                        styleName: searchPanelFilterList.currentIndex === index ? 'Bold' : 'Regular'
                    }
                    text: model.value // Roles::ValueRole
                    color: searchPanelFilterList.currentIndex === index ? Constants.colorBlack : Material.color(Material.BlueGrey, Material.Shade600)
                    background: Rectangle
                    {
                        color: index % 2 === 0 ? Material.color(Material.Grey, Material.Shade50) :
                                                 Material.color(Material.Grey, Material.Shade200)
                    }

                    MouseArea
                    {
                        anchors.fill: parent
                        onClicked:
                        {
                            searchPanelFilterList.currentIndex = index
                            app.onSearchFilterClicked(index) // AppManager's slot
                        }
                    }
                }
            }
        }
    }
}
