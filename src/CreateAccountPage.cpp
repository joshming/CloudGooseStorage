#include "CreateAccountPage.h"

#include <Wt/Dbo/Dbo.h>
#include <Wt/Dbo/Session.h>
#include <Wt/Dbo/Transaction.h>
#include <Wt/Dbo/ptr.h>
#include <Wt/WLabel.h>
#include <Wt/WLineEdit.h>
#include <Wt/WMessageBox.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include <memory>
#include <utility>
#include "FileViewPage.h"
#include "Folder.h"
#include "LoginPage.h"
#include "StorageApplication.h"
#include "User.h"

CreateAccountPage::CreateAccountPage(Wt::Dbo::Session& session)
    : m_databaseSession(&session)
{
    setStyleClass("login-page");
    // TODO: CSS

    auto* logo = addNew<Wt::WImage>("/CloudGooseStorageLogo.png");
    logo->addStyleClass("logo");

    // Add a header to differentiate this page from the login page
    auto* header = addNew<Wt::WText>("Create Account");
    header->addStyleClass("header");

    auto* usernameLabel = addNew<Wt::WLabel>("Username");
    auto* username = addNew<Wt::WLineEdit>();
    usernameLabel->setBuddy(username);

    auto* passwordLabel = addNew<Wt::WLabel>("Password");
    auto* password = addNew<Wt::WLineEdit>();
    password->setAttributeValue("type", "password");
    password->setAttributeValue("required", "");
    passwordLabel->setBuddy(password);

    auto* button = addNew<Wt::WPushButton>("Create Account");
    auto* messageBox = addNew<Wt::WText>();
    button->clicked().connect([this, username, password, messageBox] {
        if (password->text().empty()) {
            messageBox->setText("You must enter a password");
            return;
        }

        // TODO: Better error handling
        auto user = createAccount(username->text().toUTF8(), password->text().toUTF8());
        if (!user) {
            return;
        }
        auto* application = StorageApplication::instance();
        auto rootFolder = user->getRootFolder();
        application->switchPage(std::make_unique<FileViewPage>(user, *m_databaseSession, rootFolder));
    });

    // Workaround for https://redmine.emweb.be/issues/7645 on Wt < 4.10.1+
    auto* loginLink = addNew<Wt::WAnchor>(Wt::WLink("javascript:void(0);"));
    loginLink->setText("Already have an account?");
    loginLink->clicked().connect([this] {
        auto* application = StorageApplication::instance();
        application->switchPage(std::make_unique<LoginPage>(*m_databaseSession));
    });
}

Wt::Dbo::ptr<User> CreateAccountPage::createAccount(std::string username, const std::string& password)
{
    Wt::Dbo::Transaction transaction(*m_databaseSession);

    if (User::findByUsername(*m_databaseSession, username)) {
        addNew<Wt::WText>("Username is already taken.");
        return nullptr;
    }

    auto user = m_databaseSession->addNew<User>(username, password);
    const std::string rootFolderName = "~root";
    auto rootFolder = m_databaseSession->addNew<Folder>(rootFolderName, user, nullptr);

    rootFolder.flush();
    user.modify()->setRootFolder(rootFolder);

    return user;
}
