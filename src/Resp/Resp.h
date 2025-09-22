#pragma once

#include <string>
#include <vector>

struct Dictionary;

enum RESP_TYPE
{
    STRING,
    INTEGER,
    DECIMAL,
    ARRAY,
    MAP
};

struct RespObject
{
    RESP_TYPE resp_type;
    union Value
    {
        long long int integer;
        std::string str;
        std::vector<RespObject> array;
        Dictionary *map;

        Value() {}
        ~Value() {}
    } value;

    RespObject(long long int i);
    RespObject(const std::string &s);
    RespObject(const std::vector<RespObject> &arr);
    RespObject(Dictionary *m);

    RespObject(const RespObject& other);
    RespObject& operator=(const RespObject& other);
    RespObject(RespObject&& other) noexcept;
    RespObject& operator=(RespObject&& other) noexcept;
    ~RespObject();

private:
    void copy(const RespObject& other);
    void move(RespObject&& other) noexcept;
};