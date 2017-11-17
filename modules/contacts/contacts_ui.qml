import QtQuick 2.6
import QtQuick.Controls 2.0

Page
{
    id: page

    default property alias content: pane.contentItem

    Flickable
    {
        anchors.fill: parent
        contentHeight: pane.implicitHeight
        flickableDirection: Flickable.AutoFlickIfNeeded

        Pane
        {
            id: pane
            width: parent.width

            TextField
            {
                id: searchField
                placeholderText: "Enter search text"
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.fill: parent
            }

            /*
            Text
            {
                text: contactsModel.title
            }
            */
        }

        ScrollIndicator.vertical: ScrollIndicator { }
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

        toolBar.actionActivated.connect(addContact);
        toolBar.setActionImage("file:///" + moduleInfo.modulePath + "/add.png");
    }

    //
    // common module properties
    //
    property string moduleName;

    //
    // private methods
    //
    function addContact()
    {
        console.log("[Contacts/addContact]: fired");
    }
}
