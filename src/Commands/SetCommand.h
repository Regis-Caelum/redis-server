#include "AbstractCommand.h"

class SetCommand : public AbstractCommand
{
public:
    SetCommand(const std::vector<RespObject> &args);
    void execute() override;
};