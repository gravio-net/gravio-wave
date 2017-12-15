import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.1
import QtQuick.Controls.Universal 2.1
import QtGraphicalEffects 1.0
import Qt.labs.folderlistmodel 2.1
import QtMultimedia 5.8

import net.gravio.wave.helpers 1.0
import net.gravio.wave.account 1.0

Item
{
    // methods
    function openAccountDialog()
    {
        accountDialog.edit();
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

        x: 10
        y: 10
        width: window.width - 20
        height: window.height - 20

        property AccountAddress accountAddress;
        property AccountAddresses addresses;

        standardButtons: Dialog.Ok | Dialog.Cancel
        onAccepted:
        {
            var lError = "";
            if (addressDialog.accountAddress == null)
            {
                lError = addressDialog.addresses.addAddress(addressTypeCombo.currentText, labelEdit.text, primaryEdit.checked);
            }
            else
            {
                addressDialog.accountAddress.label = labelEdit.text;
                addressDialog.accountAddress.primary = primaryEdit.checked;

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
            addressDialog.accountAddress = null;

            addressTypeCombo.enabled = true;

            addressTypeCombo.currentIndex = -1;
            addressEdit.text = "";
            labelEdit.text = "";
            primaryEdit.checked = false;
            addressDialogCurrencyIcon.source = "images/black/info.png";
            qrCodeEdit.visible = false;

            addressDialog.addresses = addresses;
            addressDialog.open();
        }

        function editAddress(accountAddress, addresses)
        {
            console.log(accountAddress);
            addressDialog.title = "Edit address";
            addressDialog.accountAddress = accountAddress;
            addressTypeCombo.enabled = false;

            addressTypeCombo.currentIndex = addressTypeCombo.getIndex();
            addressEdit.text = addressEdit.getAddress();
            labelEdit.text = labelEdit.getLabel();
            primaryEdit.checked = primaryEdit.getPrimary();
            addressDialogCurrencyIcon.source = addressDialogCurrencyIcon.getIcon();

            qrCodeEdit.visible = true;
            qrCodeEdit.setInfo(addressEdit.getAddress());

            addressDialog.addresses = addresses;
            addressDialog.open();
        }

        Column
        {
            id: addressColumn
            spacing: 10

            Rectangle
            {
                id: addressDialogCurrency
                color: "transparent"
                x: 0
                y: 14

                Image
                {
                    id: addressDialogCurrencyIcon
                    width: 22
                    height: 22
                    mipmap: true
                    fillMode: Image.PreserveAspectFit
                    source: getIcon()

                    function getIcon(value)
                    {
                        if(value === undefined)
                            return (addressDialog.accountAddress != null) ? "images/black/" + addressDialog.accountAddress.originalAddressType + ".png" : "";
                        return "images/black/" + value + ".png";
                    }
                }

                ToolButton
                {
                    id: copyAddressButton
                    x: addressDialog.width - addressDialogCurrencyIcon.width - 60 //addressTypeCombo.width
                    y: -14
                    contentItem: Image
                    {
                        id: copyAddressButtonImage
                        fillMode: Image.Pad
                        horizontalAlignment: Image.AlignHCenter
                        verticalAlignment: Image.AlignVCenter
                        source: "images/black/editcopy.png"
                    }
                    Clipboard
                    {
                        id: clipboard
                    }
                    onClicked:
                    {
                        clipboard.setText(addressEdit.getAddress());
                    }
                }
            }
            ComboBox
            {
                id: addressTypeCombo
                currentIndex: getIndex()
                font.pixelSize: 14
                enabled: true
                x: addressDialogCurrencyIcon.width + 5

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

                onActivated:
                {
                    addressDialogCurrencyIcon.source = addressDialogCurrencyIcon.getIcon(currentText);
                }

                function getIndex()
                {
                    if(addressDialog.accountAddress == null) return -1;

                    switch(addressDialog.accountAddress.originalAddressType)
                    {
                        case "GIO": return 0;
                        case "BTC": return 1;
                        case "LTC": return 2;
                        case "DOGE": return 3;
                    }

                    return -1;
                }
            }
            Rectangle
            {
                id: addressEditRecangle
                y: addressTypeCombo.height + 10
                TextField
                {
                    id: addressEdit
                    text: getAddress()
                    placeholderText: "Address will be there"
                    width: addressDialog.width - 50
                    font.pixelSize: 14
                    readOnly: true
                    selectByMouse: true

                    function getAddress()
                    {
                        return (addressDialog.accountAddress != null) ? addressDialog.accountAddress.address : "";
                    }

                }
            }
            Rectangle
            {
                y: addressTypeCombo.height + 10 + addressEdit.height + 10
                TextField
                {
                    id: labelEdit
                    text: getLabel()
                    placeholderText: "Enter label"
                    width: addressDialog.width - 50
                    font.pixelSize: 14

                    function getLabel()
                    {
                        return (addressDialog.accountAddress != null) ? addressDialog.accountAddress.label : "";
                    }
                }
            }
            Rectangle
            {
                y: addressTypeCombo.height + 10 + addressEdit.height + 10 + labelEdit.height + 10
                CheckBox
                {
                    id: primaryEdit
                    text: "Primary"
                    checked: getPrimary()
                    font.pixelSize: 14

                    function getPrimary()
                    {
                        return (addressDialog.accountAddress != null) ? addressDialog.accountAddress.primary : false;
                    }
                }
            }
            Rectangle
            {
                id: qrCodeEdit
                y: addressTypeCombo.height + 10 + addressEdit.height + 10 + labelEdit.height + 10 + primaryEdit.height + 10
                x: (addressDialog.width-20) / 2 - 85

                property variant qrCode;

                function setInfo(info)
                {
                    qrCode.value = info;
                }

                Component.onCompleted:
                {
                    //
                    // create dialog
                    //

                    var lComponent = Qt.createComponent("qrcode.qml");
                    qrCode = lComponent.createObject(qrCodeEdit);
                    qrCode.width = 170;
                    qrCode.height = 170;
                    qrCode.value = addressEdit.getAddress();
                }
            }
        }
    }

    //
    // Modify account settings dialog
    //
    Dialog
    {
        id: accountDialog
        modal: true
        focus: true
        title: "Account settings"
        x: 10
        y: 10
        width: window.width - 20
        height: window.height - 20

        standardButtons: Dialog.Ok | Dialog.Cancel
        onAccepted:
        {
            gravioAccount.name = nameEdit.text;
            gravioAccount.fullName = fullNameEdit.text;

            var lError = gravioAccount.update();
            if (lError != "") errorDialog.show(lError);

            accountDialog.close();
        }
        onRejected:
        {
            gravioAccount.addresses.refetchModel();
            gravioAccount.revertChanges();

            accountDialog.close();
        }

        function edit()
        {
            accountAvatarImage.mipmap = gravioAccount.avatar == "" ? true : false;
            accountAvatarImage.source = gravioAccount.avatar == "" ? "images/black/photo_frame2.png" : gravioAccount.avatarSource();
            addressesView.model = gravioAccount.addresses;
            accountSwipeView.currentIndex = 0;

            accountDialog.open();
        }

        Rectangle
        {
            id: accountDialogPage
            anchors.fill: parent
            clip: true
            color: "white"

            SwipeView
            {
                id: accountSwipeView

                anchors.fill: accountDialogPage
                currentIndex: 0

                Page
                {
                    Rectangle
                    {
                        color: "white"
                        anchors.fill: parent

                        Column
                        {
                            id: accountColumn
                            spacing: 10

                            Row
                            {
                                Rectangle
                                {
                                    id: avatarBar
                                    width: accountDialog.width - 40
                                    height: 140

                                    ToolButton
                                    {
                                        id: takePhotoButton
                                        x: avatarBar.width / 2 - accountAvatarImage.width / 2 - (takePhotoButton.width + 5)
                                        y: 8
                                        contentItem: Image
                                        {
                                            id: takePhotoButtonImage
                                            fillMode: Image.Pad
                                            horizontalAlignment: Image.AlignHCenter
                                            verticalAlignment: Image.AlignVCenter
                                            source: "images/black/camera.png"
                                        }

                                        onClicked:
                                        {
                                            if (camera.cameraState() != Camera.ActiveState)
                                            {
                                                camera.start();
                                                accountAvatarImage.mipmap = true;
                                                accountAvatarImage.source = "images/black/photo_frame2.png";
                                                cancelPhotoButton.visible = true;
                                            }
                                            else if (camera.cameraState() == Camera.ActiveState) // take a shot
                                            {
                                                cancelPhotoButton.visible = false;
                                                camera.captureToLocation(gravioAccount.avatarSourceFolder());
                                            }
                                        }
                                    }
                                    ToolButton
                                    {
                                        id: cancelPhotoButton
                                        x: avatarBar.width / 2 - accountAvatarImage.width / 2 - (takePhotoButton.width + 5)
                                        y: avatarBar.height - 55
                                        contentItem: Image
                                        {
                                            id: cancelPhotoButtonImage
                                            fillMode: Image.Pad
                                            horizontalAlignment: Image.AlignHCenter
                                            verticalAlignment: Image.AlignVCenter
                                            source: "images/black/cancel.png"
                                        }

                                        onClicked:
                                        {
                                            cancelPhotoButton.visible = false;

                                            if (camera.cameraState() == Camera.ActiveState)
                                            {
                                                camera.stop();
                                                accountAvatarImage.mipmap = false;
                                                accountAvatarImage.source = gravioAccount.avatarSource();
                                            }
                                        }

                                        Component.onCompleted:
                                        {
                                            cancelPhotoButton.visible = false;
                                        }
                                    }

                                    Image
                                    {
                                        id: accountAvatarImage

                                        property bool rounded: true
                                        property bool adapt: true
                                        property int displayWidth: 128
                                        property int displayHeight: 128

                                        //mipmap: true // smoothing

                                        Rectangle
                                        {
                                            id: imageContainer
                                            x: 0 //(-1 * accountAvatarImage.width/2)
                                            y: 0 //-80
                                            //width: accountAvatarImage.width * 2
                                            //height: avatarBar.height * 2
                                            width: accountAvatarImage.displayWidth
                                            height: accountAvatarImage.displayHeight

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
                                                        var lComponent = Qt.createComponent("modules/contacts/cameraview.qml");
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
                                                    accountAvatarImage.mipmap = false;
                                                    gravioAccount.customAvatar = true;
                                                    gravioAccount.avatar = appHelper.fileNameFromPath(path);
                                                    accountAvatarImage.source = gravioAccount.avatarSource();
                                                }
                                           }
                                        }

                                        x: avatarBar.width / 2 - accountAvatarImage.width / 2
                                        width: accountAvatarImage.displayWidth
                                        height: accountAvatarImage.displayHeight
                                        fillMode: Image.PreserveAspectCrop

                                        source: gravioAccount.avatarSource()

                                        layer.enabled: rounded
                                        layer.effect: OpacityMask
                                        {
                                            maskSource: Item
                                            {
                                                width: accountAvatarImage.displayWidth
                                                height: accountAvatarImage.displayHeight

                                                Rectangle
                                                {
                                                    anchors.centerIn: parent
                                                    width: accountAvatarImage.displayWidth
                                                    height: accountAvatarImage.displayHeight
                                                    radius: accountAvatarImage.displayWidth
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
                                        x: avatarBar.width / 2 + accountAvatarImage.width / 2 + 5.0
                                        y: 8
                                        contentItem: Image
                                        {
                                            id: nextAvatarButtonImage
                                            fillMode: Image.Pad
                                            horizontalAlignment: Image.AlignHCenter
                                            verticalAlignment: Image.AlignVCenter
                                            source: "images/black/chevron.png"
                                        }

                                        onClicked:
                                        {
                                            gravioAccount.customAvatar = false;
                                            gravioAccount.avatar = avatarsListModel.getNext();
                                            accountAvatarImage.mipmap = false;
                                            accountAvatarImage.source = gravioAccount.avatarSource();
                                        }
                                    }
                                    ToolButton
                                    {
                                        id: chooseAvatarButton
                                        x: avatarBar.width / 2 + accountAvatarImage.width / 2 + 5.0
                                        y: avatarBar.height - 55
                                        contentItem: Image
                                        {
                                            id: chooseAvatarButtonImage
                                            fillMode: Image.Pad
                                            horizontalAlignment: Image.AlignHCenter
                                            verticalAlignment: Image.AlignVCenter
                                            source: "images/black/open.png"
                                        }

                                        onClicked:
                                        {
                                            var lComponent = Qt.createComponent("modules/contacts/filedialog.qml");
                                            var lFileDialog = lComponent.createObject(window /*main window*/);

                                            lFileDialog.x = accountDialog.x;
                                            lFileDialog.y = accountDialog.y;
                                            lFileDialog.width = accountDialog.width;
                                            lFileDialog.height = accountDialog.height;
                                            lFileDialog.selectPicture(chooseAvatarButton);
                                        }

                                        function acceptSelection(url)
                                        {
                                            console.log(url);

                                            accountAvatarImage.mipmap = false;
                                            gravioAccount.customAvatar = true;

                                            gravioAccount.copyAvatar(url);
                                            console.log(appHelper.fileNameFromPath(url));
                                            gravioAccount.avatar = appHelper.fileNameFromPath(url);
                                            accountAvatarImage.source = gravioAccount.avatarSource();
                                        }
                                    }
                                }
                            }
                            Row
                            {
                                TextField
                                {
                                    id: nameEdit
                                    text: gravioAccount.name
                                    placeholderText: "Enter name"
                                    width: accountDialog.width - 50
                                }
                            }
                            Row
                            {
                                TextField
                                {
                                    id: fullNameEdit
                                    text: gravioAccount.fullName
                                    placeholderText: "Enter full name"
                                    width: accountDialog.width - 50
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
                            id: accountAddressesColumn
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
                                    width: accountDialog.width - 40
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
                                        width: (accountDialog.width - 55) - addressTypeHeader.width
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
                                                source: "modules/contacts/add_white.png"
                                            }

                                            onClicked:
                                            {
                                                addressDialog.newAddress(gravioAccount.addresses);
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
                                    width: accountDialog.width - 50
                                    height: accountDialog.height - 305

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
                                            height: currencyIcon.height + 30.0

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
                                                        source: "images/black/" + addressType + ".png"
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
                                                        source: primary ? "images/black/apply.png" : ""
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
                                                        source: "images/white/remove.png"
                                                    }

                                                    MouseArea
                                                    {
                                                        anchors.fill: removeRect
                                                        onClicked:
                                                        {
                                                            //gravioAccount.addresses.removeAddress(index);
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
                                                        source: "images/white/back_right.png"
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
                                                console.log(index);
                                                addressDialog.editAddress(gravioAccount.addresses.get(index), gravioAccount.addresses);
                                            }
                                        }

                                        model: gravioAccount.addresses

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

            count: accountSwipeView.count
            currentIndex: accountSwipeView.currentIndex

            anchors.bottom: accountDialogPage.bottom
            anchors.horizontalCenter: accountDialogPage.horizontalCenter
        }
    }
}
