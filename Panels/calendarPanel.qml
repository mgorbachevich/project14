import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import RegisteredTypes


Popup
{
    id: calendarPanel
    padding : 0
    //closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
    closePolicy: Popup.CloseOnEscape
    focus: true
    modal: true
    dim: true
    Material.background: "transparent"
    property string titleText: ""
    property int comboWidth: screenManager.normalFontSize() * 6
    property int code: 0
    property int startCalendarYear: 2024
    property int calendarDay: 1
    property int calendarMonth: 1
    property int calendarYear: 2024
    property int calendarHour: 0
    property int calendarMinute: 0
    property int calendarSecond: 0
    onOpened: app.onPopupOpened(true)
    onClosed:
    {
        app.onPopupOpened(false)
        app.onCalendarClosed(code,
                             calendarPanelDayCombo.displayText,
                             calendarPanelMonthCombo.displayText,
                             calendarPanelYearCombo.displayText,
                             calendarPanelHourCombo.displayText,
                             calendarPanelMinuteCombo.displayText,
                             calendarPanelSecondCombo.displayText)
    }

    Rectangle
    {
        anchors.fill: parent
        radius: screenManager.spacer()
        Material.background: Material.color(Material.Grey, Material.Shade100)
        color: Material.color(Material.Grey, Material.Shade100)

        GridLayout
        {
            anchors.fill: parent
            anchors.margins: screenManager.spacer()
            columnSpacing: 0
            rowSpacing: 0
            focus: true
            Keys.onPressed: (event) =>
            {
                //app.debugLog("@@@@@ comboSettingPanel Keys.onPressed %1".arg(JSON.stringify(event)))
                event.accepted = true;
                app.clickSound();
                app.onUserAction();
                switch (event.key)
                {
                    case Qt.Key_F10: // Промотка
                        app.onRewind()
                        break
                    default:
                        break
                }
            }

            EmptyButton
            {
                Layout.column: 0
                Layout.row: 0
                Layout.preferredHeight: screenManager.buttonSize()
                Layout.alignment: Qt.AlignTop | Qt.AlignLeft
            }

            Rectangle
            {
                Layout.column: 1
                Layout.row: 0
                Layout.columnSpan: 3
                Layout.fillWidth: parent
                Layout.preferredHeight: screenManager.buttonSize()
                color: "transparent"

                CardTitleText { text: titleText }
            }

            RoundIconButton
            {
                Layout.column: 4
                Layout.row: 0
                Layout.alignment: Qt.AlignTop | Qt.AlignRigth
                Layout.preferredHeight: screenManager.buttonSize()
                icon.source: "../Icons/close"
                onClicked:
                {
                    app.onUserAction();
                    calendarPanel.close()
                }
            }

            Spacer
            {
                Layout.column: 2
                Layout.row: 1
                Layout.fillHeight: parent
            }

            SubtitleText
            {
                Layout.column: 1
                Layout.row: 2
                text: qsTr("Число")
            }

            SubtitleText
            {
                Layout.column: 2
                Layout.row: 2
                text: qsTr("Месяц")
            }

            SubtitleText
            {
                Layout.column: 3
                Layout.row: 2
                text: qsTr("Год")
            }

            ComboBox
            {
                id: calendarPanelDayCombo
                Layout.column: 1
                Layout.row: 3
                Layout.preferredWidth: comboWidth
                editable: false
                popup.modal: true
                currentIndex: calendarDay - 1
                displayText: calendarDay

                model: calendarDayModel
                delegate: Text
                {
                    padding: screenManager.spacer()
                    font { pointSize: screenManager.normalFontSize() }
                    color: Material.color(Material.BlueGrey, Material.Shade900)
                    text: model.value // Roles::ValueRole

                    MouseArea
                    {
                        anchors.fill: parent
                        onClicked:
                        {
                            app.onUserAction();
                            calendarPanelDayCombo.currentIndex = index
                            calendarPanelDayCombo.displayText = index + 1
                            calendarPanelDayCombo.popup.close()
                        }
                    }
                }
            }

            ComboBox
            {
                id: calendarPanelMonthCombo
                Layout.column: 2
                Layout.row: 3
                Layout.preferredWidth: comboWidth
                editable: false
                popup.modal: true
                currentIndex: calendarMonth - 1
                displayText: calendarMonth

                model: calendarMonthModel
                delegate: Text
                {
                    padding: screenManager.spacer()
                    font { pointSize: screenManager.normalFontSize() }
                    color: Material.color(Material.BlueGrey, Material.Shade900)
                    text: model.value // Roles::ValueRole

                    MouseArea
                    {
                        anchors.fill: parent
                        onClicked:
                        {
                            app.onUserAction();
                            calendarPanelMonthCombo.currentIndex = index
                            calendarPanelMonthCombo.displayText = index + 1
                            calendarPanelMonthCombo.popup.close()
                        }
                    }
                }
            }

            ComboBox
            {
                id: calendarPanelYearCombo
                Layout.column: 3
                Layout.row: 3
                Layout.preferredWidth: comboWidth * 3 / 2
                editable: false
                popup.modal: true
                currentIndex: calendarYear - startCalendarYear
                displayText: calendarYear

                model: calendarYearModel
                delegate: Text
                {
                    padding: screenManager.spacer()
                    font { pointSize: screenManager.normalFontSize() }
                    color: Material.color(Material.BlueGrey, Material.Shade900)
                    text: model.value // Roles::ValueRole

                    MouseArea
                    {
                        anchors.fill: parent
                        onClicked:
                        {
                            app.onUserAction();
                            calendarPanelYearCombo.currentIndex = index
                            calendarPanelYearCombo.displayText = index + startCalendarYear
                            calendarPanelYearCombo.popup.close()
                        }
                    }
                }
            }

            SubtitleText
            {
                Layout.column: 1
                Layout.row: 4
                text: qsTr("Часы")
            }

            SubtitleText
            {
                Layout.column: 2
                Layout.row: 4
                text: qsTr("Минуты")
            }

            SubtitleText
            {
                Layout.column: 3
                Layout.row: 4
                text: qsTr("Секунды")
            }

            ComboBox
            {
                id: calendarPanelHourCombo
                Layout.column: 1
                Layout.row: 5
                Layout.preferredWidth: comboWidth
                editable: false
                popup.modal: true
                currentIndex: calendarHour
                displayText: calendarHour
                visible: calendarHour >= 0

                model: calendarHourModel
                delegate: Text
                {
                    padding: screenManager.spacer()
                    font { pointSize: screenManager.normalFontSize() }
                    color: Material.color(Material.BlueGrey, Material.Shade900)
                    text: model.value // Roles::ValueRole

                    MouseArea
                    {
                        anchors.fill: parent
                        onClicked:
                        {
                            app.onUserAction();
                            calendarPanelHourCombo.currentIndex = index
                            calendarPanelHourCombo.displayText = index
                            calendarPanelHourCombo.popup.close()
                        }
                    }
                }
            }

            ComboBox
            {
                id: calendarPanelMinuteCombo
                Layout.column: 2
                Layout.row: 5
                Layout.preferredWidth: comboWidth
                editable: false
                popup.modal: true
                currentIndex: calendarMinute
                displayText: calendarMinute
                visible: calendarHour >= 0

                model: calendarMinuteModel
                delegate: Text
                {
                    padding: screenManager.spacer()
                    font { pointSize: screenManager.normalFontSize() }
                    color: Material.color(Material.BlueGrey, Material.Shade900)
                    text: model.value // Roles::ValueRole

                    MouseArea
                    {
                        anchors.fill: parent
                        onClicked:
                        {
                            app.onUserAction();
                            calendarPanelMinuteCombo.currentIndex = index
                            calendarPanelMinuteCombo.displayText = index
                            calendarPanelMinuteCombo.popup.close()
                        }
                    }
                }
            }

            ComboBox
            {
                id: calendarPanelSecondCombo
                Layout.column: 3
                Layout.row: 5
                Layout.preferredWidth: comboWidth * 3 / 2
                editable: false
                popup.modal: true
                currentIndex: calendarSecond
                displayText: calendarSecond
                visible: calendarHour >= 0

                model: calendarSecondModel
                delegate: Text
                {
                    padding: screenManager.spacer()
                    font { pointSize: screenManager.normalFontSize() }
                    color: Material.color(Material.BlueGrey, Material.Shade900)
                    text: model.value // Roles::ValueRole

                    MouseArea
                    {
                        anchors.fill: parent
                        onClicked:
                        {
                            app.onUserAction();
                            calendarPanelSecondCombo.currentIndex = index
                            calendarPanelSecondCombo.displayText = index
                            calendarPanelSecondCombo.popup.close()
                        }
                    }
                }
            }

            Spacer
            {
                Layout.column: 2
                Layout.row: 6
            }

            RoundTextButton
            {
                Layout.column: 0
                Layout.row: 7
                Layout.columnSpan: 5
                Layout.alignment: Qt.AlignCenter
                Layout.preferredHeight: screenManager.buttonSize()
                text: "ТЕКУЩЕЕ ВРЕМЯ"
                onClicked:
                {
                    app.onUserAction();
                    calendarPanelYearCombo.displayText = -1
                    calendarPanel.close()
                }
            }

            Spacer
            {
                Layout.column: 2
                Layout.row: 8
                Layout.fillHeight: parent
            }
        }
    }
}


