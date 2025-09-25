#include "AbstractCommand.h"

std::string_view AbstractCommand::name() const
{
    return m_name;
}

std::string_view AbstractCommand::error() const
{
    return m_err;
}

std::string_view AbstractCommand::response() const
{
    return m_resp;
}
