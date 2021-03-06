/* -*- C++ -*- */

#ifndef EXAMPLES_CLIENT_APPLICATION_H_
#define EXAMPLES_CLIENT_APPLICATION_H_

#include <chrono>
#include <cmath>
#include <list>
#include <string>

#include "./Market.h"
#include "quickfix/Application.h"
#include "quickfix/MessageCracker.h"
#include "quickfix/Mutex.h"
#include "quickfix/Values.h"

// server -> client
#include "quickfix/fix44/ExecutionReport.h"               // < 8 >
#include "quickfix/fix44/MarketDataSnapshotFullRefresh.h" // < W >
#include "quickfix/fix44/SecurityList.h"                  // < y >

// client -> server
#include "quickfix/fix44/MarketDataRequest.h"   // < V >
#include "quickfix/fix44/NewOrderSingle.h"      // < D >
#include "quickfix/fix44/SecurityListRequest.h" // < x >
#include "quickfix/fix44/TestRequest.h"         // < 1 >

// const
const char SessionTypeQUOTE[] = "QUOTE";
const char SessionTypeTRADE[] = "TRADE";

class Application : public FIX::Application, public FIX::MessageCracker
{
public:
    Application(const FIX::SessionSettings &settings) : m_settings(settings)
    {
        // 設定値読み込み
        SIZE = std::stol(getSetting("SIZE", "100000"));
        RANGE = std::stoi(getSetting("RANGE", "10"));
        STOP = std::stoi(getSetting("STOP", "10"));
        HISTORY = std::stol(getSetting("HISTORY", "10"));
        SPREAD = std::stoi(getSetting("SPREAD", "3"));
        HISTSEC = std::stoi(getSetting("HISTSEC", "5"));
        ORDERSEC = std::stoi(getSetting("ORDERSEC", "30"));
        TARGETpip = std::stoi(getSetting("TARGETpip", "100"));
    }

    void run();

private:
    FIX::SessionSettings m_settings;
    int sessionCount = 0; // ２セッション無いと取引しないためのカウンタ（Quote、Trade）
    int id_cnt = 0;       // 通信時に利用するIDカウンタ

    // setting
    int SIZE = 0;      // 通貨取引単位（$100,000 USD の売買なら 100000 を指定）
    int RANGE = 0;     // 利益確定pip（指定 pip 利益になったら手仕舞い）
    int STOP = 0;      // 損失確定pip（指定 pip 損失になったら手仕舞い）
    int HISTORY = 0;   // 保持するマーケット情報履歴数
    int SPREAD = 0;    // 注文判断時のpip差平均（履歴の平均SPREADが指定値以下なら新規注文実行（荒れた市場には参入しない））
    int HISTSEC = 0;   // 履歴の開始-終了秒差が指定値以下なら新規注文実行（閑散とした市場には参入しない）
    int ORDERSEC = 0;  // 新規注文時の有効秒数（指定秒内に確定しなければ注文キャンセル）
    int TARGETpip = 0; // 取引結果の累積pipが指定値を超えたら終了

    // SYMBOL
    int SYMBOL_DIGIT = 0;         // 取引対象通貨の小数点下桁数
    std::string SYMBOL_ID = "";   // 取引対象通貨のcTrader用ID
    std::string SYMBOL_NAME = ""; // 取引対象通貨の名称

    // ORDER
    int ORDER_COUNT = 0;         // 注文の待ちtick。 カウントダウン用（指定 tick が経過したらOrder）
    std::string ORDER_SIDE = ""; // 注文の方向 1:買い  2:売り
    double ORDER_PX = 0.0;       // 注文時の価格

    std::string ORDER_ID = "";     // 注文の内部ID
    std::string ORDER_POS_ID = ""; // 注文時のポジションID
    std::string STOP_ID = "";      // STOP注文の内部ID
    std::string STOP_POS_ID = "";  // STOP注文時のポジションID
    std::string SETTL_ID = "";     // 決済注文の内部ID
    std::string SETTL_POS_ID = ""; // 決済注文時のポジションID

    std::list<Market> markets;

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
    /* W  */ void onMessage(const FIX44::MarketDataSnapshotFullRefresh &, const FIX::SessionID &);
    /* 8  */ void onMessage(const FIX44::ExecutionReport &, const FIX::SessionID &);

    // --------- --------- --------- --------- --------- --------- ---------
    // client -> server
    /* 1  */ void TestRequest();
    /* x  */ void SecurityListRequest();
    /* V  */ void MarketDataRequest(/* 55  symbol */ std::string);
    /* D  */ void NewOrderSingle(
        /*  54 side  */ const char &, /* Side_BUY = '1', Side_SELL = '2'; */
        /*  38 qty   */ const long &,
        /*  40 type  */ const char &, /* OrdType_MARKET = '1';, OrdType_LIMIT = '2';, OrdType_STOP = '3'; */
        /*  44 px    */ const double &);

    // --------- --------- --------- --------- --------- --------- ---------
    // tool
    void SetMessageHeader(FIX::Message &);
    std::string getSetting(const char *, const char *defvalue = "");
    std::string getCnt();
    std::string getUTCTimeStr();
    void setNewOrder();
    bool checkMarketStatus();
};

#endif // EXAMPLES_CLIENT_APPLICATION_H_