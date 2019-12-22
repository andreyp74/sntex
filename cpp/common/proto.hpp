#ifndef PROTO_HPP_
#define PROTO_HPP_

#include <string>
#include <ctime>
#include <vector>

namespace proto
{    
    class ClientReq
    {
    public:

        ClientReq(std::time_t start_time, std::time_t end_time): 
            start_time_(start_time),
            end_time_(end_time)
        {}

        bool done() const {
            return start_time_ == 0 && end_time_ == 0;
        }

        std::time_t start_time() const
        {
            return start_time_;
        }

        std::time_t end_time() const
        {
            return end_time_;
        }

    private:
        std::time_t start_time_;
        std::time_t end_time_;
    };

    class ServerResp
    {
    public:
        ServerResp(std::vector<int>&& data): 
            data_(std::move(data))
        {}

    private:
        std::vector<int> data_;
    };

    std::string serialize_req(ClientReq& client_req)
    {
        std::string res;
        return res;
    }

    std::string serialize_resp(ServerResp& server_resp)
    {
        std::string res;
        return res;
    }

    ClientReq deserialize_req(const std::string& request)
    {
        std::time_t start_time, end_time;
        ClientReq client_req(start_time, end_time);
        return client_req;
    }

    ServerResp deserialize_resp(const std::string& response)
    {
        std::vector<int> dt;
        ServerResp server_resp(std::move(dt));
        return server_resp;
    }
}

#endif //PROTO_HPP_