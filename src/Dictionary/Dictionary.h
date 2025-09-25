#pragma once

#include <unordered_map>
#include <string>
#include <memory>

#include "../Resp/RespTypes.h"

class Dictionary
{
private:
    std::unordered_map<std::string_view, std::unique_ptr<RespObject>> dictionary;

    Dictionary() {}

public:
    Dictionary(const Dictionary &) = delete;
    Dictionary &operator=(const Dictionary &) = delete;

    static Dictionary &getInstance()
    {
        static Dictionary instance;
        return instance;
    }

    void set(std::string_view key, RespObject value)
    {
        dictionary[key] = std::make_unique<RespObject>(std::move(value));
    }

    const RespObject *get(std::string_view key) const
    {
        auto it = dictionary.find(key);
        if (it != dictionary.end())
        {
            return it->second.get();
        }
        return nullptr;
    }

    void deleteKey(std::string_view key)
    {
        auto it = dictionary.find(key);
        if (it != dictionary.end())
        {
            dictionary.erase(it);
        }
    }
};