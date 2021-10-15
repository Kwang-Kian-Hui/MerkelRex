#include "MerkelMain.h"
#include "OrderBookEntry.h"
#include "CSVReader.h"

#include <iostream>
#include <vector>
#include <sstream>
#include <iterator>

MerkelMain::MerkelMain()
{

}

void MerkelMain::init()
{
    setupWallet();
    int input;
    currentTime = orderBook.getEarliestTime();

    while(true)
    {
        printMenu();
        input = getUserOption();
        processUserOption(input);
    }
}

void MerkelMain::setupWallet(){
    std::cout << "Welcome to MerkelRex Bot." << std::endl;
    std::cout << "Lets start by setuping up your wallet." << std::endl;
    
    bool valid = false;
    while(!valid){
        valid = false;
        std::string line;
        std::cout << "Enter your deposit amount e.g. \"USDT 10000\" : " << std::endl;
        std::getline(std::cin, line);
        
        std::istringstream buf(line);
        std::istream_iterator<std::string> beg(buf),end;

        std::vector<std::string> tokens(beg, end);

        if(tokens.size() == 2){
            std::string userChoice;
            wallet.insertCurrency(tokens[0], std::stod(tokens[1]));
            std::cout << "Do you want to add more currencies into the wallet? (Y/N)" << std::endl;
            std::getline(std::cin, line);
            if(line == "N" || line == "n" || line == "no" || line == "No"){
                valid = true;
            }
            // ask if want to insert more currency, if yes, set valid to false
        }else{
            std::cout << "An error occured, please retry." << std::endl;
        }
    }
}

void MerkelMain::printMenu()
{
    std::cout << "===========Merkelrex===========" << std::endl;
    // 1 print help
    std::cout << "1: Print help " << std::endl;
    // 2 print exchange stats
    std::cout << "2: Display exchange rates" << std::endl;
    // 3 make an offer
    std::cout << "3: Make an offer " << std::endl;
    // 4 make a bid 
    std::cout << "4: Make a bid " << std::endl;
    // 5 print wallet
    std::cout << "5: Display wallet contents " << std::endl;
    // 6 continue   
    std::cout << "6: Proceed " << std::endl;

    std::cout << "7: Run Merklerex Bot" << std::endl;

    std::cout << "===============================" << std::endl;

    std::cout << "Current time is: " << currentTime << std::endl;
}

void MerkelMain::printHelp()
{
    std::cout << "Help - your aim is to make money. Analyse the market and make bids and offers. " << std::endl;
}

void MerkelMain::printMarketStats()
{
    std::cout << "------------Exchange Rates-------------" << std::endl;
    for (std::string const& p : orderBook.getKnownProducts())
    {
        std::cout << "Product: " << p << std::endl;
        std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookType::ask, 
                                                                p, currentTime);
        std::cout << "Asks seen: " << entries.size() << std::endl;
        std::cout << "Max ask: " << OrderBook::getHighPrice(entries) << std::endl;
        std::cout << "Min ask: " << OrderBook::getLowPrice(entries) << std::endl;
        std::vector<OrderBookEntry> bEntries = orderBook.getOrders(OrderBookType::bid, p, currentTime);
        std::cout << "Bids seen: " << entries.size() << std::endl;
        std::cout << "Max bid: " << OrderBook::getHighPrice(bEntries) << std::endl;
        std::cout << "Min bid: " << OrderBook::getLowPrice(bEntries) << std::endl;
    }
    std::cout << "-----------end-Exchange Rates------------" << std::endl;
    // std::cout << "OrderBook contains :  " << orders.size() << " entries" << std::endl;
    // unsigned int bids = 0;
    // unsigned int asks = 0;
    // for (OrderBookEntry& e : orders)
    // {
    //     if (e.orderType == OrderBookType::ask)
    //     {
    //         asks ++;
    //     }
    //     if (e.orderType == OrderBookType::bid)
    //     {
    //         bids ++;
    //     }  
    // }    
    // std::cout << "OrderBook asks:  " << asks << " bids:" << bids << std::endl;

}

void MerkelMain::enterAsk()
{
    std::cout << "Make an ask - enter the amount: product,price, amount, eg  ETH/BTC,200,0.5" << std::endl;
    std::string input;
    std::getline(std::cin, input);

    std::vector<std::string> tokens = CSVReader::tokenise(input, ',');
    if (tokens.size() != 3)
    {
        std::cout << "MerkelMain::enterAsk Bad input! " << input << std::endl;
    }
    else {
        try {
            OrderBookEntry obe = CSVReader::stringsToOBE(
                tokens[1],
                tokens[2], 
                currentTime, 
                tokens[0], 
                OrderBookType::ask 
            );
            obe.username = "simuser";
            if (wallet.canFulfillOrder(obe))
            {
                std::cout << "Wallet looks good. " << std::endl;
                orderBook.insertOrder(obe);
            }
            else {
                std::cout << "Wallet has insufficient funds . " << std::endl;
            }
        }catch (const std::exception& e)
        {
            std::cout << " MerkelMain::enterAsk Bad input " << std::endl;
        }   
    }
}

void MerkelMain::enterBid()
{
    std::cout << "Make an bid - enter the amount: product,price, amount, eg  ETH/BTC,200,0.5" << std::endl;
    std::string input;
    std::getline(std::cin, input);

    std::vector<std::string> tokens = CSVReader::tokenise(input, ',');
    if (tokens.size() != 3)
    {
        std::cout << "MerkelMain::enterBid Bad input! " << input << std::endl;
    }
    else {
        try {
            OrderBookEntry obe = CSVReader::stringsToOBE(
                tokens[1],
                tokens[2], 
                currentTime, 
                tokens[0], 
                OrderBookType::bid 
            );
            obe.username = "simuser";

            if (wallet.canFulfillOrder(obe))
            {
                std::cout << "Wallet looks good. " << std::endl;
                orderBook.insertOrder(obe);
            }
            else {
                std::cout << "Wallet has insufficient funds . " << std::endl;
            }
        }catch (const std::exception& e)
        {
            std::cout << " MerkelMain::enterBid Bad input " << std::endl;
        }   
    }
}

void MerkelMain::printWallet()
{
    std::cout << wallet.toString() << std::endl;
}
        
void MerkelMain::gotoNextTimeframe()
{
    std::cout << "Going to next time frame. " << std::endl;
    for (std::string p : orderBook.getKnownProducts())
    {
        std::cout << "matching " << p << std::endl;
        std::vector<OrderBookEntry> sales =  orderBook.matchAsksToBids(p, currentTime);
        std::cout << "Sales: " << sales.size() << std::endl;
        for (OrderBookEntry& sale : sales)
        {
            std::cout << "Sale price: " << sale.price << " amount " << sale.amount << std::endl; 
            if (sale.username == "simuser")
            {
                // update the wallet
                wallet.processSale(sale);
            }
        }
        
    }
    currentTime = orderBook.getNextTime(currentTime);
    if(orderBook.dataCount() < 14){
        orderBook.insertCurrentTimeData(currentTime);
    }else{
        orderBook.insertCurrentTimeAndRemoveOldestTimeData(currentTime);
    }
}
 
void MerkelMain::runMerkelRexBot()
{
    std::cout << "===========================================================================================================" << std::endl;
    std::cout << "--MMM---------MMM--EEEEEEEEE--RRRRRRRR---KKK---KKK--EEEEEEEEE--LLL--------RRRRRRRR---EEEEEEEEE--XXX---XXX--" << std::endl;
    std::cout << "--MMMM-------MMMM--EEE--------RRR---RRR--KKK--KKK---EEE--------LLL--------RRR---RRR--EEE---------XXX-XXX---" << std::endl;
    std::cout << "--MMMMMM---MMMMMM--EEEEEEEEE--RRR--RRR---KKKKKK-----EEEEEEEE---LLL--------RRR--RRR---EEEEEEEE------XXX-----" << std::endl;
    std::cout << "--MMM-MMM-MMM-MMM--EEEEEEEEE--RRRRRR-----KKKKKK-----EEEEEEEE---LLL--------RRRRRR-----EEEEEEEE------XXX-----" << std::endl;
    std::cout << "--MMM--MMMMM--MMM--EEE--------RRR--RRR---KKK--KKK---EEE--------LLL--------RRR--RRR---EEE---------XXX-XXX---" << std::endl;
    std::cout << "--MMM---MMM---MMM--EEEEEEEEE--RRR---RRR--KKK---KKK--EEEEEEEEE--LLLLLLLLL--RRR---RRR--EEEEEEEEE--XXX---XXX--" << std::endl;
    std::cout << "===========================================================================================================" << std::endl;

    currentTime = orderBook.getSufficientData(currentTime);
    
    bool proceed = false;
    std::string line;
    std::cout << "From this point, MerkelRex bot will be able to perform trades automatically on your behalf." << std::endl;
    std::cout << "However, do take note of the risks associated to the trading bot and that we will not be held liable for any losses." << std::endl;
    std::cout << "Allow MerkelRex bot to perform trades automatically? (Y/N):" << std::endl;
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
        std::vector<OrderBookEntry> eligibleOrders = orderBook.analyseAndGenerateEligibleOrders(currentTime, wallet.getWalletContents());
        
        for(OrderBookEntry &eligibleOrder : eligibleOrders){
            if(wallet.canFulfillOrder(eligibleOrder)){
                orderBook.insertOrder(eligibleOrder);
                // log down the inserted orders
            }
        }

        // process sale
        // log down successful trades

        

    } //else do nothing and exit
    // currentTime = orderBook.runMerkelRexBot(currentTime, wallet);

}

int MerkelMain::getUserOption()
{
    int userOption = 0;
    std::string line;
    std::cout << "Enter your option(1-6): ";
    std::getline(std::cin, line);
    try{
        userOption = std::stoi(line);
    }catch(const std::exception& e)
    {
        // 
    }
    std::cout << "You chose: " << userOption << std::endl;
    return userOption;
}

void MerkelMain::processUserOption(char userOption)
{
    switch (userOption)
    {
    case 0:
        std::cout << "Invalid Choice. Please choose 1-6" << std::endl;
        break;
    case 1:
        printHelp();
        break;
    case 2:
        printMarketStats();
        break;
    case 3:
        enterAsk();
        break;
    case 4:
        enterBid();
        break;
    case 5:
        printWallet();
        break;
    case 6:
        gotoNextTimeframe();
        break;
    case 7:
        runMerkelRexBot();
        break;
    default:
        std::cout << "Invalid Choice. Please choose 1-6" << std::endl;
        break;
    }  
}
