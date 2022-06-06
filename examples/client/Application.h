/* -*- C++ -*- */

#ifndef EXAMPLES_CLIENT_APPLICATION_H_
#define EXAMPLES_CLIENT_APPLICATION_H_

#include "quickfix/Application.h"
#include "quickfix/MessageCracker.h"
#include "quickfix/Values.h"
#include "quickfix/Mutex.h"

// server -> client

// client -> server
#include "quickfix/fix44/TestRequest.h" // < 1 >

// const
const char SessionTypeQUOTE[] = "QUOTE";
const char SessionTypeTRADE[] = "TRADE";

class Application : public FIX::Application,
                    public FIX::MessageCracker
{
public:
  Application(const FIX::SessionSettings &settings)
      : m_settings(settings) {}

  void run();

private:
  FIX::SessionSettings m_settings;

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

  // --------- --------- --------- --------- --------- --------- ---------
  // client -> server
  /* 1  */ void TestRequest();

  // --------- --------- --------- --------- --------- --------- ---------
  // tool
  void SetMessageHeader(FIX::Message &);
  std::string getSetting(const char *, const char *defvalue = "");
};

#endif // EXAMPLES_CLIENT_APPLICATION_H_
