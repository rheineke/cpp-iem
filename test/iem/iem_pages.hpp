// Copyright 2026 Reece Heineke<reece.heineke@gmail.com>
//
// IEM trader pages for the session tests. The outstanding-orders table is the
// 2016 page that session_test already used. The others are synthetic: built
// from what the parsers in session.cpp read (row and cell order, classes,
// attributes), not recorded from the live site. Replace them with recorded
// pages when a login is available.
#ifndef CPPIEM_TEST_IEM_IEM_PAGES_HPP_
#define CPPIEM_TEST_IEM_IEM_PAGES_HPP_

namespace iem {
namespace pages {

// MarketTrader.action: one row per contract
constexpr auto kOrderBook = R"(<html><body>
<table class="table">
<thead><tr><th>Contract</th></tr></thead>
<tbody>
  <tr>
    <td class="contract"> FRdown1216 </td>
    <td class="change-cell bestBidPrice"><p> 0.012* </p></td>
    <td class="change-cell bestAskPrice"><p> 0.020 </p></td>
    <td class="change-cell lastPrice"><p> 0.015 </p></td>
    <td class="change-cell quantity" value="3"></td>
    <td class="yourBidsCell"> 2 </td>
    <td class="yourAsksCell"><form>
      <input name="viewAssetHoldings" value=" 1 "/>
    </form></td>
  </tr>
  <tr>
    <td class="contract">FRsame1216</td>
    <td class="change-cell bestBidPrice"><p>-</p></td>
    <td class="change-cell bestAskPrice"><p>0.990*</p></td>
    <td class="change-cell lastPrice"><p>0.985</p></td>
    <td class="change-cell quantity" value=""></td>
    <td class="yourBidsCell">0</td>
    <td class="yourAsksCell">0</td>
  </tr>
</tbody>
</table>
</body></html>)";

// TraderActivity.action, activityType=bid
constexpr auto kOutstandingBids = R"(<html><body>
<table id="data-table" class="table table-striped form-table jumbotronTable">
<h2>Outstanding Bid: FRup1216</h2>
<thead><tr><th>Order Date</th></tr></thead>
<tbody>
  <tr>
    <td>2016-11-02 20:30:54.257</td>
    <td>FedPolicyB</td>
    <td>FRup1216</td>
    <td>bid</td>
    <td>1.000000</td>
    <td>0.001000</td>
    <td></td>
    <td><a href="/iem/trader/TraderActivity.action?cancelBidOrder=&amp;market=51&amp;bidOrder=4700000&amp;asset=3054&amp;activityType=bid" title="Cancel this order">x</a></td>
  </tr>
</tbody>
</table>
</body></html>)";

// TraderActivity.action, activityType=holdings
constexpr auto kHoldings = R"(<html><body>
<table class="table">
<thead><tr><th>Date</th></tr></thead>
<tbody>
  <tr>
    <td>2016-11-03 10:15:00.000</td>
    <td>FedPolicyB</td>
    <td><a>Buy</a></td>
    <td>FRsame1216</td>
    <td>5</td>
    <td>0.950</td>
  </tr>
</tbody>
</table>
</body></html>)";

// TraderMessages.action, home and viewPortfolio
constexpr auto kTraderMessages = R"(<html><body>
<table class="table">
<thead><tr><th>Date</th></tr></thead>
<tbody>
  <tr>
    <td>2016-11-04 09:00:00.000</td>
    <td>Order</td>
    <td>FRsame1216</td>
    <td>Bid entered</td>
    <td>2</td>
    <td>0.500</td>
    <td></td>
  </tr>
</tbody>
</table>
</body></html>)";

}  // namespace pages
}  // namespace iem

#endif  // CPPIEM_TEST_IEM_IEM_PAGES_HPP_
