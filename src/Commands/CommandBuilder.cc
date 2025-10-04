#include "CommandBuilder.h"

const std::unordered_map<std::string, CommandBuilder::CommandCreator> CommandBuilder::commandMap = {
    {"PING", createPingCommand},
    {"SET", createSetCommand},
    {"GET", createGetCommand},
    {"QUIT", createQuitCommand},
    {"EXISTS", createExistsCommand},
    {"DEL", createDeleteCommand},
    {"INCR", createIncrementCommand},
    {"DECR", createDecrementCommand},
};