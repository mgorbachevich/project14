import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import "../constants.js" as Constants
import RegisteredTypes 1.0

Rectangle
{
    id:  searchPanel
    objectName: "searchPanel"
    color: Material.background
    property int filterWidth: width * 0.25

    Connections // Slot for signal AppManager::activateMainWindow:
    {
        target: app
        function onActivateMainWindow()
        {
            console.debug("@@@@@ searchPanel onActivateMainWindow");
            searchPanelTextField.focus = true
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

        Text
        {
            id: searchPanelTitle
            Layout.column: 0
            Layout.row: 0
            Layout.columnSpan: 2
            Layout.alignment: Qt.AlignHCenter
            font { pointSize: Constants.normalFontSize; family: "Roboto"; styleName:'Italic' }
            color: Material.color(Material.Grey, Material.Shade600)
            text: qsTr("Поиск товаров")
        }

        Button
        {
            id: searchPanelOptionsButton
            Layout.column: 2
            Layout.row: 0
            Layout.alignment: Qt.AlignTop | Qt.AlignRight
            Layout.preferredWidth: Constants.buttonSize
            Layout.preferredHeight: Constants.buttonSize
            icon { width: Constants.iconSize; height: Constants.iconSize; source: "../Icons/settings_black_48" }
            leftInset: 0
            topInset: 0
            rightInset: 0
            bottomInset: 0
            Material.background: Material.primary
            onClicked: app.onSearchOptionsClicked() // AppManager's slot
        }

        Rectangle
        {
            Layout.column: 0
            Layout.row: 1
            Layout.rowSpan: 2
            Layout.fillWidth: parent
            Layout.fillHeight: parent
            color: Material.color(Material.Grey, Material.Shade50)

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
                delegate: Text
                {
                    padding: Constants.margin
                    font { pointSize: Constants.normalFontSize; family: "Roboto" }
                    text: model.value // Roles::ValueRole
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
            font { pointSize: Constants.normalFontSize; family: "Roboto"; styleName:'Regular' }
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
                        app.onWeightParam(1);
                        break;
                    case Qt.Key_Z: // >0<
                        app.onWeightParam(0);
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
            color: Material.color(Material.Grey, Material.Shade50)

            ListView
            {
                id: searchPanelFilterList
                anchors.fill: parent
                orientation: Qt.Vertical
                clip: true
                currentIndex: 0
                highlight: Rectangle { color: Material.primary }
                /*
                ScrollBar.vertical: ScrollBar
                {
                    width: Constants.margin
                    background: Rectangle { color: "transparent" }
                    policy: ScrollBar.AlwaysOn
                }
                */

                model: searchFilterModel
                delegate: Text
                {
                    //width: parent.width
                    padding: Constants.margin
                    font { pointSize: Constants.normalFontSize; family: "Roboto" }
                    text: model.value // Roles::ValueRole
                    //color: searchPanelFilterList.currentIndex === index ? "white" : "black"

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
