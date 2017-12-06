import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.1
import QtQuick.Controls.Universal 2.1
import QtGraphicalEffects 1.0
import Qt.labs.folderlistmodel 2.1
import QtMultimedia 5.8

import net.gravio.wave.modules.contacts 1.0

Page
{
    id: page

    Rectangle
    {
        id: searchBar
        Layout.alignment: Qt.AlignTop
        height: actionClearFilterButton.height

        RowLayout
        {
            id: searchRow
            anchors.fill: parent
            spacing: 1

            Rectangle
            {
                id: searchFieldContainer
                color: "transparent"

                height: actionClearFilterButton.height
                width: page.width - actionClearFilterButton.width - actionApplyFilterButton.width
                Layout.leftMargin: 10

                TextField
                {
                    id: searchField

                    placeholderText: "Enter search text"
                    width: searchFieldContainer.width

                    onEditingFinished:
                    {
                        contactsModel.applyFilter(searchField.text);
                    }
                }

                ToolButton
                {
                    id: actionApplyFilterButton
                    x: getX()

                    function getX()
                    {
                        var lX = searchFieldContainer.width + searchRow.spacing + searchFieldContainer.Layout.leftMargin;
                        return lX;
                    }

                    contentItem: Image
                    {
                        id: actionApplyFilterImage

                        fillMode: Image.Pad
                        horizontalAlignment: Image.AlignHCenter
                        verticalAlignment: Image.AlignVCenter
                        source: "../../images/black/apply.png"
                    }

                    onClicked:
                    {
                        contactsModel.applyFilter(searchField.text);
                    }
                }
                ToolButton
                {
                    id: actionClearFilterButton
                    x: getX()

                    function getX()
                    {
                        var lX = actionApplyFilterButton.x + actionApplyFilterButton.width - 10//searchFieldContainer.width + searchRow.spacing + searchFieldContainer.Layout.leftMargin + actionApplyFilterButton.width;
                        return lX;
                    }

                    contentItem: Image
                    {
                        id: actionClearSearchImage

                        fillMode: Image.Pad
                        horizontalAlignment: Image.AlignHCenter
                        verticalAlignment: Image.AlignVCenter
                        source: "../../images/black/remove.png"
                    }

                    onClicked:
                    {
                        searchField.text = "";
                        contactsModel.resetFilter();
                    }
                }

            }
        }

        function getHeight()
        {
            return actionClearFilterButton.height;
        }

        function getWidth()
        {
            return page.width;
        }
    }

    Rectangle
    {
        id: listBar
        y: searchBar.getHeight()
        width: searchBar.getWidth()
        height: page.height - searchBar.getHeight()
        color: "transparent"

        ListView
        {
            id: contactsView
            model: contactsModel

            focus: true
            currentIndex: -1
            anchors.fill: listBar

            delegate: SwipeDelegate
            {
                id: delegateContact
                clip: true
                width: parent.width

                highlighted: ListView.isCurrentItem
                height: avatarImage.height + 10.0

                contentItem: Row
                {
                    Rectangle
                    {
                        x: -10
                        y: -3
                        Image
                        {
                            id: avatarImage
                            property bool rounded: true
                            property bool adapt: true

                            //mipmap: true // smoothing

                            width: 48
                            height: 48
                            fillMode: Image.PreserveAspectCrop

                            source: avatarSource

                            layer.enabled: rounded
                            layer.effect: OpacityMask
                            {
                                maskSource: Item
                                {
                                    width: avatarImage.width
                                    height: avatarImage.height
                                    Rectangle
                                    {
                                        anchors.centerIn: parent
                                        width: avatarImage.adapt ? avatarImage.width : Math.min(avatarImage.width, avatarImage.height)
                                        height: avatarImage.adapt ? avatarImage.height : width
                                        radius: Math.min(width, height)
                                    }
                                }
                            }
                        }
                    }
                    Label
                    {
                        leftPadding: avatarImage.width
                        text: name + (fullName == "" ? "" : " (" + fullName + ")")
                        width: addressHeader.width
                        height: 20
                    }
                    Rectangle
                    {
                        id: addressPrimaryTypeInfo
                        color: "yellow"
                        Image
                        {
                            id: currencyPrimaryIcon
                            x: avatarImage.width + 3
                            y: 22
                            width: 18
                            height: 18
                            mipmap: true
                            fillMode: Image.PreserveAspectFit
                            source: "../../images/black/" + primaryAddressType + ".png"
                        }
                        Label
                        {
                            x: avatarImage.width + currencyPrimaryIcon.width - 12
                            y: 22
                            color: "grey"
                            leftPadding: currencyPrimaryIcon.width
                            text: primaryAddress
                            width: addressHeader.width - avatarImage.width + currencyPrimaryIcon.width
                            height: 20
                        }

                        Component.onCompleted:
                        {
                        }
                    }
                }

                onClicked:
                {
                    contactsView.currentIndex = index;
                    contactDialog.editContact(contactsModel.get(index));
                }

                swipe.right: Rectangle
                {
                    id: swipeContactRect
                    width: parent.height*2
                    anchors.right: parent.right
                    height: parent.height

                    Rectangle
                    {
                        id: removeContactRect
                        color: swipeContactRect.SwipeDelegate.pressed ? Qt.darker("tomato", 1.1) : "tomato"
                        height: swipeContactRect.height
                        anchors.left: swipeContactRect.left
                        width: parent.height

                        Image
                        {
                            id: removeContactImage
                            x: removeContactRect.width / 2 - removeContactImage.width / 2
                            y: removeContactRect.height / 2 - removeContactImage.height / 2
                            source: "../../images/white/remove.png"
                        }

                        MouseArea
                        {
                            anchors.fill: removeContactRect
                            onClicked:
                            {
                                contactsModel.removeContact(index);
                            }
                        }
                    }
                    Rectangle
                    {
                        id: cancelContactRect
                        color: swipeContactRect.SwipeDelegate.pressed ? Qt.darker("orange", 1.1) : "orange"
                        height: swipeContactRect.height
                        anchors.right: swipeContactRect.right
                        width: parent.height

                        Image
                        {
                            id: cancelContactImage
                            x: cancelContactRect.width / 2 - cancelContactImage.width / 2
                            y: cancelContactRect.height / 2 - cancelContactImage.height / 2
                            source: "../../images/white/back_right.png"
                        }

                        MouseArea
                        {
                            anchors.fill: cancelContactRect
                            onClicked:
                            {
                                delegateContact.swipe.close();
                            }
                        }
                    }
                }
            }

            Component.onCompleted:
            {
            }

            ScrollIndicator.vertical: ScrollIndicator {}
        }
    }

    //
    // common module methods
    //
    function moduleDeactivate()
    {
        console.log("[Contacts/moduleDeactivate]: fired");
        toolBar.actionActivated.disconnect(addContact);
    }

    function moduleActivate(moduleInfo)
    {
        console.log("[Contacts/moduleActivate]: fired");

        moduleName = moduleInfo.name;
        module = moduleInfo;

        toolBar.actionActivated.connect(addContact);
        toolBar.setActionImage(moduleInfo.assetPrefix + moduleInfo.modulePath + "add.png");
    }

    //
    // common module properties
    //
    property int moduleIndex;
    property string moduleName;
    property variant module;

    //
    // private methods
    //
    function addContact()
    {
        console.log("[Contacts/addContact]: fired");
        contactDialog.addContact(contactsModel.newContact());
    }

    //
    // AddAddress/ModifyAddress dialog
    //
    Dialog
    {
        id: addressDialog

        modal: true
        focus: true
        title: "Add address"

        x: 5
        y: 10
        width: window.width - 30
        contentHeight: addressColumn.height

        property ContactAddress contactAddress;
        property ContactAddresses addresses;

        standardButtons: Dialog.Ok | Dialog.Cancel
        onAccepted:
        {
            var lError = "";
            if (addressDialog.contactAddress == null /*add new address*/)
            {
                lError = addressDialog.addresses.addAddress(addressTypeCombo.currentText, addressEdit.text, labelEdit.text, primaryEdit.checked);
            }
            else
            {
                addressDialog.contactAddress.addressTypeStr = addressTypeCombo.currentText;
                addressDialog.contactAddress.address = addressEdit.text;
                addressDialog.contactAddress.label = labelEdit.text;
                addressDialog.contactAddress.primary = primaryEdit.checked;

                addressDialog.addresses.updateModel();
            }

            if (lError != "") errorDialog.show(lError);

            addressDialog.close();
        }
        onRejected:
        {
            addressDialog.close();
        }

        function newAddress(addresses)
        {
            addressDialog.title = "Add address";
            addressDialog.contactAddress = null;
            addressDialog.addresses = addresses;
            addressDialog.open();
        }

        function editAddress(contactAddress, addresses)
        {
            addressDialog.title = "Edit address";
            addressDialog.contactAddress = contactAddress;
            addressDialog.addresses = addresses;
            addressDialog.open();
        }

        Column
        {
            id: addressColumn
            spacing: 10

            Row
            {
                ComboBox
                {
                    id: addressTypeCombo
                    currentIndex: getIndex()
                    font.pixelSize: 14

                    // TODO: make model from gravio::wave::Currency
                    model: ListModel
                    {
                        id: addressTypeModel
                        ListElement
                        {
                            text: "GIO"
                        }
                        ListElement
                        {
                            text: "BTC"
                        }
                        ListElement
                        {
                            text: "LTC"
                        }
                        ListElement
                        {
                            text: "DOGE"
                        }
                    }

                    function getIndex()
                    {
                        if(addressDialog.contactAddress == null) return -1;

                        switch(addressDialog.contactAddress.addressTypeStr)
                        {
                            case "GIO": return 0;
                            case "BTC": return 1;
                            case "LTC": return 2;
                            case "DOGE": return 3;
                        }

                        return -1;
                    }
                }
            }
            Row
            {
                TextField
                {
                    id: addressEdit
                    text: (addressDialog.contactAddress != null) ? addressDialog.contactAddress.address : ""
                    placeholderText: "Enter address"
                    width: addressDialog.width - 50
                    font.pixelSize: 14
                }
            }
            Row
            {
                TextField
                {
                    id: labelEdit
                    text: (addressDialog.contactAddress != null) ? addressDialog.contactAddress.label : ""
                    placeholderText: "Enter label"
                    width: addressDialog.width - 50
                    font.pixelSize: 14
                }
            }
            Row
            {
                CheckBox
                {
                    id: primaryEdit
                    text: "Primary"
                    checked: (addressDialog.contactAddress != null) ? addressDialog.contactAddress.primary : false
                    font.pixelSize: 14
                }
            }
        }
    }

    //
    // AddContact/ModifyContact dialog
    //
    Dialog
    {
        id: contactDialog
        modal: true
        focus: true
        title: "Add contact"
        x: 5
        y: 10
        width: window.width - 30
        height: window.height - 100

        property Contact contact;

        standardButtons: Dialog.Ok | Dialog.Cancel
        onAccepted:
        {
            contact.name = nameEdit.text;
            contact.fullName = fullNameEdit.text;

            var lError = contact.update();
            if (lError != "") errorDialog.show(lError);

            contactDialog.close();
        }
        onRejected:
        {
            if (!contact.isNew())
            {
                contact.addresses.refetchModel();
                contact.revertChanges();
            }

            contactDialog.close();
        }

        function editContact(other)
        {
            contactDialog.title = "Edit contact";
            contactDialog.contact = other;

            contactAvatarImage.mipmap = false;
            contactAvatarImage.source = contactDialog.contact.avatarSource();
            addressesView.model = contactDialog.contact.addresses;
            contactSwipeView.currentIndex = 0;

            contactDialog.open();
        }

        function addContact(other)
        {
            contactDialog.title = "Add contact";
            contactDialog.contact = other;

            contactAvatarImage.mipmap = true;
            contactAvatarImage.source = "../../images/black/photo_frame2.png";
            addressesView.model = contactDialog.contact.addresses;
            contactSwipeView.currentIndex = 0;

            contactDialog.open();
        }

        Rectangle
        {
            id: contactDialogPage
            anchors.fill: parent
            clip: true
            color: "white"

            SwipeView
            {
                id: contactSwipeView

                anchors.fill: contactDialogPage
                currentIndex: 0

                Page
                {
                    Rectangle
                    {
                        color: "white"
                        anchors.fill: parent

                        Column
                        {
                            id: contactColumn
                            spacing: 10

                            Row
                            {
                                Rectangle
                                {
                                    id: avatarBar
                                    width: contactDialog.width - 40
                                    height: 140

                                    ToolButton
                                    {
                                        id: takePhotoButton
                                        x: avatarBar.width / 2 - contactAvatarImage.width / 2 - (takePhotoButton.width + 5)
                                        y: 8
                                        contentItem: Image
                                        {
                                            id: takePhotoButtonImage
                                            fillMode: Image.Pad
                                            horizontalAlignment: Image.AlignHCenter
                                            verticalAlignment: Image.AlignVCenter
                                            source: "../../images/black/camera.png"
                                        }

                                        onClicked:
                                        {
                                            if (camera.cameraState() != Camera.ActiveState)
                                            {
                                                camera.start();
                                                contactAvatarImage.mipmap = true;
                                                contactAvatarImage.source = "../../images/black/photo_frame2.png";
                                                cancelPhotoButton.visible = true;
                                            }
                                            else if (camera.cameraState() == Camera.ActiveState) // take a shot
                                            {
                                                cancelPhotoButton.visible = false;
                                                camera.captureToLocation(page.module.moduleDataPath);
                                            }
                                        }
                                    }
                                    ToolButton
                                    {
                                        id: cancelPhotoButton
                                        x: avatarBar.width / 2 - contactAvatarImage.width / 2 - (takePhotoButton.width + 5)
                                        y: avatarBar.height - 55
                                        contentItem: Image
                                        {
                                            id: cancelPhotoButtonImage
                                            fillMode: Image.Pad
                                            horizontalAlignment: Image.AlignHCenter
                                            verticalAlignment: Image.AlignVCenter
                                            source: "../../images/black/cancel.png"
                                        }

                                        onClicked:
                                        {
                                            cancelPhotoButton.visible = false;

                                            if (camera.cameraState() == Camera.ActiveState)
                                            {
                                                camera.stop();
                                                contactAvatarImage.mipmap = false;
                                                contactAvatarImage.source = contactDialog.contact.avatarSource();
                                            }
                                        }

                                        Component.onCompleted:
                                        {
                                            cancelPhotoButton.visible = false;
                                        }
                                    }

                                    Image
                                    {
                                        id: contactAvatarImage

                                        property bool rounded: true
                                        property bool adapt: true

                                        //mipmap: true // smoothing

                                        Rectangle
                                        {
                                            id: imageContainer
                                            x: (-1 * contactAvatarImage.width/2) //avatarBar.width / 2 - contactAvatarImage.width
                                            y: -80 //-80
                                            width: contactAvatarImage.width * 2
                                            height: avatarBar.height * 2
                                            color: "transparent"

                                            // camera proxy
                                            Item
                                            {
                                                id: camera

                                                property variant inner;

                                                function cameraState()
                                                {
                                                    if (inner !== undefined) return inner.cameraState();
                                                    return Camera.Unavailable;
                                                }
                                                function start()
                                                {
                                                    // TODO: create camera
                                                    if(inner === undefined)
                                                    {
                                                        var lComponent = Qt.createComponent("cameraview.qml");
                                                        inner = lComponent.createObject(imageContainer);
                                                        inner.setup(camera);
                                                    }

                                                    if (inner !== undefined)
                                                    {
                                                        inner.start();
                                                    }
                                                }
                                                function stop()
                                                {
                                                    if (inner !== undefined)
                                                    {
                                                        inner.stop();
                                                    }
                                                }
                                                function captureToLocation(location)
                                                {
                                                    if (inner !== undefined)
                                                    {
                                                        inner.captureToLocation(location);
                                                    }
                                                }
                                                function applyPicture(path)
                                                {
                                                    contactAvatarImage.mipmap = false;
                                                    contactDialog.contact.customAvatar = true;
                                                    contactDialog.contact.avatar = appHelper.fileNameFromPath(path);
                                                    contactAvatarImage.source = contactDialog.contact.avatarSource();
                                                }
                                           }
                                        }

                                        x: avatarBar.width / 2 - contactAvatarImage.width / 2
                                        width: 128
                                        height: 128
                                        fillMode: Image.PreserveAspectCrop

                                        source: contactDialog.contact.avatarSource()

                                        layer.enabled: rounded
                                        layer.effect: OpacityMask
                                        {
                                            maskSource: Item
                                            {
                                                width: contactAvatarImage.width
                                                height: contactAvatarImage.height

                                                Rectangle
                                                {
                                                    anchors.centerIn: parent
                                                    width: contactAvatarImage.adapt ? contactAvatarImage.width : Math.min(contactAvatarImage.width, contactAvatarImage.height)
                                                    height: contactAvatarImage.adapt ? contactAvatarImage.height : width
                                                    radius: Math.min(width, height)
                                                }
                                            }
                                        }

                                        FolderListModel
                                        {
                                            id: avatarsListModel
                                            property int currentIndex;

                                            Component.onCompleted:
                                            {
                                               currentIndex = -1;
                                               folder = gravioApp.assetsPath + "/images/avatar";
                                            }

                                            function getNext()
                                            {
                                                if (currentIndex + 1 < avatarsListModel.count)
                                                {
                                                    currentIndex++;
                                                }
                                                else
                                                {
                                                    currentIndex = 0;
                                                }

                                                return avatarsListModel.get(currentIndex, "fileName");
                                            }
                                        }
                                    }
                                    ToolButton
                                    {
                                        id: nextAvatarButton
                                        x: avatarBar.width / 2 + contactAvatarImage.width / 2 + 5.0
                                        y: 8
                                        contentItem: Image
                                        {
                                            id: nextAvatarButtonImage
                                            fillMode: Image.Pad
                                            horizontalAlignment: Image.AlignHCenter
                                            verticalAlignment: Image.AlignVCenter
                                            source: "../../images/black/chevron.png"
                                        }

                                        onClicked:
                                        {
                                            contactDialog.contact.customAvatar = false;
                                            contactDialog.contact.avatar = avatarsListModel.getNext();
                                            contactAvatarImage.mipmap = false;
                                            contactAvatarImage.source = contactDialog.contact.avatarSource();
                                        }
                                    }
                                    ToolButton
                                    {
                                        id: chooseAvatarButton
                                        x: avatarBar.width / 2 + contactAvatarImage.width / 2 + 5.0
                                        y: avatarBar.height - 55
                                        contentItem: Image
                                        {
                                            id: chooseAvatarButtonImage
                                            fillMode: Image.Pad
                                            horizontalAlignment: Image.AlignHCenter
                                            verticalAlignment: Image.AlignVCenter
                                            source: "../../images/black/open.png"
                                        }

                                        onClicked:
                                        {
                                            var lComponent = Qt.createComponent("filedialog.qml");
                                            var lFileDialog = lComponent.createObject(page);

                                            lFileDialog.selectPicture(chooseAvatarButton);
                                        }

                                        function acceptSelection(url)
                                        {
                                            console.log(url);

                                            contactAvatarImage.mipmap = false;
                                            contactDialog.contact.customAvatar = true;

                                            contactDialog.contact.copyAvatar(url);
                                            console.log(appHelper.fileNameFromPath(url));
                                            contactDialog.contact.avatar = appHelper.fileNameFromPath(url);
                                            contactAvatarImage.source = contactDialog.contact.avatarSource();
                                        }
                                    }
                                }
                            }
                            Row
                            {
                                TextField
                                {
                                    id: nameEdit
                                    text: contactDialog.contact.name
                                    placeholderText: "Enter name"
                                    width: contactDialog.width - 50
                                }
                            }
                            Row
                            {
                                TextField
                                {
                                    id: fullNameEdit
                                    text: contactDialog.contact.fullName
                                    placeholderText: "Enter full name"
                                    width: contactDialog.width - 50
                                }
                            }
                        }
                    }
                }
                Page
                {
                    Rectangle
                    {
                        color: "white"
                        anchors.fill: parent

                        Column
                        {
                            id: contactAddressesColumn
                            spacing: 10

                            Row
                            {
                                height: 10
                                Label
                                {
                                    y: 10
                                    verticalAlignment: Label.AlignBottom
                                    text: "Addresses"
                                }
                            }
                            Row
                            {
                                height: 10
                                x: -5
                                ToolSeparator
                                {
                                    width: contactDialog.width - 40
                                    orientation: Qt.Horizontal
                                }
                            }
                            Row
                            {
                                RowLayout
                                {
                                    spacing: 5
                                    Rectangle
                                    {
                                        id: addressTypeHeader
                                        color: "#3E9AFF"
                                        width: 32
                                        height: 40
                                    }
                                    Rectangle
                                    {
                                        id: addressHeader
                                        color: "#3E9AFF"
                                        width: (contactDialog.width - 55) - addressTypeHeader.width
                                        height: 40

                                        Label
                                        {
                                            text: "Info"
                                            color: "white"
                                            leftPadding: 22
                                            anchors.verticalCenter: parent.verticalCenter
                                        }

                                        ToolButton
                                        {
                                            id: addAddressButton
                                            x: addressHeader.width - 45
                                            y: -5

                                            contentItem: Image
                                            {
                                                id: addAddressButtonImage

                                                fillMode: Image.Pad
                                                horizontalAlignment: Image.AlignHCenter
                                                verticalAlignment: Image.AlignVCenter
                                                source: "add_white.png"
                                            }

                                            onClicked:
                                            {
                                                addressDialog.newAddress(contactDialog.contact.addresses);
                                            }
                                        }
                                    }
                                }
                            }
                            Row
                            {
                                Rectangle
                                {
                                    id: addressesViewBar
                                    y: -5
                                    width: contactDialog.width - 50
                                    height: contactDialog.height - 305

                                    ListView
                                    {
                                        id: addressesView
                                        anchors.fill: addressesViewBar

                                        focus: true
                                        currentIndex: -1

                                        delegate: SwipeDelegate
                                        {
                                            id: delegate
                                            clip: true

                                            contentItem: Row
                                            {
                                                Rectangle
                                                {
                                                    id: addressTypeInfo
                                                    x: -10
                                                    y: 2
                                                    Image
                                                    {
                                                        id: currencyIcon
                                                        width: 22
                                                        height: 22
                                                        mipmap: true
                                                        fillMode: Image.PreserveAspectFit
                                                        source: "../../images/black/" + addressType + ".png"
                                                    }
                                                }
                                                Rectangle
                                                {
                                                    x: -2
                                                    Label
                                                    {
                                                        id: addressInfo
                                                        text: address
                                                        width: addressHeader.width
                                                        font.pixelSize: 13
                                                        wrapMode: Label.WrapAnywhere
                                                        height: 12.0
                                                        leftPadding: addressTypeHeader.width + 10.0
                                                        elide: Text.ElideRight
                                                    }
                                                    Label
                                                    {
                                                        y: 16
                                                        id: labelInfo
                                                        text: "(" + (label != "" ? label : "empty") + ")"
                                                        width: addressHeader.width
                                                        font.pixelSize: 13
                                                        wrapMode: Label.WrapAnywhere
                                                        height: 12.0
                                                        leftPadding: addressTypeHeader.width + 10.0
                                                        elide: Text.ElideRight
                                                    }
                                                }
                                                Rectangle
                                                {
                                                    id: primaryAddressInfo
                                                    x: currencyIcon.width + 2
                                                    y: 2
                                                    Image
                                                    {
                                                        id: primaryAddressIcon
                                                        width: 13
                                                        height: 13
                                                        mipmap: true
                                                        fillMode: Image.PreserveAspectFit
                                                        source: primary ? "../../images/black/apply.png" : ""
                                                    }
                                                }
                                            }

                                            highlighted: ListView.isCurrentItem
                                            width: parent.width

                                            swipe.right: Rectangle
                                            {
                                                id: swipeRect
                                                width: parent.height*2
                                                anchors.right: parent.right
                                                height: parent.height

                                                Rectangle
                                                {
                                                    id: removeRect
                                                    color: swipeRect.SwipeDelegate.pressed ? Qt.darker("tomato", 1.1) : "tomato"
                                                    height: swipeRect.height
                                                    anchors.left: swipeRect.left
                                                    width: parent.height

                                                    Image
                                                    {
                                                        id: removeImage
                                                        x: removeRect.width / 2 - removeImage.width / 2
                                                        y: removeRect.height / 2 - removeImage.height / 2
                                                        source: "../../images/white/remove.png"
                                                    }

                                                    MouseArea
                                                    {
                                                        anchors.fill: removeRect
                                                        onClicked:
                                                        {
                                                            contactDialog.contact.addresses.removeAddress(index);
                                                        }
                                                    }
                                                }
                                                Rectangle
                                                {
                                                    id: cancelRect
                                                    color: swipeRect.SwipeDelegate.pressed ? Qt.darker("orange", 1.1) : "orange"
                                                    height: swipeRect.height
                                                    anchors.right: swipeRect.right
                                                    width: parent.height

                                                    Image
                                                    {
                                                        id: cancelImage
                                                        x: cancelRect.width / 2 - cancelImage.width / 2
                                                        y: cancelRect.height / 2 - cancelImage.height / 2
                                                        source: "../../images/white/back_right.png"
                                                    }

                                                    MouseArea
                                                    {
                                                        anchors.fill: cancelRect
                                                        onClicked:
                                                        {
                                                            delegate.swipe.close();
                                                        }
                                                    }
                                                }
                                            }
                                            onClicked:
                                            {
                                                addressesView.currentIndex = index;
                                                addressDialog.editAddress(contactDialog.contact.addresses.get(index), contactDialog.contact.addresses);
                                            }
                                        }

                                        model: contactDialog.contact.addresses

                                        remove: Transition
                                        {
                                            SequentialAnimation
                                            {
                                                PauseAnimation { duration: 125 }
                                                NumberAnimation { property: "height"; to: 0; easing.type: Easing.InOutQuad }
                                            }
                                        }

                                        displaced: Transition
                                        {
                                            SequentialAnimation
                                            {
                                                PauseAnimation { duration: 125 }
                                                NumberAnimation { property: "y"; easing.type: Easing.InOutQuad }
                                            }
                                        }

                                        ScrollIndicator.vertical: ScrollIndicator {}
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        PageIndicator
        {
            id: indicator

            count: contactSwipeView.count
            currentIndex: contactSwipeView.currentIndex

            anchors.bottom: contactDialogPage.bottom
            anchors.horizontalCenter: contactDialogPage.horizontalCenter
        }
    }
}
