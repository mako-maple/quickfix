/* -*- C++ -*- */

#include "Application.h"
#include "quickfix/Session.h"

/* W  */
void Application::onMessage(const FIX44::MarketDataSnapshotFullRefresh &message, const FIX::SessionID &sessionID)
{
    /* INIT Message Data */
    /* 268  */ FIX::FieldBase noMDEntries(FIX::FIELD::NoMDEntries, "");
    /* 52   */ FIX::FieldBase respDateTime(FIX::FIELD::SendingTime, "");
    /* 55   */ FIX::FieldBase symbol(FIX::FIELD::Symbol, "");

    /* Get Message Data */
    /* 268  */ message.getFieldIfSet(noMDEntries);
    /* 52   */ message.getHeader().getFieldIfSet(respDateTime);
    /* 55   */ message.getFieldIfSet(symbol);

    /* Loop: Entries */
    double ask = 0.0;
    double bid = 0.0;
    for (int i = 1; i <= std::stoi(noMDEntries.getString()); i++)
    {
        /* 268  */ FIX44::MarketDataSnapshotFullRefresh::NoMDEntries r;
        message.getGroup(i, r);
        /* 269  */ FIX::FieldBase type(FIX::FIELD::MDEntryType, "");
        /* 270  */ FIX::FieldBase px(FIX::FIELD::MDEntryPx, "0");

        /* 269  */ r.getFieldIfSet(type);
        /* 270  */ r.getFieldIfSet(px);

        if (*type.getString().c_str() == FIX::MDEntryType_BID)
        {
            bid = std::stod(px.getString());
        }
        if (*type.getString().c_str() == FIX::MDEntryType_OFFER)
        {
            ask = std::stod(px.getString());
        }
    }

    /* store MarketData */
    Market market(ask, bid, SYMBOL_DIGIT);
    markets.push_back(market); // 末尾に追加
    if ((int)markets.size() > HISTORY)
        markets.pop_front(); // 最大数を超えてたら最古を削除

    /* Show MarketData */
    std::cout
        << getUTCTimeStr()
        << "   "
        << respDateTime
        << "   "
        << std::fixed << std::setprecision(SYMBOL_DIGIT) << std::setw(9) << std::right
        << bid
        << " "
        << std::setprecision(0) << std::setw(5) << std::right
        << market.spread
        << "   "
        << std::setprecision(SYMBOL_DIGIT) << std::setw(9) << std::right
        << ask;

    // 注文カウント中はカウントを出力
    if (ORDER_COUNT > 0)
    {
        std::cout
            << "  ("
            << ORDER_COUNT
            << ")  ";
    }

    // 注文後は差pipを出力
    else if (ORDER_PX != 0.0)
    {
        std::cout
            << "  : "
            << std::setprecision(0) << std::setw(5) << std::right
            << (ORDER_SIDE == "1" ? /* BUY  */ bid - ORDER_PX : /* SELL */ ORDER_PX - ask) * std::pow(10.0, SYMBOL_DIGIT)
            << " "
            << std::fixed << std::setprecision(SYMBOL_DIGIT) << std::setw(7) << std::right
            << (ORDER_SIDE == "1" ? /* BUY  */ "∧" : /* SELL */ "∨")
            << "  "
            << ORDER_PX;
    }

    std::cout
        << std::endl;

    /* 注文判定 - カウントダウンが終わったら発注 */
    if (ORDER_COUNT > 0)
        ORDER_COUNT--;
    if (ORDER_COUNT == 0)
    {
        if (checkMarketStatus())
        {
            ORDER_PX = (ORDER_SIDE == "1" ? /* BUY */ ask : /* SELL */ bid);
            NewOrderSingle(
                /*  54 */ *ORDER_SIDE.c_str(),
                /*  38 */ SIZE,
                /*  40 */ FIX::OrdType_LIMIT,
                /*  44 */ ORDER_PX);
        }
    }
}

/* :: FIX44-CSERVER.xml
        <message name="MarketDataSnapshotFullRefresh" msgtype="W" msgcat="app">
 262        <field name="MDReqID" required="N"/>
 55         <field name="Symbol" required="Y"/>
 268        <group name="NoMDEntries" required="Y">
 269            <field name="MDEntryType" required="Y"/>
                <field name="QuoteEntryID" required="N"/>
 270            <field name="MDEntryPx" required="Y"/>
                <field name="MDEntrySize" required="N"/>
                <field name="MDEntryID" required="N"/>
            </group>
        </message>
*/