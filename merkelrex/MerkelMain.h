#pragma once

#include <vector>
#include "OrderBookEntry.h"
#include "OrderBook.h"
#include "Wallet.h"


class MerkelMain
{
    public:
        MerkelMain();
        /** Call this to start the sim */
        void init();
    private: 
        void printMenu();
        void printHelp();
        void printMarketStats();
        void enterAsk();
        void enterBid();
        void printWallet();
        void gotoNextTimeframe();
        int getUserOption();
        void runMerkelRexBot();
        void processUserOption(char userOption);
        void setupWallet();

        std::string currentTime;

//      OrderBook orderBook{"20200317.csv"};
	    OrderBook orderBook{"20200601.csv"};
        Wallet wallet;

};
