import QtQuick 2.6
import QtQuick.Controls 2.2

import net.gravio.wave.modules.wallet 1.0

Page
{
    id: walletTransaction
    default property alias content: pane.contentItem

    Flickable
    {
        anchors.fill: parent
        contentHeight: parent.implicitHeight
        flickableDirection: Flickable.AutoFlickIfNeeded

        Page
        {
            id: pane
            width: parent.width
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
}
