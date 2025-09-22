#include "Resp.h"
#include "Dictionary.h" // We can include Dictionary.h here since it's a .cpp file.
#include <new>
#include <utility>

// Full implementation of constructors
RespObject::RespObject(long long int i) : resp_type(RESP_TYPE::INTEGER)
{
    value.integer = i;
}

RespObject::RespObject(const std::string &s) : resp_type(RESP_TYPE::STRING)
{
    new (&value.str) std::string(s);
}

RespObject::RespObject(const std::vector<RespObject> &arr) : resp_type(RESP_TYPE::ARRAY)
{
    new (&value.array) std::vector<RespObject>(arr);
}

RespObject::RespObject(Dictionary *m) : resp_type(RESP_TYPE::MAP)
{
    value.map = m;
}

void RespObject::copy(const RespObject &other)
{
    resp_type = other.resp_type;
    switch (resp_type)
    {
    case RESP_TYPE::STRING:
        new (&value.str) std::string(other.value.str);
        break;
    case RESP_TYPE::ARRAY:
        new (&value.array) std::vector<RespObject>(other.value.array);
        break;
    case RESP_TYPE::MAP:
        value.map = other.value.map;
        break;
    default:
        value.integer = other.value.integer;
        break;
    }
}

void RespObject::move(RespObject &&other) noexcept
{
    resp_type = other.resp_type;
    switch (resp_type)
    {
    case RESP_TYPE::STRING:
        new (&value.str) std::string(std::move(other.value.str));
        break;
    case RESP_TYPE::ARRAY:
        new (&value.array) std::vector<RespObject>(std::move(other.value.array));
        break;
    case RESP_TYPE::MAP:
        value.map = other.value.map;
        break;
    default:
        value.integer = other.value.integer;
        break;
    }
    other.resp_type = RESP_TYPE::INTEGER;
}

RespObject::RespObject(const RespObject &other) { copy(other); }
RespObject &RespObject::operator=(const RespObject &other)
{
    if (this != &other)
    {
        this->~RespObject();
        copy(other);
    }
    return *this;
}

RespObject::RespObject(RespObject &&other) noexcept { move(std::move(other)); }
RespObject &RespObject::operator=(RespObject &&other) noexcept
{
    if (this != &other)
    {
        this->~RespObject();
        move(std::move(other));
    }
    return *this;
}

// Full implementation of destructor
RespObject::~RespObject()
{
    switch (resp_type)
    {
    case RESP_TYPE::STRING:
        value.str.~basic_string();
        break;
    case RESP_TYPE::ARRAY:
        value.array.~vector();
        break;
    case RESP_TYPE::MAP:
        break;
    case RESP_TYPE::INTEGER:
    default:
        break;
    }
}