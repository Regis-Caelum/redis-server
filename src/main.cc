#include "Server/Server.h"
#include "Network/WindowsNetworkService.h"

int main()
{
    try
    {
        std::unique_ptr<WindowsNetworkService> windowsNetworkService = std::make_unique<WindowsNetworkService>();
        Server server(std::move(windowsNetworkService));
        server.listen_for_clients("0.0.0.0", 3000);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Server error: " << e.what() << "\n";
    }

    return 0;
}
