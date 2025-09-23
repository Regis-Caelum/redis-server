#pragma once

#include <unordered_map>
#include <string>
#include <memory>

struct RespObject;

struct Dictionary
{
private:
    std::unordered_map<std::string, std::unique_ptr<RespObject>> dictionary;

    Dictionary() {}

public:
    Dictionary(const Dictionary &) = delete;
    Dictionary &operator=(const Dictionary &) = delete;

    static Dictionary &getInstance()
    {
        static Dictionary instance;
        return instance;
    }

    void set(const std::string &key, RespObject *value)
    {
        dictionary[key] = std::unique_ptr<RespObject>(value);
    }

    RespObject *get(const std::string &key) const
    {
        auto it = dictionary.find(key);
        if (it != dictionary.end())
        {
            return it->second.get();
        }
        return nullptr;
    }

    void deleteKey(const std::string &key)
    {
        auto it = dictionary.find(key);
        if (it != dictionary.end())
        {
            dictionary.erase(it);
        }
    }
};