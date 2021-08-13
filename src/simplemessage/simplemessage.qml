import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

ApplicationWindow {
    id: window
    title: qsTr("gyro")
    flags: Qt.Window | Qt.WindowTitleHint | Qt.CustomizeWindowHint

    visible: true
    width: 600; height: 500

    TextEdit {
        id: cliphelper
        visible: false
    }

    readonly property int responsiveWidth: 500
    readonly property int minContactWidth: 100
    readonly property int maxContactWidth: 500

    readonly property string rightbkcolor: "#e0e0e0"
    readonly property string panebkcolor: "#e0e0e0"
    readonly property string messagebkcolor: "#eeeeee"
    readonly property string selecteditembkcolor: "#4a90d9"
    readonly property string normalitembkcolor: "white"

    property string current_id: ""

    SwipeView  {
        id: swipeView
        currentIndex: 0
        anchors.fill: parent
        states: [
            State {
                when: window.width >= responsiveWidth
                ParentChange { target: contacts_rectangle; parent: contactsContainer; }
                ParentChange { target: message_rectangle; parent: messageContainer; }
                PropertyChanges { target: splitter; enabled: true }
            }
        ]
        Item {
            Rectangle {
                anchors.fill: contacts_rectangle
                color: "white"
            }

            Rectangle {
                id: contacts_rectangle
                anchors.fill: parent
                border.width: 0;

                ListView {
                    id: contacts
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.top: header.bottom
                    anchors.bottom: parent.bottom
                    Layout.margins: 0
                    displayMarginBeginning: 0
                    displayMarginEnd: 0
                    verticalLayoutDirection: ListView.TopToBottom
                    topMargin: 3
                    currentIndex: -1
                    model: contact_model
                    delegate: Row {

                        readonly property bool orderBy: index % 2 == 0
                        readonly property bool selected: id == current_id

                        property bool if_selected:
                        {
                            if(selected && parent.currentIndex != index)
                                parent.currentIndex = index;
                            return selected
                        }

                        Rectangle {
                            width: 2
                            height: 40
                            color: normalitembkcolor
                        }

                        Rectangle {
                            radius: 8
                            y:1
                            width: 38
                            height: 38
                            color: short_background
                            Label {
                                anchors.centerIn: parent
                                text: short_name
                                font.pointSize: 16
                                color: short_color
                            }
                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    current_id = id
                                    message_model.set_current_contact(id, name)
                                    contacts.currentIndex = -1
                                }
                            }
                        }

                        Rectangle {
                            width: 2
                            height: 40
                            color: normalitembkcolor
                        }

                        Rectangle {
                            width: contacts.width - 44
                            height: 40
                            color: selected ? selecteditembkcolor : orderBy ? rightbkcolor : normalitembkcolor

                            Label {
                                leftPadding: 3
                                y: 0
                                anchors.left:  parent.left
                                anchors.right: labelTimeField.left
                                text: name
                                font.bold: true
                                color: selected ? "white" : "black"
                                height: 20
                                maximumLineCount: 1
                                elide: Text.ElideRight
                            }

                            Label {
                                rightPadding: 3
                                id: labelTimeField
                                y: 0
                                anchors.right: parent.right
                                text: tm
                                font.pointSize: 10
                                color: selected ? "white" : "#707070"
                                height: 20
                                elide: Text.ElideLeft
                            }

                            Label {
                                leftPadding: 3
                                y: 24
                                anchors.left:  parent.left
                                anchors.right: unread_cnt > 0 ? newMessageCountField.left : parent.right
                                text: id
                                font.pointSize: 8
                                color: selected ? "white" : "#707070"
                                height: 16
                                maximumLineCount: 1
                                elide: Text.ElideRight
                            }

                            Label {
                                id: newMessageCountField
                                y: 18
                                anchors.right: rightPaddingMsgCount.left
                                padding: 3
                                text: unread_cnt
                                font.pointSize: 8
                                color: selected ? "#707070" : "white"
                                height: 20
                                width: 22
                                horizontalAlignment: Qt.AlignCenter
                                background: Rectangle {
                                    radius: 10
                                    color: selected ? "white" : "Gray" //selecteditembkcolor
                                }
                                visible: unread_cnt > 0
                            }

                            Rectangle {
                                id: rightPaddingMsgCount
                                y: 18
                                anchors.right: parent.right
                                width: 3
                                height: 20
                                color: selected ? selecteditembkcolor : orderBy ? rightbkcolor : normalitembkcolor
                                //radius: 3
                            }

                            MouseArea {
                                anchors.fill: parent
                                acceptedButtons: Qt.LeftButton
                                onClicked: {
                                    current_id = id;
                                    message_model.set_current_contact(id, name);
                                    contacts.currentIndex = -1;
                                }
                            }

                            MouseArea {
                                anchors.fill: parent
                                acceptedButtons: Qt.RightButton
                                onClicked: {
                                    current_id = id;
                                    message_model.set_current_contact(id, name);
                                    contacts.currentIndex = -1;
                                    contextContactMenu.popup();
                                }
                            }

                            Menu {
                                id: contextContactMenu
                                width: 120
                                padding: 4
                                MenuItem {
                                    text: "Copy name"
                                    onClicked: {
                                        cliphelper.text = name
                                        cliphelper.selectAll()
                                        cliphelper.copy()
                                    }
                                }
                                MenuItem {
                                    text: "Copy address"
                                    onClicked: {
                                        cliphelper.text = id
                                        cliphelper.selectAll()
                                        cliphelper.copy()
                                    }
                                }
                                background: Rectangle {
                                    implicitWidth: 200
                                    border.color: "#e3e3e3"
                                }
                            }
                        }
                    }
                    ScrollBar.vertical: ScrollBar {}
                }

                //ToolBar {
                Rectangle {
                    id: header
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.right: parent.right
                    //contentHeight: filterContacts.implicitHeight + 2
                    height: filterContacts.implicitHeight + 2

                    TextArea {
                        id: filterContacts
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        anchors.right: parent.right
                        placeholderText: qsTr("Filter contacts")
                        selectByMouse: true
                        selectedTextColor: "white"
                        selectionColor: selecteditembkcolor
                        persistentSelection: true
                        onTextChanged: {
                            contacts.currentIndex = -1;
                            current_id = "";
                            message_model.current_id = "";
                            contact_model.setFilterString(text);
                        }
                        MouseArea {
                            anchors.fill: parent
                            acceptedButtons: Qt.RightButton
                            onClicked: {
                                contextfilterContactsMenu.popup()
                            }
                        }
                        Menu {
                            id: contextfilterContactsMenu
                            width: 120
                            padding: 4
                            MenuItem {
                                text: "Copy"
                                onClicked: { filterContacts.copy() }
                            }
                            MenuItem {
                                text: "Paste"
                                onClicked: { filterContacts.paste() }
                            }
                            background: Rectangle {
                                implicitWidth: 200
                                border.color: "#e3e3e3"
                            }
                        }
                    }
                }
            }
        }
        Item {
            Rectangle {
                id: message_rectangle
                anchors.fill: parent
                border.width: 0

                Rectangle {
                    anchors.top: messagesToolbar.bottom
                    anchors.left: parent.left
                    anchors.bottom: prePane.top
                    anchors.right: parent.right
                    color: rightbkcolor
                }

                ListView {
                    id: messages
                    anchors.top: messagesToolbar.bottom
                    anchors.left: parent.left
                    anchors.bottom: prePane.top 
                    anchors.right: parent.right
                    topMargin: 2
                    bottomMargin: 2
                    spacing: 4
                    model: message_model
                    delegate: Row {
                        readonly property bool sentByMe: from == contact_model.self_id()
                        readonly property bool sentByCurrent: from == message_model.current_id

                        anchors.right: sentByMe ? parent.right : undefined

                        Rectangle {
                            width: 12
                            height: 25
                            color: rightbkcolor
                        }

                        Rectangle {
                            id: avatar
                            radius: 6
                            width: 24
                            height: 24
                            color: short_background
                            visible: !(sentByMe || sentByCurrent)
                            Label {
                                anchors.centerIn: parent
                                text: short_from
                                color: short_color
                            }
                        }

                        Rectangle {
                            width: 2
                            height: 25
                            color: rightbkcolor
                        }

                        Rectangle {
                            property string text
                            readonly property int bodyTop: 12
                            readonly property int widthCorrect: sentByCurrent ? 25 : 0
                            radius: 5
                            height: bodyTop + (text_field.paintedHeight < 25 ? 25 : text_field.paintedHeight + 4)
                            width: text_field.width < 170 ? 180 : text_field.width + 10 + (sentByMe && sended ? 16 : 0)
                            color: sentByMe ? "white" : "#e3e3e3"
                            text: body
                            Label {
                                leftPadding: 3
                                y: 0
                                anchors.left:  parent.left
                                anchors.right: labelMsgTimeField.left
                                text: sentByMe ? "=" : name
                                color: "#909090"
                                font.bold: true
                                font.pointSize: 10
                                height: parent.bodyTop
                                maximumLineCount: 1
                                elide: Text.ElideRight
                            }

                            Label {
                                rightPadding: 3
                                id: labelMsgTimeField
                                y: 0
                                height: parent.bodyTop
                                anchors.right: parent.right
                                text: tm
                                font.pointSize: 10
                                color: "#909090"
                            }

                            TextEdit {
                                id: text_field
                                y: parent.bodyTop
                                anchors.left: parent.left
                                height: parent.height
                                width: dummy_text.paintedWidth > messages.width - 26 - (sentByMe && sended ? 16 : 0)
                                       ? messages.width - 26 - (sentByMe && sended ? 16 : 0)
                                       : dummy_text.paintedWidth < 30 ? 30 : dummy_text.paintedWidth
                                readOnly: true
                                text: parent.text
                                wrapMode: Text.WordWrap
                                selectByMouse: true
                                selectedTextColor: "white"
                                selectionColor: selecteditembkcolor
                                leftPadding: 4
                                rightPadding: 4
                                topPadding: 2
                                bottomPadding: 2

                                MouseArea {
                                    anchors.fill: parent
                                    acceptedButtons: Qt.RightButton
                                    onClicked: {
                                        contextMessageMenu.popup()
                                    }
                                }

                                Menu {
                                    id: contextMessageMenu
                                    width: 120
                                    padding: 4
                                    MenuItem {
                                        text: "Copy"
                                        onClicked: { text_field.copy() }
                                    }
                                    background: Rectangle {
                                        implicitWidth: 200
                                        border.color: "#e3e3e3"
                                    }
                                }
                            }

                            Text {
                                id: dummy_text
                                text: parent.text
                                leftPadding: 4
                                rightPadding: 4
                                topPadding: 2
                                bottomPadding: 2
                                visible: false
                            }
                            Rectangle {
                                id: sended_readed
                                anchors.bottom: parent.bottom
                                anchors.right: parent.right
                                height: 16
                                width: (sentByMe && sended) ? 16 : 0
                                color: "transparent"
                                Image {
                                    anchors.fill: parent
                                    source: "images/checkg.png"
                                    visible: sentByMe && sended
                                }
                            }
                        }

                        Rectangle {
                            width: 3
                            height: 25
                            color: rightbkcolor
                        }
                    }

                    ScrollBar.vertical: ScrollBar {}

                    onCountChanged: {
                        currentIndex = count-1
                    }

                    Component.onCompleted: {
                        window.title = contact_model.self_name()
                    }
                }

                //ToolBar {
                Rectangle {
                    id: messagesToolbar
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.right: parent.right
                    //contentHeight: filterMessages.implicitHeight + 2
                    height: filterContacts.implicitHeight + 5

                    TextArea {
                        id: filterMessages
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        anchors.right: parent.right
                        placeholderText: qsTr("Find message")
                        selectByMouse: true
                        selectedTextColor: "white"
                        selectionColor: selecteditembkcolor
                        persistentSelection: true
                        onTextChanged: {
                            message_model.setFilterString(text);
                        }
                        MouseArea {
                            anchors.fill: parent
                            acceptedButtons: Qt.RightButton
                            onClicked: {
                                contextfilterMessagesMenu.popup()
                            }
                        }
                        Menu {
                            id: contextfilterMessagesMenu
                            width: 120
                            padding: 4
                            MenuItem {
                                text: "Copy"
                                onClicked: { filterMessages.copy() }
                            }
                            MenuItem {
                                text: "Paste"
                                onClicked: { filterMessages.paste() }
                            }
                            background: Rectangle {
                                implicitWidth: 200
                                border.color: "#e3e3e3"
                            }
                        }
                    }
                }

                Rectangle {
                    id: prePane
                    anchors.left: parent.left
                    anchors.right: parent.right
                    height: 3
                    anchors.bottom: prePane2.top
                    color: rightbkcolor
                }

                Rectangle {
                    id: prePane2
                    anchors.left: parent.left
                    anchors.right: parent.right
                    height: 2
                    anchors.bottom: pane.top
                    color: "white"
                }

                Pane {
                    id: pane
                    padding: 5
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    background: Rectangle {
                        color: rightbkcolor
                    }

                    RowLayout {
                        width: parent.width

                        TextArea {
                            id: messageField
                            Layout.fillWidth: true
                            placeholderText: qsTr("Compose message")
                            wrapMode: TextArea.Wrap
                            selectByMouse: true
                            selectedTextColor: "white"
                            selectionColor: selecteditembkcolor
                            persistentSelection: true
                            inputMethodHints: Qt.ImhNoPredictiveText
                            enabled: current_id.length > 0
                            property bool ctrlPressed: false
                            Keys.onPressed: {
                                if(event.key === Qt.Key_Control) ctrlPressed = true;
                                if(event.key === Qt.Key_Return) {
                                    if(ctrlPressed)
                                        messageField.append("")
                                    else if(messageField.length > 2)
                                        message_model.append(messageField.text.substring(0, messageField.text.length))
                                    event.accepted = true;
                                }
                            }
                            Keys.onReleased: {
                                if(event.key === Qt.Key_Return && !ctrlPressed) messageField.text = ""
                                if(event.key === Qt.Key_Control) ctrlPressed = false;
                            }
                            MouseArea {
                                anchors.fill: parent
                                acceptedButtons: Qt.RightButton
                                onClicked: {
                                    contextPaneMenu.popup()
                                }
                            }

                            Menu {
                                id: contextPaneMenu
                                width: 120
                                padding: 4
                                MenuItem {
                                    text: "Copy"
                                    onClicked: { messageField.copy() }
                                }
                                MenuItem {
                                    text: "Paste"
                                    onClicked: { messageField.paste() }
                                }
                                 background: Rectangle {
                                    implicitWidth: 200
                                    border.color: "#e3e3e3"
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    Row {
        id: splitView
        anchors.fill: parent
        Item {
            id: contactsContainer
            width: splitter.x
            height: parent.height
        }
        Item {
            id: messageContainer
            width: parent.width - splitter.x
            height: parent.height
        }
    }

    Rectangle {
        id: splitter
        y: 0
        x: responsiveWidth / 2
        height: parent.height
        width: 12
        color: "transparent"
        enabled: false
        MouseArea {
            id: dragArea
            anchors.fill: parent
            cursorShape: Qt.SizeHorCursor
            drag.target: parent
            drag.axis: Drag.XAxis
            drag.minimumX: minContactWidth
            drag.maximumX: maxContactWidth
        }
    }
}
