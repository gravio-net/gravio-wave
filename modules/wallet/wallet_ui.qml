import QtQuick 2.6
import QtQuick.Controls 2.0

import net.gravio.wave.modules.wallet 1.0

Page
{
    id: page

    SwipeView
    {
        id: walletsSwipeView

        anchors.fill: parent
        currentIndex: 0
    }

    PageIndicator
    {
        id: indicator
        //y: 60

        count: walletsSwipeView.count
        currentIndex: walletsSwipeView.currentIndex

        anchors.bottom: page.top
        anchors.horizontalCenter: parent.horizontalCenter
    }

    function open()
    {
        if (!initialized_)
        {
            console.log("Creating UI...");
            for (var lIdx = 0; lIdx < walletsModel.count(); lIdx++)
            {
                var lWallet = walletsModel.get(lIdx);

                console.log("Opening wallet " + lWallet.addressType());
                var lComponent = Qt.createComponent("wallet_instance.qml");
                var lWalletInstance = lComponent.createObject(page);

                lWalletInstance.open(lWallet, host_);
                walletsSwipeView.addItem(lWalletInstance);
            }

            initialized_ = true;
        }
    }

    //
    // common module methods
    //
    function moduleDeactivate()
    {
        console.log("[Wallet/moduleDeactivate]: fired");
    }

    function moduleActivate(moduleInfo, host)
    {
        console.log("[Wallet/moduleActivate]: fired");

        moduleName_ = moduleInfo.name;
        host_ = host;

        open();
    }

    //
    // common module properties
    //
    property string moduleName_;
    property bool initialized_;
    property variant host_;
}
