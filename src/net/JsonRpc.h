//
// Created by vaige on 4.2.2024.
//

#ifndef RESERVOIRCONTROLLER_JSONRPC_H
#define RESERVOIRCONTROLLER_JSONRPC_H

#include <nlohmann/json.hpp>
#include <functional>
#include <string>
#include <map>
#include <optional>
#include <exception>
#include <fmt/format.h>



class JsonRpcInterface
{
public:
    using Request = nlohmann::json;
    using Response = nlohmann::json;
    using Params = nlohmann::json;
    using Handler = std::function<Response(const std::optional<Params>&)>;

    void bind(const std::string& name, Handler handler)
    {
        mMethods[name] = handler;
    }

    std::optional<Response> handle(const Request& request)
    {
        if (!validate(request))
            throw std::runtime_error("Request is invalid");

        if (auto it = mMethods.find(request["method"]); it != mMethods.end())
        {
            return it->second(request.contains("params") ? std::optional<Params>{request["params"]} : std::nullopt);
        }
        else
        {
            throw std::runtime_error(fmt::format("No handler for {}", request["method"]));
        }
    }

    // Get description of the provided rpc interface
    nlohmann::json description() const
    {
        nlohmann::json desc;
        for (const auto& [key, value] : mMethods)
        {
            desc["methods"].push_back(key);
        }
        return desc;
    }

private:
    bool validate(const Request& request)
    {
        return request["jsonrpc"] == "2.0" && !request["method"].empty();
    }

    std::map<std::string, Handler> mMethods;
};

#endif //RESERVOIRCONTROLLER_JSONRPC_H
