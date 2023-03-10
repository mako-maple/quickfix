/* -*- C++ -*- */

#include "Application.h"
#include "quickfix/Session.h"

/* <x>  */
void Application::SecurityListRequest() {
    /* 320  */ FIX::SecurityReqID securityReqID(getCnt());
    /* 559  */ FIX::SecurityListRequestType securityListRequestType(FIX::SecurityListRequestType_SYMBOL /* 0 */);

    FIX44::SecurityListRequest message(securityReqID, securityListRequestType);
    SetMessageHeader(message);
    FIX::Session::sendToTarget(message, SessionTypeQUOTE);
}