#pragma once
#include <string>
#include <curl/curl.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/sha.h>

namespace Binance {
    const std::string ORDER_URL = "/v3/order";
    const std::string PING_URL = "/v3/ping";

    class IOrder {
    public:
        virtual std::string GetQuery() const = 0;
    };

    class LimitOrder : public IOrder {
    public:
        virtual std::string GetQuery() const override;
        LimitOrder(std::string& symbol, std::string& side, std::string& timeInForce, double quantity, double price);
    private:
        std::string Symbol_;
        std::string Side_;
        std::string TimeInForce_;
        double Quantity_;
        double Price_;
    };

    class MarketOrder : public IOrder {
    public:
        virtual std::string GetQuery() const override;
        MarketOrder(std::string& symbol, std::string& side, double quantity);
    private:
        std::string Symbol_;
        std::string Side_;
        double Quantity_;
    };

    class BinanceClient {
    public:
        BinanceClient(const std::string& apiKey, const std::string& secretKey);
        std::string GetApiKey() const;
        std::string GetSecretKey() const;

    private:
        const std::string APIKey_;
        const std::string SecretKey_;
    };

    
    class BinanceServer {
    public:
        BinanceServer(const std::string& url);
        std::string Ping(const BinanceClient& client) const;
        std::string MakeOrder(const BinanceClient& client, const std::shared_ptr<IOrder> orderInfo) const;
    private:
        const std::string Url_;
    };
}