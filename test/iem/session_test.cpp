// Copyright 2016 Reece Heineke<reece.heineke@gmail.com>
#include "iem/session.hpp"

#include "gtest/gtest.h"

#include "boost/property_tree/xml_parser.hpp"

namespace iem {

TEST(SessionTest, Session) {
  const auto usr = "foo";
  const auto pwd = "bar";
  const Session s(usr, pwd);
  EXPECT_EQ(s.username(), usr);
  EXPECT_EQ(s.password(), pwd);
  EXPECT_EQ(s.cookie(), "");
}

TEST(SessionTest, PriceParser) {
  auto px = _parse_price("");
  EXPECT_EQ(px, nanPrice());
}

TEST(SessionTest, PositionParser) {
  auto qty = _parse_quantity("");
  EXPECT_EQ(qty, 0);
}

TEST(SessionTest, LimitOrderRequest) {
  const iem::Contract c("FedPolicyB", "FRsame1216");
  const iem::Price px(1);
  const iem::PriceTimeLimit ptl(px, boost::posix_time::not_a_date_time);
  const iem::Single o(c, iem::Side::BUY, 1, ptl);
  const auto request = iem::_order_request(o);
  // TODO(rheineke): Unit test aspects of this
  std::cout << body(request) << std::endl;
}

TEST(SessionTest, ReadHTML) {
  const auto s =
      "<table id=\"data-table\" class=\"table table-striped form-table jumbotronTable\" aria-describedby=\"Trader activity table\">\n"
      "<h2>Outstanding Bid: FRup1216</h2>\n"
      "<thead>\n"
      "    <tr>\n"
      "        <th><label for=\"order.date\">Order Date</label></th>\n"
      "        <th><label for=\"market\">Market</label></th>\n"
      "        <th><label for=\"contract\">Contract</label></th>\n"
      "        <th><label for=\"order.type\">Order Type</label></th>\n"
      "        <th><label for=\"quantity\">Quantity</label></th>\n"
      "        <th><label for=\"price\">Price</label></th>\n"
      "        <th><label for=\"expiration\">Expiration</label></th>\n"
      "        <th><label for=\"order.cancelOrder\">Cancel Order</label></th>\n"
      "    </tr>\n"
      "</thead>\n"
      "<tbody>\n"
      "    <tr>\n"
      "        <td>2016-11-02 20:30:54.257</td>\n"
      "        <td>FedPolicyB</td>\n"
      "        <td>FRup1216</td>\n"
      "        <td>bid</td>\n"
      "        <td>1.000000</td>\n"
      "        <td>0.001000</td>\n"
      "        <td></td>\n"
      "        <td>\n"
      "        <a data-confirm-button=\"Yes, cancel this order\" ata-cancel-button=\"No, take me back\" data-text=\"Are you sure you wish to cancel this order?\" href=\"/iem/trader/TraderActivity.action?cancelBidOrder=&amp;market=51&amp;bidOrder=4700000&amp;asset=3054&amp;activityType=bid\" title=\"Cancel this order\" class=\"submitBtn confirm\"><img class=\"cancelOrderBtn\" src=\"/iem/images/x_icon_red.png\" alt=\"Cancel this bid order\"/>\n"
      "        </a>\n"
      "        </td>\n"
      "    </tr>\n"
      "</tbody>\n"
      "</table>";

  // Boost ptree works on streams only
  std::istringstream is(s);
  // ptree
  boost::property_tree::ptree pt;
  boost::property_tree::read_xml(is, pt);
  const auto tbody = pt.get_child("table.tbody");
  const auto tr_its = tbody.equal_range("tr");
  EXPECT_EQ(std::distance(tr_its.first, tr_its.second), 1);
}

}  // namespace iem
