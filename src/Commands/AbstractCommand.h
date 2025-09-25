#include <string>
#include <memory>
#include "../Resp/RespTypes.h"

class AbstractCommand
{
protected:
    const std::string_view m_name;
    std::string m_resp;
    std::string m_err;

public:
    AbstractCommand(std::string_view nameArg) : m_name(nameArg) {};
    std::string_view name() const;
    std::string_view error() const;
    std::string_view response() const;

    virtual void execute(std::unique_ptr<RespObject> cmdArgs) = 0;
};
