#include "GetCommand.h"
#include "SetCommand.h"
#include "PingCommand.h"
#include "QuitCommand.h"

#include <unordered_map>
#include <memory>
#include <optional>
#include <vector>
#include <string>

class CommandBuilder
{
private:
    using CommandCreator = std::unique_ptr<AbstractCommand> (*)(const std::vector<RespObject> &);
    static const std::unordered_map<std::string, CommandCreator> commandMap;

    static std::unique_ptr<AbstractCommand> createPingCommand(const std::vector<RespObject> &args)
    {
        return std::make_unique<PingCommand>(args);
    }

    static std::unique_ptr<AbstractCommand> createSetCommand(const std::vector<RespObject> &args)
    {
        return std::make_unique<SetCommand>(args);
    }

    static std::unique_ptr<AbstractCommand> createGetCommand(const std::vector<RespObject> &args)
    {
        return std::make_unique<GetCommand>(args);
    }

    static std::unique_ptr<AbstractCommand> createQuitCommand(const std::vector<RespObject> &args)
    {
        return std::make_unique<QuitCommand>(args);
    }

public:
    static std::optional<std::unique_ptr<AbstractCommand>> createCommand(
        const std::string &commandName,
        const std::vector<RespObject> &args)
    {
        auto it = commandMap.find(commandName);
        if (it != commandMap.end())
        {
            return it->second(args);
        }
        return std::nullopt;
    }
};
