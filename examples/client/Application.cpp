/* -*- C++ -*- */
#include "Application.h"

#include <thread>

#include "quickfix/Session.h"
#include "quickfix/config.h"

void Application::onLogon(const FIX::SessionID &sessionID) {
    std::cout << std::endl
              << "Logon - " << sessionID << std::endl
              << "BEGIN    :" << sessionID.getBeginString() << std::endl
              << "SENDER   :" << sessionID.getSenderCompID() << std::endl
              << "TARGET   :" << sessionID.getTargetCompID() << std::endl
              << "QUALIFIER:" << sessionID.getSessionQualifier() << std::endl
              << std::endl;
}

void Application::onLogout(const FIX::SessionID &sessionID) {
    std::cout << std::endl << "Logout - " << sessionID << std::endl;
}

void Application::fromApp(const FIX::Message &message, const FIX::SessionID &sessionID)
    EXCEPT(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType) {
    crack(message, sessionID);
    // std::cout << message.toXML() << std::endl;
}

void Application::toApp(FIX::Message &message, const FIX::SessionID &sessionID) EXCEPT(FIX::DoNotSend) {
    // std::cout << message.toXML() << std::endl;
    try {
        FIX::PossDupFlag possDupFlag;
        message.getHeader().getField(possDupFlag);
        if (possDupFlag) throw FIX::DoNotSend();
    } catch (FIX::FieldNotFound &) {
    }
}

void Application::fromAdmin(const FIX::Message &message, const FIX::SessionID &sessionID)
    EXCEPT(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon) {
    crack(message, sessionID);
    // std::cout << message.toXML() << std::endl;

    /* 112  */ FIX::FieldBase testRequestID(FIX::FIELD::TestReqID, "");
    /* 112  */ message.getFieldIfSet(testRequestID);
    if (testRequestID.getString() != "") {
        std::cout << "fromAdmin: [1] TestRequestID: " << testRequestID.getString() << std::endl;
    }
}

void Application::toAdmin(FIX::Message &message, const FIX::SessionID &sessionID) {
    if (FIX::MsgType_Logon == message.getHeader().getField(FIX::FIELD::MsgType)) {
        const FIX::Dictionary &dic = m_settings.get(sessionID);
        if (dic.has("TargetSubID")) message.getHeader().setField(FIX::TargetSubID(dic.getString("TargetSubID")));
        if (dic.has("Username")) message.getHeader().setField(FIX::Username(dic.getString("Username")));
        if (dic.has("Password")) message.getHeader().setField(FIX::Password(dic.getString("Password")));
    }
}

void Application::run() {
    // メインループ
    while (true) {
        try {
            char action;
            std::cout << std::endl
                      << "1) TestRequest" << std::endl
                      << std::endl
                      << "q) Quit" << std::endl
                      << "Action: " << std::endl;
            std::cin >> action;

            if (action == 'q')
                break;
            else if (action == '1')
                TestRequest();
        } catch (std::exception &e) {
            std::cout << "Message Not Sent: " << e.what();
        }
    }
}

// Set Message Header
void Application::SetMessageHeader(FIX::Message &message) {
    message.getHeader().setField((FIX::SenderCompID)getSetting("SenderCompID"));
    message.getHeader().setField((FIX::TargetCompID)getSetting("TargetCompID"));
}

// Get Config String
std::string Application::getSetting(const char *key, const char *defvalue) {
    const FIX::Dictionary dic = m_settings.get();
    if (dic.has(key)) return dic.getString(key);
    return defvalue;
}
