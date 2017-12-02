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
        }

        ScrollIndicator.vertical: ScrollIndicator { }
    }

    //
    // common module methods
    //
    function moduleDeactivate()
    {
        console.log("[Wallet/moduleDeactivate]: fired");
    }

    function moduleActivate(moduleInfo)
    {
        console.log("[Wallet/moduleActivate]: fired");

        moduleName = moduleInfo.name;
    }

    //
    // common module properties
    //
    property string moduleName;
}
