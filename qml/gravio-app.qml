import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.1
import QtQuick.Controls.Universal 2.1
import Qt.labs.settings 1.0

ApplicationWindow
{
    id: window
    width: 360
    height: 520
    visible: true
    title: "gravio-app v0.1"

    Settings
    {
        id: settings
        property string style: "Material"
    }

    Shortcut
    {
        sequences: ["Esc", "Back"]
        enabled: stackView.depth > 1

        onActivated:
        {
            if (stackView.depth > 1)
            {
                stackView.popItem();
                listView.currentIndex = stackView.getIndex();
            }

            footerBar.visible = stackView.depth > 1;
            footerBar.adjust();
            toolBar.adjust();
        }
    }

    Shortcut
    {
        sequence: "Menu"
        onActivated: optionsMenu.open()
    }

    footer: ToolBar
    {
        id: footerBar
        Material.foreground: "black"
        Material.background: "#FFFFFF"
        visible: false

        RowLayout
        {
            anchors.fill: parent

            ToolButton
            {
                id: backButton

                contentItem: Image
                {
                    fillMode: Image.Pad
                    horizontalAlignment: Image.AlignHCenter
                    verticalAlignment: Image.AlignVCenter
                    source: "images/black/back.png"
                }

                onClicked:
                {
                    if (stackView.depth > 1)
                    {
                        stackView.popItem();
                        listView.currentIndex = stackView.getIndex();
                    }

                    footerBar.visible = stackView.depth > 1;
                    footerBar.adjust();
                    toolBar.adjust();
                }

                Image
                {
                    id: backButtonImage

                    x: parent.width + 4.0
                    y: (parent.height - height) / 2.0
                    source: stackView.getPrevIndex() == -1 ? "images/logo.png" : modulesModel.get(stackView.getPrevIndex()).iconTitleFile
                }
            }

            ToolButton
            {
                id: actionBottomButton

                Layout.alignment: Qt.AlignRight
                contentItem: Image
                {
                    id: actionBottomImage

                    fillMode: Image.Pad
                    horizontalAlignment: Image.AlignHCenter
                    verticalAlignment: Image.AlignVCenter
                    source: "images/black/menu.png"
                }

                onClicked:
                {
                    footerBar.actionActivated();
                }
            }
        }

        signal actionActivated();

        function adjust()
        {
            backButtonImage.source = (stackView.getPrevIndex() == -1 ? "images/logo.png" : modulesModel.get(stackView.getPrevIndex()).iconTitleFile);
            actionBottomButton.visible = false;
        }

        function setActionImage(imageSource)
        {
            actionBottomImage.source = imageSource;
            actionBottomButton.visible = true;
        }
    }

    header: ToolBar
    {
        id: toolBar
        Material.foreground: "black"
        Material.background: "#FFFFFF"

        RowLayout
        {
            anchors.fill: parent

            ToolButton
            {
                contentItem: Image
                {
                    fillMode: Image.Pad
                    horizontalAlignment: Image.AlignHCenter
                    verticalAlignment: Image.AlignVCenter
                    source: "images/black/drawer.png"
                }

                onClicked:
                {
                    drawer.open()
                }
            }

            Label
            {
                id: titleLabel
                text: listView.currentItem ? modulesModel.get(listView.currentIndex).caption : " "
                leftPadding: titleLogo.width + 4.0
                font.pixelSize: 20
                elide: Label.ElideRight
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                anchors.centerIn: parent

                Image
                {
                    id: titleLogo
                    horizontalAlignment: Qt.AlignHCenter
                    verticalAlignment: Qt.AlignVCenter
                    fillMode: Image.PreserveAspectFit
                    source: stackView.depth > 1 ? (listView.currentItem ? modulesModel.get(listView.currentIndex).iconTitleFile : "") : "images/logo.png"

                    onSourceChanged:
                    {
                        if (stackView.depth > 1) y = -4.0;
                        else y = -6.0;
                    }
                }
            }

            ToolButton
            {
                id: actionTopButton
                Layout.alignment: Qt.AlignRight
                contentItem: Image
                {
                    id: actionTopImage

                    fillMode: Image.Pad
                    horizontalAlignment: Image.AlignHCenter
                    verticalAlignment: Image.AlignVCenter
                    source: "images/black/configure.png"
                }

                onClicked:
                {
                    toolBar.actionActivated();
                }
            }
        }

        signal actionActivated();

        function setActionImage(imageSource)
        {
            actionTopImage.source = imageSource;
            actionTopButton.visible = true;
        }

        function adjust()
        {
            actionTopButton.visible = !(stackView.depth > 1);
        }
    }

    Drawer
    {
        id: drawer
        width: Math.min(window.width, window.height) / 3 * 2
        height: window.height
        interactive: stackView.depth === 1

        ListView
        {
            id: listView

            focus: true
            currentIndex: -1
            anchors.fill: parent

            delegate: ItemDelegate
            {
                width: parent.width
                text: caption
                highlighted: ListView.isCurrentItem
                height: listIcon.height + 5.0
                leftPadding: listIcon.width + 5.0

                Image
                {
                    id: listIcon
                    y: 1
                    width: 32
                    height: 32
                    fillMode: Image.PreserveAspectFit
                    source: iconFile
                }

                onClicked:
                {
                    listView.currentIndex = index;
                    stackView.pushItem(index, source);
                    footerBar.visible = true;
                    footerBar.adjust();
                    toolBar.adjust();
                    drawer.close();
                }
            }

            model: modulesModel

            ScrollIndicator.vertical: ScrollIndicator { }
        }
    }

    StackView
    {
        id: stackView
        anchors.fill: parent

        property var list: []

        function pushItem(index, item)
        {
            list.push(index);
            push(item);
        }

        function getPrevIndex()
        {
            if (list.length > 1) { console.info(list[list.length-2]); return list[list.length-2]; }
            return -1;
        }

        function getIndex()
        {
            if (list.length > 0) { console.info(list[list.length-1]); return list[list.length-1]; }
            return -1;
        }

        function popItem()
        {
            list.pop();
            pop();
        }

        initialItem: GridLayout
        {
            id: grid
            columns: 3

            Repeater
            {
                model: modulesModel
                delegate: Rectangle
                {
                    id: rect
                    color: "transparent"
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.alignment: Qt.AlignTop

                    TextMetrics
                    {
                        id: metrics
                        text: caption
                    }

                    ItemDelegate
                    {
                        text: caption
                        width: rect.width
                        height: icon.height + metrics.height + 7.0
                        leftPadding: rect.width/2.0 - metrics.width/2.0 - captionOffset
                        topPadding: icon.height + 3.0

                        Image
                        {
                            id: icon
                            width: 64
                            height: 64
                            x: rect.width/2.0 - icon.width/2.0
                            fillMode: Image.PreserveAspectFit
                            source: iconFile
                        }

                        onClicked:
                        {
                            aboutDialog.title = index.toString()
                            aboutDialog.open()
                        }
                    }
                }
            }
        }
    }

    Dialog {
        id: settingsDialog
        x: Math.round((window.width - width) / 2)
        y: Math.round(window.height / 6)
        width: Math.round(Math.min(window.width, window.height) / 3 * 2)
        modal: true
        focus: true
        title: "Settings"

        standardButtons: Dialog.Ok | Dialog.Cancel
        onAccepted: {
            settings.style = styleBox.displayText
            settingsDialog.close()
        }
        onRejected: {
            styleBox.currentIndex = styleBox.styleIndex
            settingsDialog.close()
        }

        contentItem: ColumnLayout {
            id: settingsColumn
            spacing: 20

            RowLayout {
                spacing: 10

                Label {
                    text: "Style:"
                }

                ComboBox {
                    id: styleBox
                    property int styleIndex: -1
                    model: availableStyles
                    Component.onCompleted: {
                        styleIndex = find(settings.style, Qt.MatchFixedString)
                        if (styleIndex !== -1)
                            currentIndex = styleIndex
                    }
                    Layout.fillWidth: true
                }
            }

            Label {
                text: "Restart required"
                color: "#e41e25"
                opacity: styleBox.currentIndex !== styleBox.styleIndex ? 1.0 : 0.0
                horizontalAlignment: Label.AlignHCenter
                verticalAlignment: Label.AlignVCenter
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
        }
    }

    Dialog {
        id: aboutDialog
        modal: true
        focus: true
        title: "About"
        x: (window.width - width) / 2
        y: window.height / 6
        width: Math.min(window.width, window.height) / 3 * 2
        contentHeight: aboutColumn.height

        Column {
            id: aboutColumn
            spacing: 20

            Label {
                width: aboutDialog.availableWidth
                text: "The Qt Quick Controls 2 module delivers the next generation user interface controls based on Qt Quick."
                wrapMode: Label.Wrap
                font.pixelSize: 12
            }

            Label {
                width: aboutDialog.availableWidth
                text: "In comparison to the desktop-oriented Qt Quick Controls 1, Qt Quick Controls 2 "
                    + "are an order of magnitude simpler, lighter and faster, and are primarily targeted "
                    + "towards embedded and mobile platforms."
                wrapMode: Label.Wrap
                font.pixelSize: 12
            }
        }
    }
}
