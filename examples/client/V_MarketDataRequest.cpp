/* -*- C++ -*- */

#include "Application.h"
#include "quickfix/Session.h"

/* V  */
void Application::MarketDataRequest(std::string symbol) {
    FIX44::MarketDataRequest message;

    /* INIT */
    /* 262  */ FIX::MDReqID reqID(getCnt());
    /* 263  */ FIX::SubscriptionRequestType reqType(FIX::SubscriptionRequestType_SNAPSHOT_PLUS_UPDATES);
    /* 264  */ FIX::MarketDepth depth(1);    // Full book will be provided,
                                             //  0 = Depth subscription;
                                             //  1 = Spot subscription.
    /* 265  */ FIX::MDUpdateType upType(1);  // Only 1 = Incremental refresh

    /* SET */
    /* 262  */ message.set(reqID);
    /* 263  */ message.set(reqType);
    /* 264  */ message.set(depth);
    /* 265  */ message.set(upType);

    // -- set group MarketData Entry Types
    /* 267  */ FIX44::MarketDataRequest::NoMDEntryTypes entryType;
    /* 269  */ entryType.set(FIX::MDEntryType(FIX::MDEntryType_BID));
    /* 269  */ entryType.set(FIX::MDEntryType(FIX::MDEntryType_OFFER));
    message.addGroup(entryType);

    // -- set group Symbol
    /* 146  */ FIX44::MarketDataRequest::NoRelatedSym noRelatedSym;
    /* 55   */ noRelatedSym.set(FIX::Symbol(symbol));
    message.addGroup(noRelatedSym);

    // -- send
    SetMessageHeader(message);
    FIX::Session::sendToTarget(message, SessionTypeQUOTE);
}

/* :: FIX44-CSERVER.xml
    <message name="MarketDataRequest" msgtype="V" msgcat="app">
        <field name="MDReqID" required="Y"/>
        <field name="SubscriptionRequestType" required="Y"/>
        <field name="MarketDepth" required="Y"/>
        <field name="MDUpdateType" required="N"/>
        <group name="NoMDEntryTypes" required="Y">
            <field name="MDEntryType" required="Y"/>
        </group>
        <group name="NoRelatedSym" required="Y">
            <field name="Symbol" required="Y"/>
        </group>
    </message>
*/
