#include <future>
#include <bitcoin/bitcoin.hpp>

#include "block_subscribe.hpp"
#include "echo.hpp"
#include "service.hpp"

bool init(bc::leveldb_blockchain& chain, const std::string& dbpath)
{
    std::promise<std::error_code> ec_promise;
    // completion handler for start.
    auto blockchain_started = 
        [&ec_promise](const std::error_code& ec)
        {
            ec_promise.set_value(ec);
        };
    chain.start(dbpath, blockchain_started);
    // wait until std::error_code is set by
    // start completion handler.
    std::error_code ec = ec_promise.get_future().get();
    if (ec)
    {
        bc::log_fatal() << "Blockchain init failed: " << ec.message();
        return false;
    }
    return true;
}

int main(int argc, char** argv)
{
    config_map_type config;
    load_config(config, "query.cfg");
    bc::threadpool pool(4);
    bc::leveldb_blockchain chain(pool);
    if (!init(chain, config["database"]))
        return 1;
    block_subscribe sub(chain);
    sub.start(config);
    echo() << "Server starting...";
    start_thrift_server(config, chain);
    echo() << "Server stopped.";
    return 0;
}

