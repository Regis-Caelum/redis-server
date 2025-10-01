#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include <optional>
#include <chrono>

#include "../Resp/RespTypes.h"

class Dictionary
{
private:
    struct Entry
    {
        std::unique_ptr<RespObject> value;
        std::optional<std::chrono::steady_clock::time_point> expiry;
    };

    std::unordered_map<std::string, std::unique_ptr<Entry>> dictionary;

    Dictionary() {}

public:
    inline Dictionary(const Dictionary &) = delete;
    inline Dictionary &operator=(const Dictionary &) = delete;

    inline static Dictionary &getInstance()
    {
        static Dictionary instance;
        return instance;
    }

    inline void set(std::string key, RespObject value)
    {
        auto entry = std::make_unique<Entry>();
        entry->value = std::make_unique<RespObject>(std::move(value));
        entry->expiry = std::nullopt;
        dictionary[key] = std::move(entry);
    }

    inline void setWithExpiry(std::string key, RespObject value, std::chrono::seconds ttl)
    {
        auto entry = std::make_unique<Entry>();
        entry->value = std::make_unique<RespObject>(std::move(value));
        entry->expiry = std::chrono::steady_clock::now() + ttl;
        dictionary[key] = std::move(entry);
    }

    inline const std::optional<RespObject> get(std::string key) const
    {
        auto it = dictionary.find(key);
        if (it != dictionary.end())
        {
            if (it->second->expiry.has_value() && std::chrono::steady_clock::now() > it->second->expiry.value())
            {
                return std::nullopt;
            }
            return *(it->second)->value;
        }
        return std::nullopt;
    }

    inline void deleteKey(std::string key)
    {
        auto it = dictionary.find(key);
        if (it != dictionary.end())
        {
            dictionary.erase(it);
        }
    }
};