#ifndef PROTO_HPP_
#define PROTO_HPP_

#include <string>
#include <ctime>
#include <vector>

namespace proto
{    
    namespace client
    {
        class Req
        {
        public:

            Req(std::time_t start_time, std::time_t end_time): 
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

        std::string serialize(const Req& req)
        {
            std::string res;
            return res;
        }

        Req deserialize(const std::string& request)
        {
            std::time_t start_time, end_time;
            return Req(start_time, end_time);
        }
    }

    namespace server
    {
        class Resp
        {
        public:
            Resp(std::vector<int>&& data): 
                data_(std::move(data))
            {}

        private:
            std::vector<int> data_;
        };

        std::string serialize(Resp& resp)
        {
            std::string res;
            return res;
        }

        Resp deserialize(const std::string& response)
        {
            std::vector<int> dt;
            return Resp(std::move(dt));
        }
    }
}

#endif //PROTO_HPP_