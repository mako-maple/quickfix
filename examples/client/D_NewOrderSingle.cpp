/* -*- C++ -*- */

#include "Application.h"
#include "quickfix/Session.h"

/* D  */
void Application::NewOrderSingle(
    /* 54   */ const char &side = FIX::Side_BUY, /* Side_BUY = '1', Side_SELL = '2'; */
    /* 38   */ const long &qty = 0,
    /* 40   */ const char &type = FIX::OrdType_LIMIT, /* OrdType_MARKET = '1'
                                                       * OrdType_LIMIT = '2'
                                                       * OrdType_STOP = '3' */
    /* 44   */ const double &px = 0.0)
{
    FIX44::NewOrderSingle message;

    /* INIT */
    std::string orderID = getCnt();
    /* 11  */ message.set(FIX::ClOrdID(orderID));
    /* 55  */ message.set(FIX::Symbol(SYMBOL_ID));
    /* 54  */ message.set(FIX::Side(side));
    /* 60  */ message.set(FIX::TransactTime());
    /* 38  */ message.set(FIX::OrderQty(qty));
    /* 40  */ message.set(FIX::OrdType(type));
    /* 494 */ message.set(FIX::Designation(orderID));

    if (type == FIX::OrdType_STOP)
    {
        /* 99  */ message.set(FIX::StopPx(px));
        STOP_ID = orderID;
    }
    else
    {
        /* 44  */ message.set(FIX::Price(px));
    }

    /*
    ** const char TimeInForce_GOOD_TILL_CANCEL = '1';    キャンセルするまで有効
    ** const char TimeInForce_IMMEDIATE_OR_CANCEL = '3'; 全部執行できなければ削除（一部約定なし）
    ** const char TimeInForce_GOOD_TILL_DATE = '6';      指定時間まで有効
    **
    ** 新規注文時はリミットを３０秒にセット( TimeInForce_GOOD_TILL_DATE )
    ** 決済注文時はリミットなし( TimeInForce_GOOD_TILL_CANCEL )
    */
    if (ORDER_ID == "")
    {
        // 注文の有効期限をセット（デフォルト30秒後）
        time_t tim = time(NULL);
        tim += ORDERSEC;
        /* 59  */ message.set(FIX::TimeInForce(FIX::TimeInForce_GOOD_TILL_DATE));
        /* 126 */ message.set(FIX::ExpireTime(FIX::UtcTimeStamp(tim, 0)));
        ORDER_ID = orderID;
    }
    else
    {
        /* 59  */ message.set(FIX::TimeInForce(FIX::TimeInForce_GOOD_TILL_CANCEL));
        /* 721 */ message.set(FIX::PosMaintRptID(ORDER_POS_ID));
        std::string s = std::string() + side;
        if (ORDER_SIDE.compare(s)) 
        {
            // 同一方向ならSTOP
        }else{
            SETTL_ID = orderID;
        }
    }

    // -- send
    SetMessageHeader(message);
    FIX::Session::sendToTarget(message, SessionTypeTRADE);

    // log
    std::cout << "--- < D > ---- NewOrderSingle --------" << std::endl;
    std::cout
        << "  ID " << orderID
        << "  Side " << (side == FIX::Side_BUY ? "BUY" : "SELL")
        << "  Type " << (type == FIX::OrdType_LIMIT ? "LIMIT" : "STOP")
        << "  Qty " << qty
        << "  Px " << px
        << std::endl;
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