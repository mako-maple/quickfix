/* -*- C++ -*- */

// 市場情報(Market)
class Market {
public:
    Market() {}
    Market(const double i_ask, const double i_bid, const int i_digit) : ask(i_ask), bid(i_bid), digit(i_digit) {
        spread = (ask * std::pow(10.0, digit)) - (bid * std::pow(10.0, digit));  // 売買差計算
        tp     = std::chrono::system_clock::now();                               // 市場情報取得時間記録
        t      = std::chrono::system_clock::to_time_t(tp);
        //tm     = std::gmtime(&t);
        tm     = std::localtime(&t);
    }

    double ask = 0.0;                          // 買 - 買うときの値段
    double bid = 0.0;                          // 売 - 売るときの値段
    int digit  = 0;                            // 小数点桁 - 通貨の小数点桁
    int spread = 0;                            // 差 - 売買の差（スプレッド）
    std::chrono::system_clock::time_point tp;  // 市場データの取得時間 time_point 形式
    std::time_t t;                             // 市場データの取得時間 time_t 形式
    std::tm* tm;                               // 市場データの取得時間 tm 形式
};
