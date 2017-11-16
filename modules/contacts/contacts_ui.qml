import QtQuick 2.6
import QtQuick.Controls 2.0

import net.gravio.wave.modules.contacts 1.0

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

            Text
            {
                text: contactsModel.title
            }
        }

        ScrollIndicator.vertical: ScrollIndicator { }
    }
}
