/* -*- C++ -*- */

#include "Application.h"
#include "quickfix/Session.h"

/* <1>  */
void Application::TestRequest() {
    FIX44::TestRequest message;
    SetMessageHeader(message);

    // send message( Quote & Trade )
    std::vector<std::string> type{"1-Quote", "2-Trade"};
    for (std::string ID : type) {
        message.set(FIX::TestReqID(ID));
        FIX::Session::sendToTarget(message, *ID.c_str() == '1' ? SessionTypeQUOTE : SessionTypeTRADE);
        std::cout << "<1> TestRequest: " << ID << std::endl;
    }
}
