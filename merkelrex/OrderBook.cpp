#include "OrderBook.h"
#include "CSVReader.h"
#include <map>
#include <algorithm>
#include <iostream>

/** construct, reading a csv data file */
OrderBook::OrderBook(std::string filename)
{
    orders = CSVReader::readCSV(filename);
}

/** return vector of all know products in the dataset*/
std::vector<std::string> OrderBook::getKnownProducts()
{
    std::vector<std::string> products;

    std::map<std::string, bool> prodMap;

    for (OrderBookEntry &e : orders)
    {
        prodMap[e.product] = true;
    }

    // now flatten the map to a vector of strings
    for (auto const &e : prodMap)
    {
        products.push_back(e.first);
    }

    return products;
}
/** return vector of Orders according to the sent filters*/
std::vector<OrderBookEntry> OrderBook::getOrders(OrderBookType type,
                                                 std::string product,
                                                 std::string timestamp)
{
    std::vector<OrderBookEntry> orders_sub;
    for (OrderBookEntry &e : orders)
    {
        if (e.orderType == type &&
            e.product == product &&
            e.timestamp == timestamp)
        {
            orders_sub.push_back(e);
        }
    }
    return orders_sub;
}

double OrderBook::getHighPrice(std::vector<OrderBookEntry> &orders)
{
    double max = orders[0].price;
    for (OrderBookEntry &e : orders)
    {
        if (e.price > max)
            max = e.price;
    }
    return max;
}

double OrderBook::getLowPrice(std::vector<OrderBookEntry> &orders)
{
    double min = orders[0].price;
    for (OrderBookEntry &e : orders)
    {
        if (e.price < min)
            min = e.price;
    }
    return min;
}

std::string OrderBook::getEarliestTime()
{
    return orders[0].timestamp;
}

std::string OrderBook::getNextTime(std::string timestamp)
{
    std::string next_timestamp = "";
    for (OrderBookEntry &e : orders)
    {
        if (e.timestamp > timestamp)
        {
            next_timestamp = e.timestamp;
            break;
        }
    }
    if (next_timestamp == "")
    {
        next_timestamp = orders[0].timestamp;
    }
    return next_timestamp;
}

void OrderBook::insertOrder(OrderBookEntry &order)
{
    orders.push_back(order);
    std::sort(orders.begin(), orders.end(), OrderBookEntry::compareByTimestamp);
}

std::vector<OrderBookEntry> OrderBook::matchAsksToBids(std::string product, std::string timestamp)
{
    // asks = orderbook.asks
    std::vector<OrderBookEntry> asks = getOrders(OrderBookType::ask,
                                                 product,
                                                 timestamp);
    // bids = orderbook.bids
    std::vector<OrderBookEntry> bids = getOrders(OrderBookType::bid,
                                                 product,
                                                 timestamp);

    // sales = []
    std::vector<OrderBookEntry> sales;

    // I put in a little check to ensure we have bids and asks
    // to process.
    if (asks.size() == 0 || bids.size() == 0)
    {
        std::cout << " OrderBook::matchAsksToBids no bids or asks" << std::endl;
        return sales;
    }

    // sort asks lowest first
    std::sort(asks.begin(), asks.end(), OrderBookEntry::compareByPriceAsc);
    // sort bids highest first
    std::sort(bids.begin(), bids.end(), OrderBookEntry::compareByPriceDesc);
    // for ask in asks:
    std::cout << "max ask " << asks[asks.size() - 1].price << std::endl;
    std::cout << "min ask " << asks[0].price << std::endl;
    std::cout << "max bid " << bids[0].price << std::endl;
    std::cout << "min bid " << bids[bids.size() - 1].price << std::endl;

    for (OrderBookEntry &ask : asks)
    {
        //     for bid in bids:
        for (OrderBookEntry &bid : bids)
        {
            //         if bid.price >= ask.price # we have a match
            if (bid.price >= ask.price)
            {
                //             sale = new order()
                //             sale.price = ask.price
                OrderBookEntry sale{ask.price, 0, timestamp,
                                    product,
                                    OrderBookType::asksale};

                if (bid.username == "simuser")
                {
                    sale.username = "simuser";
                    sale.orderType = OrderBookType::bidsale;
                }
                if (ask.username == "simuser")
                {
                    sale.username = "simuser";
                    sale.orderType = OrderBookType::asksale;
                }

                //             # now work out how much was sold and
                //             # create new bids and asks covering
                //             # anything that was not sold
                //             if bid.amount == ask.amount: # bid completely clears ask
                if (bid.amount == ask.amount)
                {
                    //                 sale.amount = ask.amount
                    sale.amount = ask.amount;
                    //                 sales.append(sale)
                    sales.push_back(sale);
                    //                 bid.amount = 0 # make sure the bid is not processed again
                    bid.amount = 0;
                    //                 # can do no more with this ask
                    //                 # go onto the next ask
                    //                 break
                    break;
                }
                //           if bid.amount > ask.amount:  # ask is completely gone slice the bid
                if (bid.amount > ask.amount)
                {
                    //                 sale.amount = ask.amount
                    sale.amount = ask.amount;
                    //                 sales.append(sale)
                    sales.push_back(sale);
                    //                 # we adjust the bid in place
                    //                 # so it can be used to process the next ask
                    //                 bid.amount = bid.amount - ask.amount
                    bid.amount = bid.amount - ask.amount;
                    //                 # ask is completely gone, so go to next ask
                    //                 break
                    break;
                }

                //             if bid.amount < ask.amount # bid is completely gone, slice the ask
                if (bid.amount < ask.amount &&
                    bid.amount > 0)
                {
                    //                 sale.amount = bid.amount
                    sale.amount = bid.amount;
                    //                 sales.append(sale)
                    sales.push_back(sale);
                    //                 # update the ask
                    //                 # and allow further bids to process the remaining amount
                    //                 ask.amount = ask.amount - bid.amount
                    ask.amount = ask.amount - bid.amount;
                    //                 bid.amount = 0 # make sure the bid is not processed again
                    bid.amount = 0;
                    //                 # some ask remains so go to the next bid
                    //                 continue
                    continue;
                }
            }
        }
    }
    return sales;
}

////////////////////////////////////////////////////////////////////
void OrderBook::insertCurrentTimeData(std::string timestamp)
{
    insertCurrentTimeDataForProduct("ETH/BTC", timestamp);
    insertCurrentTimeDataForProduct("DOGE/BTC", timestamp);
    insertCurrentTimeDataForProduct("BTC/USDT", timestamp);
    insertCurrentTimeDataForProduct("ETH/USDT", timestamp);
    insertCurrentTimeDataForProduct("DOGE/USDT", timestamp);
}

void OrderBook::insertCurrentTimeDataForProduct(std::string product, std::string timestamp)
{
    std::vector<OrderBookEntry> asks;
    std::vector<OrderBookEntry> bids;
    double lastTradedSpread;
    for (OrderBookEntry &e : orders)
    {
        if (e.product == product && e.orderType == OrderBookType::ask && e.timestamp == timestamp)
        {
            asks.push_back(e);
        }
        if (e.product == product && e.orderType == OrderBookType::bid && e.timestamp == timestamp)
        {
            bids.push_back(e);
        }
    }

    if (asks.size() != 0 && bids.size() != 0)
    {
        double askVol, avgAskPrice, bidVol, avgBidPrice;
        for (OrderBookEntry &ask : asks)
        {
            avgAskPrice += ask.price;
            askVol += ask.amount;
        }
        for (OrderBookEntry &bid : bids)
        {
            avgBidPrice += bid.price;
            bidVol += bid.amount;
        }
        avgAskPrice = avgAskPrice / asks.size();
        avgBidPrice = avgBidPrice / bids.size();

        AnalysisData newData = AnalysisData{product, avgAskPrice, askVol, avgBidPrice, bidVol};

        if (product == "ETH/BTC")
        {
            ethBTCData.push_back(newData);
        }
        if (product == "DOGE/BTC")
        {
            dogeBTCData.push_back(newData);
        }
        if (product == "BTC/USDT")
        {
            btcUSDTData.push_back(newData);
        }
        if (product == "ETH/USDT")
        {
            ethUSDTData.push_back(newData);
        }
        if (product == "DOGE/USDT")
        {
            dogeUSDTData.push_back(newData);
        }
        //std::cout << product << " " << (((asks[0].price - bids[bids.size()-1].price) / asks[0].price) * 100) / asks.size() << std::endl;
        lastTradedSpread = (((asks[0].price - bids[0].price) / asks[0].price) * 100) / asks.size(); //bids.size() - 1
        spreadValue[product] = lastTradedSpread;
    }
}

int OrderBook::dataCount()
{
    return ethBTCData.size();
}

void OrderBook::insertCurrentTimeAndRemoveOldestTimeData(std::string timestamp)
{
    ethBTCData.erase(ethBTCData.begin());
    dogeBTCData.erase(dogeBTCData.begin());
    btcUSDTData.erase(btcUSDTData.begin());
    ethUSDTData.erase(ethUSDTData.begin());
    dogeUSDTData.erase(dogeUSDTData.begin());
    insertCurrentTimeData(timestamp);
}

std::string OrderBook::getSufficientData(std::string currentTimestamp)
{

    // check if there are data
    std::string line;
    bool proceed = false;
    if (ethBTCData.size() < 14)
    {
        //skip time line and take data
        std::cout << "Insufficient Data" << std::endl;
        std::cout << "Merkelrex bot will need " << (14 - ethBTCData.size()) << " more data in order to run." << std::endl;
        std::cout << "Time will be skipped to collect sufficient data." << std::endl;
        std::cout << "Would you like to proceed? (Y/N):" << std::endl;
        while (!proceed)
        {
            std::getline(std::cin, line);
            if (line == "N" || line == "n" || line == "no" || line == "No")
            {
                break;
            }
            else if (line == "y" || line == "Y" || line == "yes" || line == "Yes")
            {
                proceed = true;
                break;
            }
            else
            {
                std::cout << "Invalid input, please enter a valid input: " << std::endl;
            }
        }
        if (proceed)
        {
            //skip time
            // int count = 0;
            for (int i = ethBTCData.size(); i < 14; ++i)
            {
                insertCurrentTimeData(currentTimestamp);
                currentTimestamp = getNextTime(currentTimestamp);
            }
        } //else do nothing and exit
    }
    return currentTimestamp;
}

std::vector<OrderBookEntry> OrderBook::analyseAndGenerateEligibleOrders(std::string currentTimestamp, std::map<std::string, double> wallet)
{
    // check wallet contents
    for (std::pair<std::string, double> currencyPair : wallet)
    {
        double productRSI;
        if (currencyPair.first == "BTC")
        {
            // bid DOGE/BTC, ETH/BTC
            // ask BTC/USDT
            // check spread, if spread too huge then liquidity is low, ignore
            if (spreadValue["BTC/USDT"] <= 0.001)
            {
                generateSlopeValue(btcUSDTData);
                // get RSI from data set
                productRSI = calculateRSI(btcUSDTData, OrderBookType::ask);
                // ask or bid have to relook and try to explain
                // since we are selling BTC for USDT, we must see the price that people are
                // willing to pay to buy our product, so its bid price

                // calculate AB ratio for current time
                double abRatio = calculateABRatio(btcUSDTData);

                // getCurrentPrice
                double currentPrice = getCurrentPrice(btcUSDTData);
                double predictedPrice = currentPrice;
                // ab ratio * slope value of product + 'currentPrice'
                for (PredictedB0B1 &pVals : slopeValue)
                {
                    if (pVals.product == "BTC/USDT")
                    {
                        std::cout << currentPrice << std::endl;
                        std::cout << (pVals.b0 + pVals.b1 * abRatio) * currentPrice + currentPrice << std::endl;
                        predictedPrice = (pVals.b0 + pVals.b1 * abRatio) * currentPrice + currentPrice;
                    }
                }
                //if bid and price down trade
                if (productRSI <= 30 && predictedPrice < (currentPrice - currentPrice * spreadValue["BTC/USDT"]))
                {
                    createEligibleOrder("BTC/USDT", currentTimestamp, OrderBookType::bid, predictedPrice, currentPrice);
                }
            }

            //get RSI using ask price for DOGE/BTC, ETH/BTC
            if (spreadValue["DOGE/BTC"] <= 0.001)
            {
                generateSlopeValue(dogeBTCData);
                //get RSI from data set
                productRSI = calculateRSI(dogeBTCData, OrderBookType::bid);
                double abRatio = calculateABRatio(dogeBTCData);
                double currentPrice = getCurrentPrice(dogeBTCData);
                double predictedPrice = currentPrice;
                // ab ratio * slope value of product + 'currentPrice'
                for (PredictedB0B1 &pVals : slopeValue)
                {
                    if (pVals.product == "DOGE/BTC")
                    {
                        std::cout << currentPrice << std::endl;
                        std::cout << (pVals.b0 + pVals.b1 * abRatio) * currentPrice + currentPrice << std::endl;
                        predictedPrice = (pVals.b0 + pVals.b1 * abRatio) * currentPrice + currentPrice;
                    }
                }
                //if bid and price down trade
                if (productRSI >= 60 && predictedPrice > (currentPrice + currentPrice * spreadValue["DOGE/BTC"]))
                {
                    createEligibleOrder("DOGE/BTC", currentTimestamp, OrderBookType::ask, predictedPrice, currentPrice);
                }
            }
            if (spreadValue["ETH/BTC"] <= 0.001)
            {
                generateSlopeValue(ethBTCData);
                //get RSI from data set
                productRSI = calculateRSI(ethBTCData, OrderBookType::ask);
                double abRatio = calculateABRatio(ethBTCData);
                double currentPrice = getCurrentPrice(ethBTCData);
                double predictedPrice = currentPrice;
                // ab ratio * slope value of product + 'currentPrice'
                for (PredictedB0B1 &pVals : slopeValue)
                {
                    if (pVals.product == "ETH/BTC")
                    {
                        std::cout << currentPrice << std::endl;
                        std::cout << (pVals.b0 + pVals.b1 * abRatio) * currentPrice + currentPrice << std::endl;
                        predictedPrice = (pVals.b0 + pVals.b1 * abRatio) * currentPrice + currentPrice;
                    }
                }
                //if bid and price down trade
                if (productRSI >= 60 && predictedPrice > (currentPrice + currentPrice * spreadValue["ETH/BTC"]))
                {
                    createEligibleOrder("ETH/BTC", currentTimestamp, OrderBookType::ask, predictedPrice, currentPrice);
                }
            }
            //check price that people willing to sell for DOGE/BTC, ETH/BTC, we buy DOGE for BTC

            //if RSI high and ab ratio positive
        }

        if (currencyPair.first == "USDT")
        {
            // bid BTC/USDT, DOGE/USDT, ETH/USDT
            if (spreadValue["BTC/USDT"] <= 0.001)
            {
                generateSlopeValue(btcUSDTData);
                //get RSI from data set
                productRSI = calculateRSI(btcUSDTData, OrderBookType::bid);
                double abRatio = calculateABRatio(btcUSDTData);
                double currentPrice = getCurrentPrice(btcUSDTData);
                double predictedPrice = currentPrice;
                for (PredictedB0B1 &pVals : slopeValue)
                {
                    if (pVals.product == "BTC/USDT")
                    {
                        std::cout << currentPrice << std::endl;
                        std::cout << (pVals.b0 + pVals.b1 * abRatio) * currentPrice + currentPrice << std::endl;
                        predictedPrice = (pVals.b0 + pVals.b1 * abRatio) * currentPrice + currentPrice;
                    }
                }
                //if ask and price up trade
                if (productRSI >= 60 && predictedPrice > (currentPrice + currentPrice * spreadValue["BTC/USDT"]))
                {
                    std::cout << "Create" << std::endl;
                    createEligibleOrder("BTC/USDT", currentTimestamp, OrderBookType::ask, predictedPrice, currentPrice);
                }
            }
            if (spreadValue["DOGE/USDT"] <= 0.001)
            {
                generateSlopeValue(dogeUSDTData);
                //get RSI from data set
                productRSI = calculateRSI(dogeUSDTData, OrderBookType::bid);
                double abRatio = calculateABRatio(dogeUSDTData);
                double currentPrice = getCurrentPrice(dogeUSDTData);
                double predictedPrice = currentPrice;
                for (PredictedB0B1 &pVals : slopeValue)
                {
                    if (pVals.product == "DOGE/USDT")
                    {
                        std::cout << currentPrice << std::endl;
                        std::cout << (pVals.b0 + pVals.b1 * abRatio) * currentPrice + currentPrice << std::endl;
                        predictedPrice = (pVals.b0 + pVals.b1 * abRatio) * currentPrice + currentPrice;
                    }
                }
                //if ask and price up trade
                if (productRSI >= 60 && predictedPrice > (currentPrice + currentPrice * spreadValue["DOGE/USDT"]))
                {
                    createEligibleOrder("DOGE/USDT", currentTimestamp, OrderBookType::ask, predictedPrice, currentPrice);
                }
            }
            if (spreadValue["ETH/USDT"] <= 0.001)
            {
                generateSlopeValue(ethUSDTData);
                //get RSI from data set
                productRSI = calculateRSI(ethUSDTData, OrderBookType::bid);
                double abRatio = calculateABRatio(ethUSDTData);
                double currentPrice = getCurrentPrice(ethUSDTData);
                double predictedPrice = currentPrice;
                for (PredictedB0B1 &pVals : slopeValue)
                {
                    if (pVals.product == "ETH/USDT")
                    {
                        std::cout << currentPrice << std::endl;
                        std::cout << (pVals.b0 + pVals.b1 * abRatio) * currentPrice + currentPrice << std::endl;
                        predictedPrice = (pVals.b0 + pVals.b1 * abRatio) * currentPrice + currentPrice;
                    }
                }
                //if ask and price up trade
                if (productRSI >= 60 && predictedPrice > (currentPrice + currentPrice * spreadValue["ETH/USDT"]))
                {
                    createEligibleOrder("ETH/USDT", currentTimestamp, OrderBookType::ask, predictedPrice, currentPrice);
                }
            }
        }
        if (currencyPair.first == "DOGE")
        {
            // ask DOGE/BTC, DOGE/USDT
            if (spreadValue["DOGE/USDT"] <= 0.001)
            {
                generateSlopeValue(dogeUSDTData);
                //get RSI from data set
                productRSI = calculateRSI(dogeUSDTData, OrderBookType::ask);
                double abRatio = calculateABRatio(dogeUSDTData);
                double currentPrice = getCurrentPrice(dogeUSDTData);
                double predictedPrice = currentPrice;
                for (PredictedB0B1 &pVals : slopeValue)
                {
                    if (pVals.product == "DOGE/USDT")
                    {
                        std::cout << currentPrice << std::endl;
                        std::cout << (pVals.b0 + pVals.b1 * abRatio) * currentPrice + currentPrice << std::endl;
                        predictedPrice = (pVals.b0 + pVals.b1 * abRatio) * currentPrice + currentPrice;
                    }
                }
                //if ask and price up trade
                if (productRSI <= 30 && predictedPrice < (currentPrice - currentPrice * spreadValue["DOGE/USDT"]))
                {
                    createEligibleOrder("DOGE/USDT", currentTimestamp, OrderBookType::bid, predictedPrice, currentPrice);
                }
            }
            if (spreadValue["DOGE/BTC"] <= 0.001)
            {
                generateSlopeValue(dogeBTCData);
                //get RSI from data set
                productRSI = calculateRSI(dogeBTCData, OrderBookType::bid);
                double abRatio = calculateABRatio(dogeBTCData);
                double currentPrice = getCurrentPrice(dogeBTCData);
                double predictedPrice = currentPrice;
                for (PredictedB0B1 &pVals : slopeValue)
                {
                    if (pVals.product == "DOGE/BTC")
                    {
                        std::cout << currentPrice << std::endl;
                        std::cout << (pVals.b0 + pVals.b1 * abRatio) * currentPrice + currentPrice << std::endl;
                        predictedPrice = (pVals.b0 + pVals.b1 * abRatio) * currentPrice + currentPrice;
                    }
                }
                //if ask and price up trade
                if (productRSI <= 30 && predictedPrice < (currentPrice - currentPrice * spreadValue["DOGE/BTC"]))
                {
                    createEligibleOrder("DOGE/BTC", currentTimestamp, OrderBookType::ask, predictedPrice, currentPrice);
                }
            }
            // check spread, if spread too huge then liquidity is low, ignore

            //get RSI using ask price
            //get RSI using bid price
        }
        if (currencyPair.first == "ETH")
        {
            // ask ETH/BTC, ETH/USDT
            if (spreadValue["ETH/BTC"] <= 0.001)
            {
                generateSlopeValue(ethBTCData);
                //get RSI from data set
                productRSI = calculateRSI(ethBTCData, OrderBookType::bid);
                double abRatio = calculateABRatio(ethBTCData);
                double currentPrice = getCurrentPrice(ethBTCData);
                double predictedPrice = currentPrice;
                for (PredictedB0B1 &pVals : slopeValue)
                {
                    if (pVals.product == "ETH/BTC")
                    {
                        std::cout << currentPrice << std::endl;
                        std::cout << (pVals.b0 + pVals.b1 * abRatio) * currentPrice + currentPrice << std::endl;
                        predictedPrice = (pVals.b0 + pVals.b1 * abRatio) * currentPrice + currentPrice;
                    }
                }
                //if ask and price up trade
                if (productRSI <= 30 && predictedPrice < (currentPrice - currentPrice * spreadValue["ETH/BTC"]))
                {
                    createEligibleOrder("ETH/BTC", currentTimestamp, OrderBookType::ask, predictedPrice, currentPrice);
                }
            }
            if (spreadValue["ETH/USDT"] <= 0.001)
            {
                generateSlopeValue(ethUSDTData);
                //get RSI from data set
                productRSI = calculateRSI(ethUSDTData, OrderBookType::bid);
                double abRatio = calculateABRatio(ethUSDTData);
                double currentPrice = getCurrentPrice(ethUSDTData);
                double predictedPrice = currentPrice;
                for (PredictedB0B1 &pVals : slopeValue)
                {
                    if (pVals.product == "ETH/USDT")
                    {
                        std::cout << currentPrice << std::endl;
                        std::cout << (pVals.b0 + pVals.b1 * abRatio) * currentPrice + currentPrice << std::endl;
                        predictedPrice = (pVals.b0 + pVals.b1 * abRatio) * currentPrice + currentPrice;
                    }
                }
                //if ask and price up trade
                if (productRSI <= 30 && predictedPrice < (currentPrice - currentPrice * spreadValue["ETH/USDT"]))
                {
                    createEligibleOrder("ETH/USDT", currentTimestamp, OrderBookType::ask, predictedPrice, currentPrice);
                }
            }
        }
    }

    return eligibleOrders;
}

double OrderBook::calculateRSI(std::vector<AnalysisData> productData, OrderBookType orderType)
{
    std::vector<double> gains, losses;
    double avgGain, avgLoss;
    double gainOrLoss;
    for (int i = 0; i < productData.size(); i++)
    {

        if (i != productData.size() - 1)
        {
            //double avgGainLoss = btcUSDTData[i + 1].avgAsk - btcUSDTData[i].avgAsk;
            if (orderType == OrderBookType::ask)
            {
                gainOrLoss = (productData[i + 1].avgAsk - productData[i].avgAsk) / productData[i].avgAsk;
            }
            if (orderType == OrderBookType::bid) // if OrderBookType::bid
            {
                gainOrLoss = (productData[i + 1].avgBid - productData[i].avgBid) / productData[i].avgBid;
            }

            //minus price of current timestamp and push_back into average gain/loss vector
            //if gain, push to gain vector, then loss vector if loss
            if (gainOrLoss >= 0)
            {
                gains.push_back(gainOrLoss);
                avgGain += gainOrLoss;
            }
            else
            {
                losses.push_back(gainOrLoss);
                avgLoss -= gainOrLoss; // - cuz the numbers are negative, avgLoss for RSI is not negative
            }
        }
    }
    //gain loss vector get average
    avgGain = avgGain / gains.size();
    avgLoss = avgLoss / losses.size();

    //calculate RSI
    double RSI = 100 - (100 / (1 + (avgGain / avgLoss)));
    std::cout << "RSI of " << productData[0].product << ": " << RSI << std::endl;
    return RSI;
}

double OrderBook::calculateABRatio(std::vector<AnalysisData> productData)
{
    return productData[productData.size() - 1].askVolume / productData[productData.size() - 1].bidVolume;
}

double OrderBook::getCurrentPrice(std::vector<AnalysisData> productData)
{
    return (productData[productData.size() - 1].avgAsk + productData[productData.size() - 1].avgBid) / 2;
}

void OrderBook::createEligibleOrder(std::string product, std::string currentTimestamp, OrderBookType orderType, double predictedPrice, double currentPrice)
{
    // get the orders that is made in the current time stamp
    OrderBookType newOrderType = OrderBookType::ask;
    if (orderType == OrderBookType::ask)
    {
        newOrderType = OrderBookType::bid;
    }

    std::vector<OrderBookEntry> orders = getOrders(orderType, product, currentTimestamp);
    for (int i = 0; i < orders.size() - 1; ++i)
    {
        if (orders[i].price < predictedPrice)
        {
            OrderBookEntry newOrder{orders[i].price,  //price
                                    orders[i].amount, //amount
                                    currentTimestamp, //timestamp
                                    product,          //product
                                    newOrderType,
                                    "simuser"}; //orderType
            //obe.username = "simuser"
            eligibleOrders.push_back(newOrder);
            targetPrices.push_back(predictedPrice);
        }
        else
        {
            //the orders are sorted in ascending order for the price. 
            //once a price higher than the predicted price, the rest will all be higher. so break;
            break;
        }
    }
    for (OrderBookEntry &obe : eligibleOrders)
    {
        std::cout << obe.product << ", " << obe.price << ", " << obe.amount << ", " << OrderBookEntry::orderBookTypeToString(obe.orderType) << std::endl;
    }
}

void OrderBook::generateSlopeValue(std::vector<AnalysisData> productData){
    // create a vector of ab ratios  as x[]
    // create a vector of price growth or loss with that ratio as y[]
    std::vector<double> x;
    std::vector<double> y;
    // use of 14 historical data
    for (int i = 1; i < productData.size(); ++i)
    {
        x.push_back(productData[i - 1].askVolume / productData[i - 1].bidVolume);
        //growth or loss use the average growth/loss of ask and bid price
        double avgGrowthLoss = (productData[i].avgAsk - productData[i - 1].avgAsk + productData[i].avgBid - productData[i - 1].avgBid) / 2;
        y.push_back(avgGrowthLoss);
    }

    // perform model training
    std::vector<double> error;
    std::map<double, double> b0Vec;
    std::map<double, double> b1Vec;
    double err;
    // initialise b0, b1 and learning rate
    double b0 = 0, b1 = 0, alpha = 0.0001;

    // 4 epochs
    for (int i = 0; i < x.size() * 4; ++i)
    {
        int index = i % x.size(); //access index after each epoch
        double p = b0 + b1 * x[index];
        err = p - y[index];
        b0 = b0 - alpha * err;
        b1 = b1 - alpha * err * x[index];
        //std::cout << "B0 = " << b0 << ", B1 = " << b1 << ", error = " << err << std::endl;
        error.push_back(err);
        b0Vec[err] = b0;
        b1Vec[err] = b1;
    }

    double smallestError = 100;
    for (int i = 0; i < error.size(); ++i)
    {
        if (std::abs(error[i]) < std::abs(smallestError))
        {
            smallestError = error[i];
        }
    }
    std::cout << smallestError << ", " << b0Vec[smallestError] << ", " << b1Vec[smallestError] << std::endl;

    PredictedB0B1 b0b1Values = {productData[0].product, b0Vec[smallestError], b1Vec[smallestError]};

    bool found = false;
    for(int i = 0; i < slopeValue.size(); ++i){
        if(slopeValue[i].product == productData[0].product){
            slopeValue[i] = b0b1Values;
            found = true;
        }
    }
    if(!found){
        slopeValue.push_back(b0b1Values);
    }
}

// for(AnalysisData &ad : ethBTCData){
        //     std::cout << ad.product << ", aVol: " << ad.askVolume << ", avgAsk: " << ad.avgAsk << ", bVol: " << ad.bidVolume << ", avgBid: " << ad.avgBid << std::endl;
        // }
        // for(AnalysisData &ad : dogeBTCData){
        //     std::cout << ad.product << ", aVol: " << ad.askVolume << ", avgAsk: " << ad.avgAsk << ", bVol: " << ad.bidVolume << ", avgBid: " << ad.avgBid << std::endl;
        // }
        // for(AnalysisData &ad : btcUSDTData){
        //     std::cout << ad.product << ", aVol: " << ad.askVolume << ", avgAsk: " << ad.avgAsk << ", bVol: " << ad.bidVolume << ", avgBid: " << ad.avgBid << std::endl;
        // }
        // for(AnalysisData &ad : ethUSDTData){
        //     std::cout << ad.product << ", aVol: " << ad.askVolume << ", avgAsk: " << ad.avgAsk << ", bVol: " << ad.bidVolume << ", avgBid: " << ad.avgBid << std::endl;
        // }
        // for(AnalysisData &ad : dogeUSDTData){
        //     std::cout << ad.product << ", aVol: " << ad.askVolume << ", avgAsk: " << ad.avgAsk << ", bVol: " << ad.bidVolume << ", avgBid: " << ad.avgBid << std::endl;
        // }

        // std::cout << "ETH/BTC: " << spreadValue["ETH/BTC"] << std::endl;
        // std::cout << "DOGE/BTC: " << spreadValue["DOGE/BTC"] << std::endl;
        // std::cout << "BTC/USDT: " << spreadValue["BTC/USDT"] << std::endl;
        // std::cout << "ETH/USDT: " << spreadValue["ETH/USDT"] << std::endl;
        // std::cout << "DOGE/USDT: " << spreadValue["DOGE/USDT"] << std::endl;