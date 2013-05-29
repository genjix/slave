#include "block_subscribe.hpp"

#include <bitcoin/bitcoin.hpp>
using namespace bc;

block_subscribe::block_subscribe(blockchain& chain)
  : chain_(chain), context_(1), socket_(context_, ZMQ_SUB)
{
}

void block_subscribe::start(config_map_type& config)
{
    std::string connect_addr = "tcp://*:" + config["block-publish-port"];
    int timeout = 1000;
    socket_.connect(connect_addr.c_str());
    socket_.setsockopt(ZMQ_SUBSCRIBE, "", 0);
    socket_.setsockopt(ZMQ_RCVTIMEO, &timeout, sizeof(timeout));
    thread_ = std::thread(std::bind(&block_subscribe::run, this));
}

void block_subscribe::stop()
{
    stopped_ = true;
    thread_.join();
}

data_chunk receive(zmq::socket_t& socket)
{
    zmq::message_t message;
    if (!socket.recv(&message))
        return data_chunk();
    uint8_t* begin = static_cast<uint8_t*>(message.data());
    uint8_t* end = begin + message.size();
    return data_chunk(begin, end);
}

void block_subscribe::run()
{
    while (!stopped_)
        update();
}

void import_finished(const std::error_code& ec)
{
    if (ec)
        log_error() << "Import block: " << ec.message();
}

void block_subscribe::update()
{
    data_chunk raw_depth = receive(socket_);
    if (raw_depth.empty())
        return;
    data_chunk raw_block = receive(socket_);
    BITCOIN_ASSERT(!raw_block.empty());
    uint32_t depth = cast_chunk<uint32_t>(raw_depth);
    block_type blk;
    try
    {
        satoshi_load(raw_block.begin(), raw_block.end(), blk);
    }
    catch (end_of_stream)
    {
        log_error() << "Problem parsing raw block ("
            << raw_block.size() << ")";
        return;
    }
    log_info() << "Block #" << depth << " " << hash_block_header(blk);
    chain_.import(blk, depth, import_finished);
}

