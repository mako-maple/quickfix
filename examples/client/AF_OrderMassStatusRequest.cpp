/* -*- C++ -*- */

#include "Application.h"
#include "quickfix/Session.h"

/* AF  */
void Application::OrderMassStatusRequest() {
    FIX44::OrderMassStatusRequest message;

    /* INIT */
    /* 584  */ FIX::MassStatusReqID reqID( getCnt() );
    /* 585  */ FIX::MassStatusReqType reqType(FIX::MassStatusReqType_STATUS_FOR_ALL_ORDERS);  // 7 Only

    /* SET */
    /* 584  */ message.set(reqID);
    /* 585  */ message.set(reqType);

    // -- send
    SetMessageHeader(message);
    FIX::Session::sendToTarget(message, SessionTypeTRADE);
}

/* :: FIX44-CSERVER.xml
    <message name="OrderMassStatusRequest" msgtype="AF" msgcat="app">
        <field name="MassStatusReqID" required="Y"/>
        <field name="MassStatusReqType" required="Y"/>
        <field name="IssueDate" required="N"/>
     </message>
*/
