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

    //
    // final constructon stasis
    //
    Component.onCompleted:
    {
        // need to pass secret
        gravioAccount.open("");
    }

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
            stackView.deactivateModule();
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
                    stackView.deactivateModule();
                }

                Image
                {
                    id: backButtonImage

                    x: parent.width + 4.0
                    y: (parent.height - height) / 2.0
                    source: stackView.getPrevModuleName() == "(empty)" ? "images/logo.png" : modulesModel.get(stackView.getPrevModuleName()).iconTitleFile
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
            backButtonImage.source = (stackView.getPrevModuleName() == "(empty)" ? "images/logo.png" : modulesModel.get(stackView.getPrevModuleName()).iconTitleFile);
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
                text: stackView.depth > 1 && listView.currentItem ? modulesModel.get(listView.currentIndex).caption : " "
                leftPadding: titleLogo.width + 4.0
                font.pixelSize: 20
                elide: Label.ElideRight
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                anchors.centerIn: parent

                Image
                {
                    id: titleLogo
                    height: getHeight()
                    horizontalAlignment: Qt.AlignHCenter
                    verticalAlignment: Qt.AlignVCenter
                    fillMode: Image.PreserveAspectFit
                    source: stackView.depth > 1 ? (listView.currentItem ? modulesModel.get(listView.currentIndex).iconTitleFile : "") : "images/logo.png"

                    function getHeight()
                    {
                        if (stackView.depth > 1) return 32;
                        return toolBar.height;
                    }

                    onSourceChanged:
                    {
                        if (stackView.depth > 1)
                        {
                            if (listView.currentItem) y = modulesModel.get(listView.currentIndex).iconTitleTopPadding;
                            else y = -4.0;
                        }
                        else y = -10.0;
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
                    if (!(stackView.depth > 1))
                    {
                        //
                        // create dialog
                        //

                        var lComponent = Qt.createComponent("account.qml");
                        var lDialog = lComponent.createObject(window);
                        lDialog.openAccountDialog();
                    }
                    else
                    {
                        toolBar.actionActivated();
                    }
                }
            }
        }

        signal actionActivated();

        function adjust()
        {
            actionTopButton.visible = !(stackView.depth > 1);
            actionTopImage.source = "images/black/configure.png";
        }

        function setActionImage(imageSource)
        {
            actionTopImage.source = imageSource;
            actionTopButton.visible = true;
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
            anchors.margins: 10

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
                    stackView.activateModule(index, name, source);
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
        anchors.margins: 10

        property var list: []

        function pushItem(name, module)
        {
            console.log("[StackView/pushItem]: depth(0) = " + stackView.depth);

            var lModule = this.find
            (
                function(item, index)
                {
                    return item.moduleName == name;
                },
                StackView.DontLoad // ForceLoad
            );

            console.log("[StackView/pushItem]: depth(1) = " + stackView.depth + ", module = " + lModule);

            if (lModule == null)
            {
                lModule = push(module);
                list.push(name);
            }
            else
            {
                console.log("[StackView/pushItem/unwind]: depth(2) = " + stackView.depth + ", module = " + lModule);
                this.pop(lModule, StackView.Transition);
                list = [];

                // refill index
                for(var lIdx = 0; lIdx < this.depth; lIdx++)
                {
                    var lLoadedModule = this.get(lIdx, StackView.DontLoad);
                    if (lLoadedModule != null)
                    {
                        list.push(lLoadedModule.moduleName);
                    }
                }
            }

            console.log("[StackView/pushItem]: depth(3) = " + stackView.depth + ", module = " + lModule);
            return lModule;
        }

        function getPrevModuleName()
        {
            if (list.length > 1) { return list[list.length-2]; }
            return "(empty)";
        }

        function getModuleName()
        {
            if (list.length > 0) { return list[list.length-1]; }
            return "(empty)";
        }

        function popItem()
        {
            list.pop();
            this.pop();
        }

        function activateCurrentModulePart(source)
        {
            activateModule(-1, source.moduleName(), source);
        }

        function activateModule(index, name, source)
        {
            var lModule = null;

            // check
            if (stackView.currentItem.moduleName == name) return;

            // deactivate current module - unsubscribe from toolbar actions & etc.
            stackView.currentItem.moduleDeactivate();

            // move to the new index
            if (index != -1) listView.currentIndex = index;

            // try to load module (if already loaded - checked inside)
            modulesModel.load(name);

            // push new module
            lModule = stackView.pushItem(name, source); // push to start *.qml

            // reset tool&footer bars
            footerBar.visible = true;
            footerBar.adjust();
            toolBar.adjust();
            drawer.close();

            // activate new current module
            lModule.moduleActivate(modulesModel.get(stackView.getModuleName()) /*module*/, stackView /*host*/);
        }

        function deactivateModule()
        {
            if (stackView.depth > 1)
            {
                // _MUST_ be implemented in <module>.qml
                stackView.currentItem.moduleDeactivate();

                stackView.popItem();

                var lIdx = modulesModel.findIndex(stackView.getModuleName());
                if (lIdx != -1) listView.currentIndex = lIdx;
            }

            footerBar.visible = stackView.depth > 1;
            footerBar.adjust();
            toolBar.adjust();

            console.log("[StackView/popItem]: depth = " + stackView.depth + ", module = " + stackView.currentItem);

            // re-activate new current module
            stackView.currentItem.moduleActivate(modulesModel.get(stackView.getModuleName()), stackView);
        }

        initialItem: GridLayout
        {
            id: gridView
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
                        height: icon.height + metrics.height + 7.0 //TODO:
                        leftPadding: rect.width/2.0 - metrics.width/2.0 - captionOffset
                        topPadding: icon.height

                        Image
                        {
                            id: icon
                            width: 64
                            height: 64
                            x: rect.width/2.0 - icon.width/2.0 + iconLeftPadding
                            fillMode: Image.PreserveAspectFit
                            source: iconFile
                        }

                        onClicked:
                        {
                            stackView.activateModule(index, name, source);
                        }
                    }
                }
            }

            //
            // common module methods
            //
            function moduleDeactivate()
            {
            }

            function moduleActivate(moduleInfo, host)
            {
            }

            //
            // common module properties
            //
            property string moduleName;

            //
            // default page
            //
            Component.onCompleted:
            {
                moduleName = "(empty)";
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

    Dialog
    {
        id: errorDialog
        modal: true
        focus: true
        title: "Error"
        x: (window.width - width) / 2
        y: window.height / 6
        width: Math.min(window.width, window.height) / 3 * 2
        contentHeight: erorColumn.height
        standardButtons: Dialog.Ok

        function show(msg)
        {
            console.log(msg);
            errorDialog.errorText.text = msg;
            errorDialog.open();
        }

        Column
        {
            id: erorColumn
            spacing: 20

            Label
            {
                id: errorText
                width: errorDialog.availableWidth
                wrapMode: Label.Wrap
                font.pixelSize: 12
            }
        }
    }
}
