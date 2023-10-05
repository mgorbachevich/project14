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

    Connections // Slot for signal AppManager::showMainPage:
    {
        target: app
        function onShowMainPage(index)
        {
            if (index === 2)
            {
                console.debug("@@@@@ searchPanel onShowMainPage");
                searchPanelTextField.forceActiveFocus()
            }
        }
    }

    GridLayout
    {
        id: searchPanelLayout
        anchors.fill: parent
        anchors.leftMargin: app.spacer()
        anchors.rightMargin: app.spacer()
        anchors.topMargin: app.spacer()
        anchors.bottomMargin: app.spacer() * 2
        columnSpacing: app.spacer()
        rowSpacing: app.spacer()
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
            onClicked:
            {
                app.onUserAction();
                app.onSearchOptionsClicked()
            }
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
                onFlickStarted: app.onUserAction()

                ScrollBar.vertical: ScrollBar
                {
                    width: app.spacer()
                    background: Rectangle { color: "transparent" }
                    policy: ScrollBar.AlwaysOn
                }

                model: searchPanelModel
                delegate: Label
                {
                    padding: app.spacer()
                    width: searchPanelResultListRectangle.width
                    font { pointSize: app.normalFontSize() }
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
                        onClicked:
                        {
                            app.onUserAction();
                            app.onSearchResultClicked(index)
                        }
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
            Layout.bottomMargin: app.spacer()
            Material.accent: Material.Orange
            color: Material.color(Material.Grey, Material.Shade900)
            font { pointSize: app.normalFontSize() }
            placeholderText: "?????"
            //inputMethodHints: Qt.ImhDigitsOnly // Keyboard
            onTextEdited: app.onUserAction();
            focus: true
            Keys.onPressed: (event) =>
            {
                console.debug("@@@@@ searchPanelTextField Keys.onPressed ", JSON.stringify(event));
                event.accepted = true;
                app.onUserAction();
                switch (event.key)
                {
                    case Qt.Key_0: case Qt.Key_1: case Qt.Key_2: case Qt.Key_3: case Qt.Key_4:
                    case Qt.Key_5: case Qt.Key_6: case Qt.Key_7: case Qt.Key_8: case Qt.Key_9:
                        text += event.text
                        app.onSearchFilterEdited(text);
                        break;
                    case Qt.Key_Backspace: case Qt.Key_Delete: case Qt.Key_C:
                        text = text.substring(0, text.length - 1);
                        app.onSearchFilterEdited(text);
                        break;
                    case Qt.Key_Escape:
                        text = ""
                        app.onSearchFilterEdited(text);
                        break;
                    case Qt.Key_Left:
                        app.onSwipeMainPage(1)
                        break;
                    case Qt.Key_F9: // Ключ
                        app.onLockClicked()
                        break;
                    case Qt.Key_T: // >T<
                        app.onTareClicked()
                        break
                    case Qt.Key_Z: // >0<
                        app.onZeroClicked()
                        break
                    case Qt.Key_Up:
                        if (!searchPanelResultList.atYBeginning) searchPanelResultList.flick(0, Constants.flickVelocity)
                        else app.beep()
                        break;
                    case Qt.Key_Down:
                        if (!searchPanelResultList.atYEnd) searchPanelResultList.flick(0, -Constants.flickVelocity)
                        else app.beep()
                        break;
                    default:
                        app.beep();
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
                onFlickStarted: app.onUserAction()
                //highlight: Rectangle { color: Material.color(Material.BlueGrey, Material.Shade100) }

                ScrollBar.vertical: ScrollBar
                {
                    width: app.spacer()
                    background: Rectangle { color: "transparent" }
                    policy: ScrollBar.AlwaysOn
                }

                model: searchFilterModel
                delegate: Label
                {
                    width: searchPanelFilterList.width
                    padding: app.spacer()
                    font
                    {
                        pointSize: app.normalFontSize();
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
                            app.onUserAction();
                            searchPanelFilterList.currentIndex = index
                            app.onSearchFilterClicked(index)
                        }
                    }
                }
            }
        }
    }
}
