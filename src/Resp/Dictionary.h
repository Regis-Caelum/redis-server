#pragma once

#include <unordered_map>
#include <string>

struct RespObject;

struct Dictionary
{
private:
    std::unordered_map<std::string, RespObject *> dictionary;

    Dictionary() {}

public:
    Dictionary(const Dictionary &) = delete;
    Dictionary &operator=(const Dictionary &) = delete;

    static Dictionary &getInstance()
    {
        static Dictionary instance;
        return instance;
    }

    ~Dictionary()
    {
        for (auto &pair : dictionary)
            delete pair.second;
    }

    void set(const std::string &key, RespObject *value)
    {
        auto it = dictionary.find(key);
        if (it != dictionary.end())
        {
            delete it->second;
        }
        dictionary[key] = value;
    }

    RespObject *get(const std::string &key) const
    {
        auto it = dictionary.find(key);
        if (it != dictionary.end())
        {
            return it->second;
        }
        return nullptr;
    }

    void deleteKey(const std::string &key)
    {
        auto it = dictionary.find(key);
        if (it != dictionary.end())
        {
            delete it->second;
            dictionary.erase(it);
        }
    }
};
