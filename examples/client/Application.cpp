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

    // ２セッションともにログオン済みなら「対象通貨一覧」を取得
    sessionCount++;
    if (sessionCount == 2) {
        SecurityListRequest();
    }
}

void Application::onLogout(const FIX::SessionID &sessionID) {
    std::cout << std::endl << "Logout - " << sessionID << std::endl;

    // セッション数はマイナスにはしない
    sessionCount--;
    if (sessionCount < 0) {
        sessionCount = 0;
    }
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
                      << "m) Markt List" << std::endl
                      << std::endl
                      << "q) Quit" << std::endl
                      << "Action: " << std::endl;
            std::cin >> action;

            if (action == 'q')
                break;
            else if (action == 'm')
                showMarketHistory();
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

// Get Counter
std::string Application::getCnt() { return std::to_string(++id_cnt); }

// Market状態表示
void Application::showMarketHistory() {
    auto st = markets.begin();
    auto ed = markets.end();
    --ed;

    // 履歴の開始～終了の秒差を取得
    int sec = std::chrono::duration_cast<std::chrono::seconds>(ed->tp - st->tp).count();
    std::cout << std::endl << "SEC : " << sec << "   " << std::endl;

    // 履歴を表示
    for (auto itr = markets.begin(); itr != markets.end(); ++itr) {
        auto millisec = std::chrono::duration_cast<std::chrono::milliseconds>(itr->tp.time_since_epoch()).count();
        std::cout
            << std::put_time(itr->tm, "%H:%M:%S")
            << " (" << std::chrono::duration_cast<std::chrono::seconds>(itr->tp - st->tp).count() << ")  : "
            << itr->bid
            << " " << std::setprecision(0) << std::setw(5) << std::right
            << itr->spread
            << " " << std::fixed << std::setprecision(SYMBOL_DIGIT) << std::setw(9) << std::right
            << itr->ask
            << "  " << millisec << " ( " << (millisec % 2 == 0 ? "=0 BUY" : "<>0 SELL") << " )"
            << std::endl;
            //<< "  " << std::ctime(&itr->t);
    }
}

// Market状態確認：履歴の秒差と平均値幅(SPREAD) が設定値内であればTrue、以外はFalse
bool Application::checkMarketHistory() {
    auto st = markets.begin();
    auto ed = markets.end();
    --ed;

    showMarketHistory();

    // 履歴の開始～終了の秒差を取得
    unsigned int sec = std::chrono::duration_cast<std::chrono::seconds>(ed->tp - st->tp).count();
    std::cout << "----- 秒差 : " << sec << " > HISTSEC(" << HISTSEC << ") " << std::endl;
    if (sec > HISTSEC) {
        std::cout << "----- 秒差 false " << std::endl << std::endl;
        return false;
    }
    std::cout << "----- 秒差 true " << std::endl << std::endl;

    // SPREAD 平均計算 - 端数切捨て
    int sum = 0;
    for (auto itr = markets.begin(); itr != markets.end(); ++itr) {
        sum += itr->spread;
    }
    std::cout << "----- 平均値幅 : " << sum << " / " << markets.size() << " = ";
    if (markets.size() > 0 and sum > 0) {
        std::cout << (sum / markets.size()) << " <= SPREAD(" << SPREAD << ")" << std::endl;
        if ((sum / markets.size()) <= SPREAD) {
            std::cout << "----- 平均値幅 true " << std::endl << std::endl;
            return true;
        } else {
            std::cout << "----- 平均値幅 false " << std::endl << std::endl;
            return false;
        }
    } else {
        std::cout << " ０除算" << std::endl << "----- 平均値幅 false " << std::endl << std::endl;
        return false;
    }
}

// 新規注文
void Application::newOrder() {
    // SIDE 注文方向（売り、買い） :: 市場情報取得のミリ秒の偶数奇数で決定
    auto ed = markets.end();
    --ed;
    auto millisec   = std::chrono::duration_cast<std::chrono::milliseconds>(ed->tp.time_since_epoch()).count();
    const char side = (millisec % 2 == 0 ? FIX::Side_BUY : FIX::Side_SELL);

    // Px 指値 :: 市場情報の価格を設定（方向意識）
    const double px = (side == '1' ? ed->ask : ed->bid);

    // 注文
    NewOrderSingle(side, SIZE, FIX::OrdType_LIMIT, px);
}
