#include <cstddef>
#include <cstdlib>
#include <iostream>
#include "binance.h"

#include <stdio.h>
#include <string>
#include <unistd.h>
#include <getopt.h>
#include <memory>


int main(int argc, char** argv) {
    std::string apiKey;
    std::string secretKey;
    std::string type;
    std::string symbol;
    std::string side;
    std::shared_ptr<Binance::IOrder> order;

    option opts[] = {
        {"api-key", required_argument, NULL, 'k'},
        {"secret-key", required_argument, NULL, 'K'},
        {"symbol", required_argument, NULL, 'S'},
        {"side", required_argument, NULL, 's'},
        {"type", required_argument, NULL, 't'},
        {0}
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "k:K:S:s:t", opts, 0)) != -1) {
        switch (opt) {
            case 'k':
                apiKey = optarg;
                break;
            case 'K':
                secretKey = optarg;
                break;
            case 'S':
                symbol = optarg;
                break;
            case 's':
                side = optarg;
                break;
            case 't':
                type = optarg;
                break;
            case '?':
                printf("Unknown option: %c\n", optopt);
                break;
        }
    }
    double quantity;
    if (type ==  "MARKET") {
        std::cout << "Enter quantity : ";
        std::cin >> quantity;
        order = std::make_shared<Binance::MarketOrder>(symbol, side, quantity);
    } else if (type == "LIMIT") {
        double price;
        std::string timeInForce;
        std::cout << "Enter quantity : ";
        std::cin >> quantity;
        std::cout << "Enter price : ";
        std::cin >> price;
        std::cout << "Enter time in force : ";
        std::cin >> timeInForce;
        order = std::make_shared<Binance::LimitOrder>(symbol, side, timeInForce, quantity, price);
    } else {
        std::cerr << "Unavailable order type" << std::endl;
        return 0;
    }

    Binance::BinanceClient client(apiKey, secretKey);
    Binance::BinanceServer server("https://testnet.binance.vision/api");
    std::string response = server.MakeOrder(client, order);
    std::cerr << response << std::endl;
}   