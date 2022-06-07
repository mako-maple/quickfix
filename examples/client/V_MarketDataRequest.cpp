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
262         <field name="MDReqID" required="Y"/>
263         <field name="SubscriptionRequestType" required="Y"/>
264         <field name="MarketDepth" required="Y"/>
265         <field name="MDUpdateType" required="N"/>
267         <group name="NoMDEntryTypes" required="Y">
269             <field name="MDEntryType" required="Y"/>
            </group>
146         <group name="NoRelatedSym" required="Y">
55              <field name="Symbol" required="Y"/>
            </group>
        </message>
*/