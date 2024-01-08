#include <Wt/Dbo/Exception.h>
#include <Wt/Dbo/backend/Sqlite3.h>
#include <Wt/WApplication.h>
#include <Wt/WConfig.h>
#include <Wt/WGlobal.h>
#include <Wt/WServer.h>
#include <csignal>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <memory>
#include "SharingLink.h"
#include "StorageApplication.h"
#include "User.h"

int main(int argc, char** argv)
{
    // Reference sources used:
    // https://webtoolkit.eu/wt/doc/reference/html/classWt_1_1WServer.html#details
    // https://github.com/emweb/wt/blob/c1abebc8c90e6cce24df77f2c9ed9dd330180549/src/http/WServer.C#L330

    // NOLINTBEGIN (cppcoreguidelines-pro-bounds-pointer-arithmetic)
    // Pointer arithmetic is necessary here to get the values out of argv.
    std::string applicationPath { argv[0] };
    std::vector<std::string> args { argv + 1, argv + argc };
    // NOLINTEND

    try {
        Wt::WServer server(applicationPath);
        server.setServerConfiguration(applicationPath, args, WTHTTP_CONFIGURATION);

        // Register sharing links as globally-accessible resources.
        {
            auto databaseSession = StorageApplication::createDatabaseSession();
            Wt::Dbo::Transaction transaction(*databaseSession);

            auto sharingLinks = databaseSession->find<SharingLink>().resultList();
            for (auto sharingLink : sharingLinks) {
                sharingLink->registerResource();
            }
        }

        server.addEntryPoint(Wt::EntryPointType::Application, [](const Wt::WEnvironment& env) {
            return std::make_unique<StorageApplication>(env);
        });
        if (server.start()) {
            int signal = Wt::WServer::waitForShutdown();

            std::cerr << "Server shutdown on signal " << signal << std::endl;
            server.stop();

            if (signal == SIGHUP) {
                Wt::WServer::restart(applicationPath, args);
            }
        }
    } catch (std::exception& ex) {
        std::cerr << "Exception: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
