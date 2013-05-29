#include "config.hpp"

#include <boost/lexical_cast.hpp>
#include <libconfig.h++>

template <typename T>
void get_value(const libconfig::Setting& root, config_map_type& config,
    const std::string& key_name, const T& fallback_value)
{
    T value;
    if (root.lookupValue(key_name, value))
        config[key_name] = boost::lexical_cast<std::string>(value);
    else
        config[key_name] = boost::lexical_cast<std::string>(fallback_value);
}

void load_config(config_map_type& config, const std::string& config_path)
{
    libconfig::Config cfg;
    // Ignore error if unable to read config file.
    try
    {
        cfg.readFile(config_path.c_str());
    }
    catch (const libconfig::FileIOException&) {}
    catch (const libconfig::ParseException&) {}
    // Read off values
    const libconfig::Setting& root = cfg.getRoot();
    get_value<std::string>(root, config, "output-file", "debug.log");
    get_value<std::string>(root, config, "error-file", "error.log");
    get_value<std::string>(root, config, "database", "database");
    get_value(root, config, "block-publish-port", 5563);
    get_value(root, config, "tx-publish-port", 5564);
    get_value(root, config, "service-port", 9090);
    get_value<std::string>(root, config, "stop-secret", "");
}

