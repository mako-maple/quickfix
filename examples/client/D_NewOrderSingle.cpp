/* -*- C++ -*- */

#include "Application.h"
#include "quickfix/Session.h"

/* D  */
void Application::NewOrderSingle(
    /* 54   */ const char &side = FIX::Side_BUY, /* Side_BUY = '1', Side_SELL = '2'; */
    /* 38   */ const long &qty  = 0,
    /* 40   */ const char &type = FIX::OrdType_LIMIT, /* OrdType_MARKET = '1'
                                                       * OrdType_LIMIT = '2'
                                                       * OrdType_STOP = '3' */
    /* 44   */ const double &px = 0.0) {
    /* ORDER_ID 払い出し */
    std::string orderID = getCnt();

    /* INIT :: 基本情報設定 */
    FIX44::NewOrderSingle message;
    /* 11  */ message.set(FIX::ClOrdID(orderID));
    /* 55  */ message.set(FIX::Symbol(SYMBOL_ID));
    /* 54  */ message.set(FIX::Side(side));
    /* 60  */ message.set(FIX::TransactTime());
    /* 38  */ message.set(FIX::OrderQty(qty));
    /* 40  */ message.set(FIX::OrdType(type));
    /* 494 */ message.set(FIX::Designation(orderID));

    /* 取引状態保持 */
    double pxx = px + (side == FIX::Side_BUY ? -1 : 1);
    Trade trade(TypeNew, std::stoi(orderID), side, qty, pxx);

    /* 新規注文なら */
    if (type == FIX::OrdType_LIMIT and ORDER_ID == "") {
        ORDER_ID = orderID;
        /* 44  */ message.set(FIX::Price(pxx));

        // 注文の有効期限をセット（デフォルト30秒後）
        time_t tim = time(NULL);
        tim += ORDERSEC;
        /* 59  */ message.set(FIX::TimeInForce(FIX::TimeInForce_GOOD_TILL_DATE));
        /* 126 */ message.set(FIX::ExpireTime(FIX::UtcTimeStamp(tim, 0)));
    }

    /* 決済注文なら */
    else if (type == FIX::OrdType_LIMIT and ORDER_ID != "" and SETTLE_ID == "") {
        SETTLE_ID = orderID;
        /* 44  */ message.set(FIX::Price(pxx));

        // 決済注文ならキャンセルするまで有効
        /* 59  */ message.set(FIX::TimeInForce(FIX::TimeInForce_GOOD_TILL_CANCEL));

        // 決済対象は保持しているポジションからIDを設定
        /* 721 */ message.set(FIX::PosMaintRptID(ORDER_POS_ID));

        /* 取引状態保持 */
        trade.type     = TypeSettle;
        trade.parentId = std::stoi(ORDER_ID);
    }

    /* STOP注文なら */
    else if (type == FIX::OrdType_STOP and ORDER_ID != "" and STOP_ID == "") {
        STOP_ID = orderID;
        /* 99  */ message.set(FIX::StopPx(px));

        // STOP注文ならキャンセルするまで有効
        /* 59  */ message.set(FIX::TimeInForce(FIX::TimeInForce_GOOD_TILL_CANCEL));

        // 決済対象は保持しているポジションからIDを設定
        /* 721 */ message.set(FIX::PosMaintRptID(ORDER_POS_ID));

        /* 取引状態保持 */
        trade.type     = TypeStop;
        trade.parentId = std::stoi(ORDER_ID);
    }

    /* いずれでもなければ何もしない */
    else {
        return;
    }

    // 取引状態追加
    trades.push_front(trade);

    // ステータス設定
    status = StatusTrade;

    // -- send
    SetMessageHeader(message);
    FIX::Session::sendToTarget(message, SessionTypeTRADE);

    std::cout << message.toXML() << std::endl;

    // log
    std::cout << "---- < D > ---- NewOrderSingle --------" << std::endl;
    std::cout << "  ID " << orderID
              << "  Side " << (side == FIX::Side_BUY ? "買BUY" : "売SELL")
              << "  Type " << (type == FIX::OrdType_LIMIT ? "LIMIT" : "STOP")
              << "  Qty " << qty
              << "  Px " << px
              << std::endl
              << std::endl;

    OrderMassStatusRequest();
}

/*
        <message name="NewOrderSingle" msgtype="D" msgcat="app">
11          <field name="ClOrdID" required="Y"/>
55          <field name="Symbol" required="Y"/>
54          <field name="Side" required="Y"/>
60          <field name="TransactTime" required="Y"/>
38          <field name="OrderQty" required="Y"/>
40          <field name="OrdType" required="Y"/>
44          <field name="Price" required="N"/>
99          <field name="StopPx" required="N"/>
59          <field name="TimeInForce" required="N"/>
126         <field name="ExpireTime" required="N"/>
721         <field name="PosMaintRptID" required="N"/>
494         <field name="Designation" required="N"/>
        </message>
*/
