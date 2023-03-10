/* -*- C++ -*- */

#ifndef EXAMPLES_CLIENT_APPLICATION_H_
#define EXAMPLES_CLIENT_APPLICATION_H_

#include <chrono>
#include <cmath>
#include <list>
#include <string>

#include "quickfix/Application.h"
#include "quickfix/MessageCracker.h"
#include "quickfix/Mutex.h"
#include "quickfix/Values.h"

// const
const char SessionTypeQUOTE[] = "QUOTE";
const char SessionTypeTRADE[] = "TRADE";

// server -> client
#include "quickfix/fix44/SecurityList.h"  // < y >

// client -> server
#include "quickfix/fix44/SecurityListRequest.h"  // < x >
#include "quickfix/fix44/TestRequest.h"          // < 1 >

class Application : public FIX::Application, public FIX::MessageCracker {
public:
    Application(const FIX::SessionSettings &settings) : m_settings(settings) {}

    void run();

private:
    FIX::SessionSettings m_settings;
    int sessionCount = 0;  // ２セッション無いと取引しないためのカウンタ（Quote、Trade）
    int id_cnt       = 0;  // 通信時に利用するIDカウンタ

    void onCreate(const FIX::SessionID &) {}
    void onLogon(const FIX::SessionID &sessionID);
    void onLogout(const FIX::SessionID &sessionID);
    void toAdmin(FIX::Message &, const FIX::SessionID &);
    void toApp(FIX::Message &, const FIX::SessionID &) EXCEPT(FIX::DoNotSend);
    void fromAdmin(const FIX::Message &message, const FIX::SessionID &sessionID)
        EXCEPT(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon);
    void fromApp(const FIX::Message &message, const FIX::SessionID &sessionID)
        EXCEPT(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType);

    // --------- --------- --------- --------- --------- --------- ---------
    // server -> client
    /* y  */ void onMessage(const FIX44::SecurityList &, const FIX::SessionID &);

    // --------- --------- --------- --------- --------- --------- ---------
    // client -> server
    /* 1  */ void TestRequest();
    /* x  */ void SecurityListRequest();

    // --------- --------- --------- --------- --------- --------- ---------
    // tool
    void SetMessageHeader(FIX::Message &);
    std::string getSetting(const char *, const char *defvalue = "");
    std::string getCnt();
};

#endif
