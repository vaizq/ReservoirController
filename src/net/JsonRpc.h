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



class JsonRpcInterface
{
public:
    using Request = nlohmann::json;
    using Response = nlohmann::json;
    using Params = nlohmann::json;
    using Handler = std::function<std::optional<Response>(const std::optional<Params>&)>;


    void bind(const std::string& name, Handler handler)
    {
        mMethods[name] = handler;
    }

    std::optional<Response> handle(const Request& request)
    {
        if (auto it = mMethods.find(request["method"]); it != mMethods.end())
        {
            return it->second(request.contains("params") ? std::optional<Params>{request["params"]} : std::nullopt);
        }
        else if (request["method"] == "getDescription")
        {
            return description();
        }

        return {};
    }

private:

    // Get description of the provided rpc interface
    [[nodiscard]] nlohmann::json description() const
    {
        nlohmann::json desc;
        for (const auto& [key, value] : mMethods)
        {
            desc["methods"].push_back(key);
        }
        return desc;
    }

    std::map<std::string, Handler> mMethods;
};

#endif //RESERVOIRCONTROLLER_JSONRPC_H
