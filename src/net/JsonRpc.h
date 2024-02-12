//
// Created by vaige on 4.2.2024.
//

#ifndef RESERVOIRCONTROLLER_JSONRPC_H
#define RESERVOIRCONTROLLER_JSONRPC_H

#include <nlohmann/json.hpp>
#include <functional>
#include <string>
#include <vector>
#include <map>
#include <optional>
#include <exception>


struct Parameter
{
    enum class Type
    {
        Boolean,
        Integer,
        Real,
        String
    };

    std::string name;
    Type type;
};


class JsonRpcInterface
{
public:
    using Request = nlohmann::json;
    using Response = nlohmann::json;
    using Procedure = std::function<std::optional<Response>(const std::optional<nlohmann::json>&)>;

    struct Handler
    {
        Procedure procedure;
        std::vector<Parameter> params;
    };

    void bind(const std::string& name, Procedure procedure, const std::vector<Parameter>& params = {})
    {
        mHandlers[name] = Handler{std::move(procedure), params};
    }

    std::optional<Response> handle(const Request& request)
    {
        if (auto it = mHandlers.find(request["method"]); it != mHandlers.end())
        {
            return it->second.procedure(request.contains("params") ? std::optional<nlohmann::json>{request["params"]} : std::nullopt);
        }
        else if (request["method"] == "getDescription" || request["method"] == "getDesc")
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
        for (const auto& [name, handler] : mHandlers)
        {
            nlohmann::json method;
            method["name"] = name;

            for (const auto& param : handler.params)
            {
                nlohmann::json p;
                p["name"] = param.name;
                p["type"] = [&param]() {
                    switch(param.type)
                    {
                        case Parameter::Type::Boolean:
                            return "Boolean";
                        case Parameter::Type::Integer:
                            return "Integer";
                        case Parameter::Type::Real:
                            return "Real";
                        case Parameter::Type::String:
                            return "String";
                    }
                }();
                method["params"].push_back(p);
            }

            desc["methods"].push_back(method);
        }
        return desc;
    }

    std::map<std::string, Handler> mHandlers;
};

#endif //RESERVOIRCONTROLLER_JSONRPC_H

















