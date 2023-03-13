/* -*- C++ -*- */

#include "Application.h"
#include "quickfix/Session.h"

/* W  */
void Application::onMessage(const FIX44::MarketDataSnapshotFullRefresh &message, const FIX::SessionID &sessionID) {
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
    for (int i = 1; i <= std::stoi(noMDEntries.getString()); i++) {
        /* 268  */ FIX44::MarketDataSnapshotFullRefresh::NoMDEntries r;
        message.getGroup(i, r);
        /* 269  */ FIX::FieldBase type(FIX::FIELD::MDEntryType, "");
        /* 270  */ FIX::FieldBase px(FIX::FIELD::MDEntryPx, "0");

        /* 269  */ r.getFieldIfSet(type);
        /* 270  */ r.getFieldIfSet(px);

        if (*type.getString().c_str() == FIX::MDEntryType_BID) {
            bid = std::stod(px.getString());
        }
        if (*type.getString().c_str() == FIX::MDEntryType_OFFER) {
            ask = std::stod(px.getString());
        }
    }

    /* Show MarketData */
    std::cout << respDateTime 
              << "   " 
              << std::fixed 
              << std::setprecision(SYMBOL_DIGIT) << std::setw(9) << std::right
              << bid 
              << " " 
              << std::setprecision(0) << std::setw(5) << std::right 
              << (ask - bid) * std::pow(10.0, SYMBOL_DIGIT)
              << "   "
              << std::setprecision(SYMBOL_DIGIT) << std::setw(9) << std::right 
              << ask 
              << std::endl;
}

/* :: FIX44-CSERVER.xml
    <message name="MarketDataSnapshotFullRefresh" msgtype="W" msgcat="app">
        <field name="MDReqID" required="N"/>
        <field name="Symbol" required="Y"/>
        <group name="NoMDEntries" required="Y">
            <field name="MDEntryType" required="Y"/>
            <field name="QuoteEntryID" required="N"/>
            <field name="MDEntryPx" required="Y"/>
            <field name="MDEntrySize" required="N"/>
            <field name="MDEntryID" required="N"/>
        </group>
    </message>
 */
