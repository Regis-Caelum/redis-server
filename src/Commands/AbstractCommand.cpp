#include "AbstractCommand.h"
#include "../Resp/Resp.h"

AbstractCommand::AbstractCommand(const std::vector<RespObject> &arguments) : args(arguments) {}

AbstractCommand::~AbstractCommand() = default;