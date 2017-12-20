import QtQuick 2.6
import QtQuick.Controls 2.2

import net.gravio.wave.modules.wallet 1.0

Page
{
    id: walletInstance
    default property alias content: pane.contentItem

    // wallet instance
    property Wallet wallet_;

    // methods
    function open(wallet)
    {
        wallet_ = wallet;
    }

    Flickable
    {
        anchors.fill: parent
        contentHeight: parent.implicitHeight
        flickableDirection: Flickable.AutoFlickIfNeeded

        Page
        {
            id: pane
            width: parent.width

            //
            //
            //

            Image
            {
                id: coinAvatarImage
                width: 160
                height: 160
                mipmap: true
                fillMode: Image.PreserveAspectFit

                anchors
                {
                    horizontalCenter: parent.horizontalCenter; top: parent.top; topMargin: 20
                }

                source: (wallet_ != null ? ("../../images/" + wallet_.addressType() + ".png") : "")
            }

            //
            // "Balances"
            //
            Label
            {
                x: 10
                y: coinAvatarImage.height + 10
                verticalAlignment: Label.AlignBottom
                font.pixelSize: 16
                text: "Balances"
            }
            ToolSeparator
            {
                x: 5
                y: coinAvatarImage.height + 20
                width: pane.width - 10
                orientation: Qt.Horizontal
            }
            //
            // availableBalance
            //
            Label
            {
                id: availableBalanceLabel
                x: 10
                y: coinAvatarImage.height + 35
                verticalAlignment: Label.AlignBottom
                font.pixelSize: 14
                text: "Available"
            }
            Label
            {
                TextMetrics
                {
                    id: availableMetrics
                    font.pixelSize: 14
                    text: wallet_ != null ? wallet_.availableBalance() : "?.00000000"
                }

                x: pane.width - availableMetrics.width - 10
                y: coinAvatarImage.height + 35
                verticalAlignment: Label.AlignBottom
                font.pixelSize: 14
                text: wallet_ != null ? wallet_.availableBalance() : "?.00000000"
            }
            //
            // pendingBalance
            //
            Label
            {
                id: pendingBalanceLabel
                x: 10
                y: availableBalanceLabel.y + 20
                verticalAlignment: Label.AlignBottom
                font.pixelSize: 14
                text: "Pending"
            }
            Label
            {
                TextMetrics
                {
                    id: pendingMetrics
                    font.pixelSize: 14
                    text: wallet_ != null ? wallet_.pendingBalance() : "?.00000000"
                }

                x: pane.width - pendingMetrics.width - 10
                y: availableBalanceLabel.y + 20
                verticalAlignment: Label.AlignBottom
                font.pixelSize: 14
                text: wallet_ != null ? wallet_.pendingBalance() : "?.00000000"
            }
            //
            // TotalBalance
            //
            Label
            {
                id: totalBalanceLabel
                x: 10
                y: pendingBalanceLabel.y + 20
                verticalAlignment: Label.AlignBottom
                font.pixelSize: 14
                text: "Total"
            }
            Label
            {
                TextMetrics
                {
                    id: totalMetrics
                    font.pixelSize: 14
                    text: wallet_ != null ? wallet_.totalBalance() : "?.00000000"
                }

                x: pane.width - totalMetrics.width - 10
                y: pendingBalanceLabel.y + 20
                verticalAlignment: Label.AlignBottom
                font.pixelSize: 14
                text: wallet_ != null ? wallet_.totalBalance() : "?.00000000"
            }
            //
            // "Recent transactions"
            //
            Label
            {
                id: recentTransactionLabel
                x: 10
                y: totalBalanceLabel.y + 25
                verticalAlignment: Label.AlignBottom
                font.pixelSize: 16
                text: "Recent transactions"
            }
            ToolSeparator
            {
                x: 5
                y: totalBalanceLabel.y + 35
                width: pane.width - 10
                orientation: Qt.Horizontal
            }

            Rectangle
            {
                id: list5Bar
                x: 10
                y: recentTransactionLabel.y + 40
                width: pane.width - 10
                height: walletInstance.height - (recentTransactionLabel.y)
                color: "transparent"

                ListView
                {
                    id: list5View
                    model: wallet_.transactions()

                    focus: true
                    currentIndex: -1
                    anchors.fill: list5Bar

                    delegate: ItemDelegate
                    {
                        id: delegateLastTransaction
                        clip: true
                        width: parent.width

                        //height: 40

                        contentItem: Row
                        {
                            Image
                            {
                                id: txTypeImage
                                x: 0
                                y: 0
                                width: 40
                                height: 40
                                fillMode: Image.PreserveAspectFit

                                source: getSource()

                                function getSource()
                                {
                                    console.log("tx_type=" + type);
                                    if (type == "send_to_address")
                                    {
                                        return "../../images/black/tx_output.png";
                                    }
                                    else if (type == "recv_with_address")
                                    {
                                        return "../../images/black/tx_input.png";
                                    }
                                    else if (type == "generated")
                                    {
                                        return "../../images/black/tx_mined.png";
                                    }
                                }
                            }
                            Label
                            {
                                x: 0
                                y: 0
                                text: address
                            }
                        }
                    }
                }
            }
        }

        ScrollIndicator.vertical: ScrollIndicator { }
    }

}
