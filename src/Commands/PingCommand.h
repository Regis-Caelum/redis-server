#include "AbstractCommand.h"

class PingCommand : public AbstractCommand
{
public:
    PingCommand();
    PingCommand(const std::vector<RespObject> &args);
    void execute(std::unique_ptr<RespObject> cmdArgs) override;
};