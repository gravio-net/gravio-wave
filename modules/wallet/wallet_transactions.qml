import QtQuick 2.6
import QtQuick.Controls 2.2

import net.gravio.wave.modules.wallet 1.0

Page
{
    Text
    {
        text: "Transactions page"
    }

    // wallet instance
    property Wallet wallet_;
    property variant host_;

    function open(wallet, host)
    {
        wallet_ = wallet;
        host_ = host;
    }

    //
    // common module methods
    //
    function moduleName()
    {
        return wallet_.addressType() + "Transactions";
    }

    function moduleDeactivate()
    {
    }

    function moduleActivate(moduleInfo, host)
    {
    }
}
