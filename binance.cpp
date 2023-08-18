#include "binance.h"

#include <iostream>
#include <string>


std::string b2a_hex( char *byte_arr, int n ) {
    const static std::string HexCodes = "0123456789abcdef";
    std::string HexString;
    for ( int i = 0; i < n ; ++i ) {
        unsigned char BinValue = byte_arr[i];
        HexString += HexCodes[( BinValue >> 4 ) & 0x0F];
        HexString += HexCodes[BinValue & 0x0F];
    }
    return HexString;
}

std::string CalcHmacSHA256(std::string_view decodedKey, std::string_view msg) {
    unsigned char* hash;
    unsigned int hashLen;

    unsigned char* digest = HMAC(
        EVP_sha256(),
        decodedKey.data(),
        static_cast<int>(decodedKey.size()),
        reinterpret_cast<unsigned char const*>(msg.data()),
        static_cast<int>(msg.size()),
        NULL,
        NULL
    );
    return b2a_hex((char*)digest, 32);
}

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* response) {
    size_t totalSize = size * nmemb;
    response->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

Binance::BinanceClient::BinanceClient(const std::string& apiKey, const std::string& secretKey)
    : APIKey_(apiKey)
    , SecretKey_(secretKey)
{
}

std::string Binance::BinanceClient::GetSecretKey() const {
    return SecretKey_;
}

std::string Binance::BinanceClient::GetApiKey() const {
    return APIKey_;
}

Binance::BinanceServer::BinanceServer(const std::string& url)
    : Url_(url)
{
}

std::string Binance::BinanceServer::Ping(const BinanceClient& client) const {
        std::string endpoint = Url_ + "/ping";
        struct curl_slist* headers = nullptr;
        std::string apiHeader = "X-MBX-APIKEY: " + client.GetApiKey();
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, apiHeader.data());

        // Set the cURL options
        CURL* curl = curl_easy_init();
        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, endpoint.data());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

            // Perform the request
            CURLcode res = curl_easy_perform(curl);
            if (res != CURLE_OK) {
                std::cerr << "cURL request failed: " << curl_easy_strerror(res) << std::endl;
                return "Bad connection";
            }

            // Clean up
            curl_easy_cleanup(curl);
            curl_slist_free_all(headers);
            return "Ok!";
        }
        return "Curl initialization error";
}

std::string Binance::LimitOrder::GetQuery() const {
    std::string timestamp = std::to_string(time(nullptr) * 1000);
    return "symbol=" + Symbol_ + "&timeInForce=GTC" + 
            "&side=" + Side_ + "&type=" + "LIMIT" +
            "&quantity=" + std::to_string(Quantity_) + 
            "&price=" + std::to_string(Price_) +
            "&timestamp=" + timestamp;
}

Binance::LimitOrder::LimitOrder(std::string& symbol, std::string& side, std::string& timeInForce, double quantity, double price)
    : Symbol_(symbol)
    , Side_(side)
    , TimeInForce_(timeInForce)
    , Quantity_(quantity)
    , Price_(price)
{
}

std::string Binance::MarketOrder::GetQuery() const {
    std::string timestamp = std::to_string(time(nullptr) * 1000);
    return "symbol=" + Symbol_ + "&side=" + Side_ + 
            "&type=" + "MARKET" +
            "&quantity=" + std::to_string(Quantity_) +
            "&timestamp=" + timestamp;
}

Binance::MarketOrder::MarketOrder(std::string& symbol, std::string& side, double quantity)
    : Symbol_(symbol)
    , Side_(side)
    , Quantity_(quantity)
{
}

std::string Binance::BinanceServer::MakeOrder(const BinanceClient &client, const std::shared_ptr<IOrder> orderInfo) const {
    std::string url = Url_ + "/v3/order";
    std::cerr << url << std::endl;

    std::string queryString = orderInfo->GetQuery();

    std::string signature = CalcHmacSHA256(client.GetSecretKey(), queryString);
    std::string postData = queryString + "&signature=" + signature;

    CURL* curl = curl_easy_init();
    if (curl) {
        std::string response;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, ("X-MBX-APIKEY: " + client.GetApiKey()).c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        std::cout << "Response: " << response << std::endl;
        return response;
    }
    return "Curl initialization error";
}
