#ifndef QUERY_SERVICE_HPP
#define QUERY_SERVICE_HPP

#include <bitcoin/bitcoin.hpp>

#include "thrift/QueryService.h"
#include "config.hpp"
#include "sync_blockchain.hpp"

class query_service_handler
  : public QueryServiceIf
{
public:
    typedef std::function<void ()> stop_function_type;

    query_service_handler(config_map_type& config, bc::blockchain& chain);
    void initialize(stop_function_type stop_function);

    bool stop(const std::string& secret);
    // blockchain methods
    void block_header_by_depth(BlockHeader& blk, const int32_t depth);
    void block_header_by_hash(BlockHeader& blk, const std::string& hash);
    void block_transaction_hashes_by_depth(
        HashList& tx_hashes, const int32_t depth);
    void block_transaction_hashes_by_hash(
        HashList& tx_hashes, const std::string& hash);
    int32_t block_depth(const std::string& hash);
    int32_t last_depth();
    void transaction(Transaction& tx, const std::string& hash);
    void transaction_index(
        TransactionIndex& tx_index, const std::string& hash);
    void spend(InputPoint& inpoint, const OutputPoint& outpoint);
    void outputs(OutputPointList& outpoints, const std::string& address);

private:
    sync_blockchain chain_;
    const std::string stop_secret_;
    stop_function_type stop_function_;
};

void start_thrift_server(config_map_type& config, bc::blockchain& chain);

#endif

