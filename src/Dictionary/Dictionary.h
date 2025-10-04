#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include <optional>
#include <chrono>

#include "../Resp/RespTypes.h"

struct Entry
{
    std::unique_ptr<RespObject> respObj;
    std::optional<std::chrono::steady_clock::time_point> expiry;

    Entry() = default;
    Entry(const Entry& other)
        : respObj(other.respObj ? std::make_unique<RespObject>(*other.respObj) : nullptr),
          expiry(other.expiry){}

    Entry(std::unique_ptr<RespObject> resp, std::optional<std::chrono::steady_clock::time_point> expiry)
        : respObj(std::move(resp)), expiry(expiry) {}

    Entry(Entry&&) noexcept = default;
    Entry& operator=(Entry&&) noexcept = default;
    Entry& operator=(const Entry&) = delete;
};

class Dictionary
{
private:
    std::unordered_map<std::string, std::unique_ptr<Entry>> dictionary;

    Dictionary() {}

public:
    Dictionary(const Dictionary &) = delete;
    Dictionary &operator=(const Dictionary &) = delete;

    static Dictionary &getInstance()
    {
        static Dictionary instance;
        return instance;
    }

    void set(std::string key, RespObject value)
    {
        auto entry = std::make_unique<Entry>();
        entry->respObj = std::make_unique<RespObject>(std::move(value));
        entry->expiry = std::nullopt;
        dictionary[key] = std::move(entry);
    }

    void setWithExpiry(std::string key, RespObject value, std::chrono::seconds ttl)
    {
        auto entry = std::make_unique<Entry>(
            std::make_unique<RespObject>(std::move(value)),
            std::chrono::steady_clock::now() + ttl);
        dictionary[key] = std::move(entry);
    }

    std::optional<Entry> get(const std::string &key) const
    {
        auto it = dictionary.find(key);
        if (it == dictionary.end() || !it->second)
            return std::nullopt;

        auto &entry = *(it->second);
        if (entry.expiry && std::chrono::steady_clock::now() > *entry.expiry)
            return std::nullopt;

        return Entry{std::make_unique<RespObject>(*entry.respObj), entry.expiry};
    }

    void deleteKey(std::string key)
    {
        dictionary.erase(key);
    }

    void clear()
    {
        dictionary.clear();
    }
};