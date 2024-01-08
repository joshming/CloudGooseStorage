#include "StorageApplication.h"

#include <Wt/Dbo/Session.h>
#include <Wt/Dbo/backend/Sqlite3.h>
#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WGlobal.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include <cstdlib>
#include <memory>
#include "File.h"
#include "FileViewPage.h"
#include "Folder.h"
#include "LoginPage.h"
#include "User.h"

constexpr const char* USERS_TABLE_EXISTS_QUERY = "SELECT EXISTS(SELECT 1 FROM sqlite_master WHERE type = 'table' AND name = 'users')";

StorageApplication::StorageApplication(const Wt::WEnvironment& env)
    : Wt::WApplication(env)
    , m_databaseSession(createDatabaseSession())
{

    setTitle("Cloud Goose Storage");

    // This references the cloud-goose-storage.css file in the docroot directory.
    useStyleSheet(Wt::WLink("/cloud-goose-storage.css"));
    root()->addStyleClass("root");

    auto loginPage = std::make_unique<LoginPage>(*m_databaseSession);
    switchPage(std::move(loginPage));
}

StorageApplication* StorageApplication::instance()
{
    return dynamic_cast<StorageApplication*>(Wt::WApplication::instance());
}

std::unique_ptr<Wt::Dbo::Session> StorageApplication::createDatabaseSession()
{
    auto databaseConnection = std::make_unique<Wt::Dbo::backend::Sqlite3>("CloudGooseStorage.db");
    auto databaseSession = std::make_unique<Wt::Dbo::Session>();
    databaseSession->setConnection(std::move(databaseConnection));

    databaseSession->mapClass<File>("files");
    databaseSession->mapClass<Folder>("folders");
    databaseSession->mapClass<SharingLink>("sharing_links");
    databaseSession->mapClass<User>("users");

    // Check if the users table exists. If it does, we assume that all the
    // tables are correct.
    bool usersTableExists = false;
    {
        Wt::Dbo::Transaction transaction(*databaseSession);
        usersTableExists = databaseSession->query<bool>(USERS_TABLE_EXISTS_QUERY);
    }
    if (!usersTableExists) {
        databaseSession->createTables();
    }

    return databaseSession;
}

void StorageApplication::switchPage(std::unique_ptr<Wt::WWidget> newPage)
{
    root()->clear();
    root()->addWidget(std::move(newPage));
}
