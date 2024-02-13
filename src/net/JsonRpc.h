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


namespace JsonRpc
{


    struct Parameter
    {
        enum class Type
        {
            Boolean,
            Integer,
            Real,
            String
        };

        Type type;
        std::string name;
    };


    enum class ErrorCode : int
    {
        ParseError = -32700,
        InvalidRequest = -32600,
        MethodNotFound = -32601,
        InvalidParams = 32602,
        InternalError = -32603,
        ServerError = -32000
    };


    class Api
    {
    public:
        using Request = nlohmann::json;
        using Response = nlohmann::json;
        using Procedure = std::function<Response(const Request&)>;

        struct Method
        {
            Procedure procedure;
            std::vector<Parameter> params;
        };

        void bind(const std::string &name, Procedure procedure, std::vector<Parameter> params = {})
        {
            mMethods[name] = Method{std::move(procedure), std::move(params)};
        }

        Response handle(const Request &request)
        {
            if (auto it = mMethods.find(request["method"]); it != mMethods.end()) {
                auto response = it->second.procedure(request);
                response["jsonrpc"] = 2.0;
                return response;
            } else if (request["method"] == "getDescription" || request["method"] == "getDesc") {
                return description();
            }

            return Response{{"jsonrpc", 2.0}};
        }

        static Response createErrorResponse(int id, ErrorCode errorCode, const std::string &errorMessage)
        {
            Api::Response resp;
            resp["id"] = id;
            resp["error"]["code"] = static_cast<int>(errorCode);
            resp["error"]["message"] = errorCode;
            return resp;
        }
    private:

        // Get description of the provided rpc interface
        [[nodiscard]] nlohmann::json description() const
        {
            nlohmann::json desc {
                    {"jsonrpc", 2.0}
            };
            for (const auto &[name, method]: mMethods) {
                nlohmann::json methodJson;
                methodJson["name"] = name;

                for (const auto &param: method.params) {
                    nlohmann::json p;
                    p["name"] = param.name;
                    p["type"] = [&param]()
                    {
                        switch (param.type) {
                            case Parameter::Type::Boolean:
                                return "boolean";
                            case Parameter::Type::Integer:
                                return "integer";
                            case Parameter::Type::Real:
                                return "real";
                            case Parameter::Type::String:
                                return "string";
                            default:
                                return "integer";
                        }
                    }();
                    methodJson["params"].push_back(p);
                }

                desc["methods"].push_back(methodJson);
            }
            return desc;
        }

        std::map<std::string, Method> mMethods;
    };



}


#endif //RESERVOIRCONTROLLER_JSONRPC_H

















