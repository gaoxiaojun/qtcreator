import QtQuick 1.1
import "../components/" as Components

Item {
    id: root

    property int currentItem: 0
    property alias model: repeater.model
    property alias itemCount: repeater.count

    Timer {
        id: timer
        repeat: true
        interval: 30*1000
        onTriggered: repeater.incrementIndex()
    }

    Timer {
        id: modelUpdateTimer
        repeat: false
        interval: 1000
        onTriggered: repeater.handleModelUpdate();
    }

    Repeater {
        id: repeater
        function incrementIndex() {
            repeater.itemAt(currentItem).active = false
            currentItem = (currentItem+1) % repeater.count
            repeater.itemAt(currentItem).active = true
        }

        function handleModelUpdate() {
            timer.stop();
            currentItem = 0;
            for (var i = 0; i < count; ++i) {
                if (i != currentItem)
                    repeater.itemAt(i).active = false;
                else
                    repeater.itemAt(i).active = true;
            }
            timer.start();
        }

        function handleModelChanged() {
            modelUpdateTimer.restart();
        }

        function handleItemRemoved(index, item) {
            modelUpdateTimer.restart();
        }

        function handleItemAdded(index, item) {
            modelUpdateTimer.restart();
        }

        anchors.fill: parent
        onModelChanged: handleModelChanged()
        onItemAdded: handleItemAdded(index, item)
        onItemRemoved: handleItemRemoved(index, item)
        delegate: Item {
            property bool active: false
            id: delegateItem
            opacity: 0
            height: root.height
            width: 270
            Column {
                spacing: 10
                width: parent.width
                id: column
                Text {
                    id: heading1;
                    text: title;
                    font.bold: true;
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere;
                    textFormat: Text.RichText;
                    width: parent.width-icon.width-5
                }
                Row {
                    spacing: 5
                    width: parent.width
                    Image {
                        id: icon;
                        source: blogIcon;
                        asynchronous: true
                    }
                    Text {
                        id: heading2;
                        text: blogName;
                        font.italic: true;
                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere;
                        textFormat: Text.RichText;
                        width: parent.width-icon.width-5
                    }
                }
                Text {
                    id: text;
                    text: description;
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    textFormat: Text.RichText
                    width: parent.width-10
                }
                Text { visible: link !== "";
                    id: readmore;
                    text: qsTr("Click to read more...");
                    font.italic: true;
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere;
                    textFormat: Text.RichText
                }
            }
            Components.QStyleItem {
                id: styleItem;
                cursor: "pointinghandcursor";
                anchors.fill: column
            }
            MouseArea {
                anchors.fill: column;
                onClicked: Qt.openUrlExternally(link);
                hoverEnabled: true;
                id: mouseArea
            }

            StateGroup {
                id: activeState
                states: [ State { name: "active"; when: delegateItem.active; PropertyChanges { target: delegateItem; opacity: 1 } } ]
                transitions: [
                    Transition { from: ""; to: "active"; reversible: true; NumberAnimation { target: delegateItem; property: "opacity"; duration: 1000 } }
                ]
            }

            states: [
                State { name: "clicked"; when: mouseArea.pressed;  PropertyChanges { target: text; color: "black" } },
                State { name: "hovered"; when: mouseArea.containsMouse;  PropertyChanges { target: text; color: "#074C1C" } }
            ]


        }
    }
}
