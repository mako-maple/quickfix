/* -*- C++ -*- */
#include "Application.h"

#include "quickfix/Session.h"
#include "quickfix/config.h"

void Application::onLogon(const FIX::SessionID &sessionID)
{
    std::cout << std::endl
              << "Logon - " << sessionID << std::endl
              << "BEGIN    :" << sessionID.getBeginString() << std::endl
              << "SENDER   :" << sessionID.getSenderCompID() << std::endl
              << "TARGET   :" << sessionID.getTargetCompID() << std::endl
              << "QUALIFIER:" << sessionID.getSessionQualifier() << std::endl
              << std::endl;

    // ２セッションともにログオン済みなら「対象通貨一覧」を取得
    sessionCount++;
    if (sessionCount == 2)
    {
        SecurityListRequest();
    }
}

void Application::onLogout(const FIX::SessionID &sessionID)
{
    std::cout << std::endl
              << "Logout - " << sessionID << std::endl;

    // セッション数はマイナスにはしない
    sessionCount--;
    if (sessionCount < 0)
    {
        sessionCount = 0;
    }
}

void Application::fromApp(const FIX::Message &message, const FIX::SessionID &sessionID)
    EXCEPT(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType)
{
    crack(message, sessionID);
}

void Application::toApp(FIX::Message &message, const FIX::SessionID &sessionID)
    EXCEPT(FIX::DoNotSend)
{
    try
    {
        FIX::PossDupFlag possDupFlag;
        message.getHeader().getField(possDupFlag);
        if (possDupFlag)
            throw FIX::DoNotSend();
    }
    catch (FIX::FieldNotFound &)
    {
    }
}

void Application::fromAdmin(const FIX::Message &message, const FIX::SessionID &sessionID)
    EXCEPT(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon)
{
    crack(message, sessionID);

    /* 112  */ FIX::FieldBase testRequestID(FIX::FIELD::TestReqID, "");
    /* 112  */ message.getFieldIfSet(testRequestID);
    if (testRequestID.getString() != "")
    {
        std::cout << "fromAdmin: [112] TestRequestID: " << testRequestID.getString() << std::endl;
    }
}

void Application::toAdmin(FIX::Message &message, const FIX::SessionID &sessionID)
{
    if (FIX::MsgType_Logon == message.getHeader().getField(FIX::FIELD::MsgType))
    {
        const FIX::Dictionary &dic = m_settings.get(sessionID);
        if (dic.has("TargetSubID"))
            message.getHeader().setField(FIX::TargetSubID(dic.getString("TargetSubID")));
        if (dic.has("Username"))
            message.getHeader().setField(FIX::Username(dic.getString("Username")));
        if (dic.has("Password"))
            message.getHeader().setField(FIX::Password(dic.getString("Password")));
    }
}

void Application::run()
{
    // メインループ
    while (true)
    {
        try
        {
            char action;
            std::cout << std::endl
                      << "1) TestRequest" << std::endl
                      << "d) New Order" << std::endl
                      << "M) Market Data" << std::endl
                      << std::endl
                      << "q) Quit" << std::endl
                      << "Action: " << std::endl;
            std::cin >> action;

            if (action == 'q')
                break;
            else if (action == '1')
                TestRequest();
            else if (action == 'd')
                setNewOrder();
            else if (action == 'M')
                checkMarketStatus();
        }
        catch (std::exception &e)
        {
            std::cout << "Message Not Sent: " << e.what();
        }
    }
}

// Set Message Header
void Application::SetMessageHeader(FIX::Message &message)
{
    message.getHeader().setField((FIX::SenderCompID)getSetting("SenderCompID"));
    message.getHeader().setField((FIX::TargetCompID)getSetting("TargetCompID"));
}

// Get Config String
std::string Application::getSetting(const char *key, const char *defvalue)
{
    const FIX::Dictionary dic = m_settings.get();
    if (dic.has(key))
        return dic.getString(key);
    return defvalue;
}

// Get Counter
std::string Application::getCnt()
{
    return std::to_string(++id_cnt);
}

// Get UTC Time String
std::string Application::getUTCTimeStr()
{
    struct timespec ts;
    struct tm t;

    // Get epoch time
    int ret;
    ret = clock_gettime(CLOCK_REALTIME, &ts);
    if (ret < 0)
        return "";

    // Convert into local and parsed time
    gmtime_r(&ts.tv_sec, &t);

    // Create string with strftime
    char buf[32] = "yyyy/mm/dd hh:mm:ss.000";
    ret = strftime(buf, 32, "%Y/%m/%d %H:%M:%S", &t);
    if (ret < 0)
        return "";

    // Add milli-seconds with snprintf
    char output[32];
    const int msec = ts.tv_nsec / 1000000;
    ret = snprintf(output, 32, "%s.%03d", buf, msec);
    if (ret < 0)
        return buf;

    // Result
    return output;
}

// New Order 準備
void Application::setNewOrder()
{
    // Order 可能？ 取引中ならIDが設定済みのハズ
    if (ORDER_ID != "")
    {
        std::cout << std::endl
                  << "取引中のため新規注文できません。" << std::endl;
        return;
    }

    // Order 待ち状態？ カウントダウン状態なら何もしない
    if (ORDER_COUNT > 0)
    {
        std::cout << std::endl
                  << "注文カウントダウン中のため新規注文できません。" << std::endl;
        return;
    }

    // ナノ秒取得
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);

    // 待ち時間？待ちTick？を設定 - 取得したナノ秒からカウントダウン数を設定
    ORDER_COUNT = ts.tv_nsec / 10000000;

    // 方向設定（買、売） - 取得したナノ秒の偶数奇数により方向を設定
    ORDER_SIDE = (ts.tv_nsec / 1000000 % 2 == 0 ? "1" /* Side_BUY */
                                                : "2" /* Side_SELL */);

    // クリア
    STOP_ID = "";
    SETTL_ID = "";

    // 設定表示
    std::cout << "set New Order ["
              << (ORDER_SIDE == "1" ? /* BUY  */ "∧" : /* SELL */ "∨")
              << "]  count[" << ORDER_COUNT << "]" << std::endl;
    return;
}

// Market 状態確認
bool Application::checkMarketStatus()
{
    // 履歴数が足りなければ対象外
    if ((int)markets.size() < HISTORY)
    {
        return false;
    }

    // 履歴の秒差確認（設定値よりも時間がかかってたら対象外）（マーケットが閑散としてる）
    auto st = markets.begin();
    auto ed = markets.end();
    --ed; // .end() は最後じゃない？
    int sec = std::chrono::duration_cast<std::chrono::seconds>(ed->tm - st->tm).count();
    std::cout << " SEC: " << sec << std::endl;
    if (sec > HISTSEC)
    {
        return false;
    }

    // 履歴のSpread差確認（設定値よりも大きかったら対象外）（マーケットが荒れている）
    int spread = 0;
    for (auto itr = markets.begin(); itr != markets.end(); ++itr)
    {
        /* DEBUG
            std::time_t time_stamp = std::chrono::system_clock::to_time_t(itr->tm);
            std::cout << "\t"
                    << itr->bid << " "
                    << itr->spread << " "
                    << itr->ask << "\t"
                    << std::chrono::duration_cast<std::chrono::seconds>(itr->tm - st->tm).count() << "\t"
                    << std::put_time(std::localtime(&time_stamp), "%Y-%m-%d %H:%M:%S")
                    << std::endl; // std::ctime(&time_stamp);
        */
        spread += itr->spread;
    }
    int sp = std::ceil((double)spread / (double)markets.size());
    /* DEBUG
        std::cout << std::endl
              << " - spread: " << spread << "  count:" << markets.size() << " = " << sp << std::endl;
    */
    if (sp > SPREAD)
    {
        return false;
    }

    // 履歴確認ＯＫ
    return true;
}
