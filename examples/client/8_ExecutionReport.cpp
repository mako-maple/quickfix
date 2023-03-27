/* -*- C++ -*- */

#include "Application.h"
#include "quickfix/Session.h"

/* 8  */
void Application::onMessage(const FIX44::ExecutionReport &message, const FIX::SessionID &sessionID) {
    /* INIT Message Data */
    /* 37   */ FIX::FieldBase orderID(FIX::FIELD::OrderID, "");
    /* 41   */ FIX::FieldBase origClOrdID(FIX::FIELD::OrigClOrdID, "");
    /* 11   */ FIX::FieldBase clOrdID(FIX::FIELD::ClOrdID, "");
    /* 911  */ FIX::FieldBase totNumReports(FIX::FIELD::TotNumReports, "");
    /* 150  */ FIX::FieldBase execType(FIX::FIELD::ExecType, "");
    /* 39   */ FIX::FieldBase ordStatus(FIX::FIELD::OrdStatus, "");
    /* 55   */ FIX::FieldBase symbol(FIX::FIELD::Symbol, "");
    /* 54   */ FIX::FieldBase side(FIX::FIELD::Side, "");
    /* 38   */ FIX::FieldBase orderQty(FIX::FIELD::OrderQty, "");
    /* 40   */ FIX::FieldBase ordType(FIX::FIELD::OrdType, "");
    /* 44   */ FIX::FieldBase price(FIX::FIELD::Price, "");
    /* 99   */ FIX::FieldBase stopPx(FIX::FIELD::StopPx, "");
    /* 59   */ FIX::FieldBase timeInForce(FIX::FIELD::TimeInForce, "");
    /* 126  */ FIX::FieldBase expireTime(FIX::FIELD::ExpireTime, "");
    /* 151  */ FIX::FieldBase leavesQty(FIX::FIELD::LeavesQty, "");
    /* 14   */ FIX::FieldBase cumQty(FIX::FIELD::CumQty, "");
    /* 32   */ FIX::FieldBase lastQty(FIX::FIELD::LastQty, "");
    /* 6    */ FIX::FieldBase avgPx(FIX::FIELD::AvgPx, "");
    /* 60   */ FIX::FieldBase transactTime(FIX::FIELD::TransactTime, "");
    /* 58   */ FIX::FieldBase text(FIX::FIELD::Text, "");
    /* 103  */ FIX::FieldBase ordRejReason(FIX::FIELD::OrdRejReason, "");
    /* 721  */ FIX::FieldBase posMaintRptID(FIX::FIELD::PosMaintRptID, "");
    /* 584  */ FIX::FieldBase massStatusReqID(FIX::FIELD::MassStatusReqID, "");

    /* Get Message Data */
    /* 37   */ message.getFieldIfSet(orderID);
    /* 41   */ message.getFieldIfSet(origClOrdID);
    /* 11   */ message.getFieldIfSet(clOrdID);
    /* 911  */ message.getFieldIfSet(totNumReports);
    /* 150  */ message.getFieldIfSet(execType);
    /* 39   */ message.getFieldIfSet(ordStatus);
    /* 55   */ message.getFieldIfSet(symbol);
    /* 54   */ message.getFieldIfSet(side);
    /* 38   */ message.getFieldIfSet(orderQty);
    /* 40   */ message.getFieldIfSet(ordType);
    /* 44   */ message.getFieldIfSet(price);
    /* 99   */ message.getFieldIfSet(stopPx);
    /* 59   */ message.getFieldIfSet(timeInForce);
    /* 126  */ message.getFieldIfSet(expireTime);
    /* 151  */ message.getFieldIfSet(leavesQty);
    /* 14   */ message.getFieldIfSet(cumQty);
    /* 32   */ message.getFieldIfSet(lastQty);
    /* 6    */ message.getFieldIfSet(avgPx);
    /* 60   */ message.getFieldIfSet(transactTime);
    /* 58   */ message.getFieldIfSet(text);
    /* 103  */ message.getFieldIfSet(ordRejReason);
    /* 721  */ message.getFieldIfSet(posMaintRptID);
    /* 584  */ message.getFieldIfSet(massStatusReqID);

    // ExecType String
    std::string ExecType = "Unknown";
    if (execType.getString() == "0") {
        ExecType = "New";
    } else if (execType.getString() == "4") {
        ExecType = "Cancelled";
    } else if (execType.getString() == "5") {
        ExecType = "Replace";
    } else if (execType.getString() == "8") {
        ExecType = "Rejected";
    } else if (execType.getString() == "C") {
        ExecType = "Expired";
    } else if (execType.getString() == "F") {
        ExecType = "Trade";
    } else if (execType.getString() == "I") {
        ExecType = "Order Status";
    }

    // OrderStatus String
    std::string OrderStatus = "Unknown";
    if (ordStatus.getString() == "0") {
        OrderStatus = "New";
    } else if (ordStatus.getString() == "1") {
        OrderStatus = "Partially filled";
    } else if (ordStatus.getString() == "2") {
        OrderStatus = "Filled";
    } else if (ordStatus.getString() == "8") {
        OrderStatus = "Rejected";
    } else if (ordStatus.getString() == "4") {
        OrderStatus = "Cancelled";
    } else if (ordStatus.getString() == "C") {
        OrderStatus = "Expired";
        status = StatusNone;  // カウンタ再セット
        setTradeStatus(std::stoi(orderID.getString()), TradeStatusExpired);
    }

    // log
    std::cout << "--- < 8 > ---- ExecutionReport --------" << std::endl;
    std::cout << "   37 OrderID        : " << orderID << std::endl
              << "   41 origClOrdID    : " << origClOrdID << std::endl
              << "   11 ClOrdID        : " << clOrdID << std::endl
              << "  911 TotNumReports  : " << totNumReports << std::endl
              << "  150 ExecType       : " << execType << "  " << ExecType << std::endl
              << "   39 OrderStatus    : " << ordStatus << "  " << OrderStatus << std::endl
              << "   55 Symbol         : " << symbol << std::endl
              << "   54 Side           : " << side << std::endl
              << "   38 OrderQty       : " << orderQty << std::endl
              << "   40 OrdType        : " << ordType << std::endl
              << "   44 Price          : " << price << std::endl
              << "   99 StopPx         : " << stopPx << std::endl
              << "   59 TimeInForce    : " << timeInForce << std::endl
              << "  126 ExpireTime     : " << expireTime << std::endl
              << "  151 LeavesQty      : " << leavesQty << std::endl
              << "   14 CumQty         : " << cumQty << std::endl
              << "   32 LastQty        : " << lastQty << std::endl
              << "    6 AvgPx          : " << avgPx << std::endl
              << "   60 TransactTime   : " << transactTime << std::endl
              << "   58 Text           : " << text << std::endl
              << "  103 OrdRejReason   : " << ordRejReason << std::endl
              << "  721 PosMaintRptID  : " << posMaintRptID << std::endl
              << "  584 MassStatusReqID: " << massStatusReqID << std::endl
              << std::endl;
}
/* 150 ExecType
                0 = New;           ExecType_NEW
                4 = Canceled;      ExecType_CANCELLED
                5 = Replace;       ExecType_REPLACE
                8 = Rejected;      ExecType_REJECTED
                C = Expired;       ExecType_EXPIRED
                F = Trade;         ExecType_TRADE
                I = Order Status.  ExecType_ORDER_STATUS
*/
/* 39 OrderStatus
                0 = New;
                1 = Partially filled;
                2 = Filled;
                8 = Rejected;
                4 = Cancelled (When an order is partially filled, "Cancelled" is returned signifying Tag 151:
                               LeavesQty is cancelled and will not be subsequently filled);
                C = Expired.
*/

/*
        <message name="ExecutionReport" msgtype="8" msgcat="app">
            <field name="OrderID" required="Y"/>
            <field name="ClOrdID" required="N"/>
            <field name="OrigClOrdID" required="N"/>
            <field name="TotNumReports" required="N"/>
            <field name="ExecType" required="Y"/>
            <field name="OrdStatus" required="Y"/>
            <field name="Symbol" required="N"/>
            <field name="Side" required="N"/>
            <field name="OrderQty" required="N"/>
            <field name="OrdType" required="N"/>
            <field name="Price" required="N"/>
            <field name="StopPx" required="N"/>
            <field name="TimeInForce" required="N"/>
            <field name="ExpireTime" required="N"/>
            <field name="LastQty" required="N"/>
            <field name="LastPx" required="N"/>
            <field name="LeavesQty" required="N"/>
            <field name="CumQty" required="N"/>
            <field name="AvgPx" required="N"/>
            <field name="TransactTime" required="N"/>
            <field name="Text" required="N"/>
            <field name="OrdRejReason" required="N"/>
            <field name="PosMaintRptID" required="N"/>
            <field name="Designation" required="N"/>
            <field name="MassStatusReqID" required="N"/>
            <field name="AbsoluteTP" required="N"/>
            <field name="RelativeTP" required="N"/>
            <field name="AbsoluteSL" required="N"/>
            <field name="RelativeSL" required="N"/>
            <field name="TrailingSL" required="N"/>
            <field name="TriggerMethodSL" required="N"/>
            <field name="GuaranteedSL" required="N"/>
        </message>
*/
