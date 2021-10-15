#include "OrderBookEntry.h"

OrderBookEntry::OrderBookEntry( double _price, 
                        double _amount, 
                        std::string _timestamp, 
                        std::string _product, 
                        OrderBookType _orderType, 
                        std::string _username)
: price(_price), 
  amount(_amount), 
  timestamp(_timestamp),
  product(_product), 
  orderType(_orderType), 
  username(_username)
{
  
    
}

OrderBookType OrderBookEntry::stringToOrderBookType(std::string s)
{
  if (s == "ask")
  {
    return OrderBookType::ask;
  }
  if (s == "bid")
  {
    return OrderBookType::bid;
  }
  if (s == "asksale")
  {
    return OrderBookType::asksale;
  }
  if (s == "bidsale")
  {
    return OrderBookType::bidsale;
  }
  return OrderBookType::unknown;
}

std::string OrderBookEntry::orderBookTypeToString(OrderBookType obt)
{
  if (obt == OrderBookType::ask)
  {
    return "ask";
  }
  if (obt == OrderBookType::bid)
  {
    return "bid";
  }
  if (obt == OrderBookType::asksale)
  {
    return "asksale";
  }
  if (obt == OrderBookType::bidsale)
  {
    return "bidsale";
  }
  return "unknown";
}