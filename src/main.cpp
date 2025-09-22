#include "Server/Server.h"

int main()
{
    try
    {
        Server server;
        server.listen();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Server error: " << e.what() << "\n";
    }

    return 0;
}
