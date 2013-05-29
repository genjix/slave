#ifndef QUERY_SUBSCRIBER_HPP
#define QUERY_SUBSCRIBER_HPP

#include <thread>
#include <bitcoin/bitcoin.hpp>
#include <zmq.hpp>

#include "config.hpp"

class block_subscribe
{
public:
    block_subscribe(bc::blockchain& chain);
    void start(config_map_type& config);
    void stop();

private:
    void run();
    void update();

    bc::blockchain& chain_;

    zmq::context_t context_;
    zmq::socket_t socket_;

    std::thread thread_;
    bool stopped_ = false;
};

#endif

