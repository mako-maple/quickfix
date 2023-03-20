/* -*- C++ -*- */

// 取引情報(Trade)
class Trade {
public:
    Trade() {}
    Trade(const int i_type, const int i_id, const char i_side, const int i_qty, const int i_px)
        : type(i_type), id(i_id), side(i_side), qty(i_qty), px(i_px) {}

    int type   = 0;  // 1:新規, 2:決済, 3:STOP
    int status = 0;  // 0:送信, 1:注文中, 2:無効, 3:期限切れ, 4:キャンセル, 5:部分約定, 6:全部約定
    int id     = 0;      // 内部取引ＩＤ
    int parentId = 0;    // 親取引内部ＩＤ
    int ordId    = 0;    // cTrader取引ＩＤ
    int posId    = 0;    // ポジションＩＤ
    char side    = 0;    // 売買方向（買,売)
    int qty      = 0;    // 注文数
    int lastQty  = 0;    // 約定数
    double px    = 0.0;  // 注文値
    double avePx = 0.0;  // 約定値（平均）
};
