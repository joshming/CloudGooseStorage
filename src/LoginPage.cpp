#include "LoginPage.h"

#include <Wt/Dbo/Dbo.h>
#include <Wt/Dbo/Session.h>
#include <Wt/Dbo/Transaction.h>
#include <Wt/Dbo/ptr.h>
#include <Wt/WAnchor.h>
#include <Wt/WLabel.h>
#include <Wt/WLineEdit.h>
#include <Wt/WMessageBox.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include <memory>
#include <utility>
#include "CreateAccountPage.h"
#include "FileViewPage.h"
#include "StorageApplication.h"
#include "User.h"

LoginPage::LoginPage(Wt::Dbo::Session& session)
    : m_databaseSession(&session)
{
    setStyleClass("login-page");
    // TODO: CSS

    auto* logo = addNew<Wt::WImage>("/CloudGooseStorageLogo.png");
    logo->addStyleClass("logo");

    // Add a header to differentiate this page from the create account page
    auto* header = addNew<Wt::WText>("Log In");
    header->addStyleClass("header");

    auto* usernameLabel = addNew<Wt::WLabel>("Username");
    auto* username = addNew<Wt::WLineEdit>();
    usernameLabel->setBuddy(username);

    auto* passwordLabel = addNew<Wt::WLabel>("Password");
    auto* password = addNew<Wt::WLineEdit>();
    password->setAttributeValue("type", "password");
    passwordLabel->setBuddy(password);

    auto* button = addNew<Wt::WPushButton>("Log In");
    auto* messageBox = addNew<Wt::WText>();
    button->clicked().connect([this, username, password, messageBox, &session] {
        auto user = login(username->text().toUTF8(), password->text().toUTF8());
        if (!user) {
            messageBox->setText("Invalid Credentials");
            return;
        }

        auto* application = StorageApplication::instance();
        auto rootFolder = user->getRootFolder();
        application->switchPage(std::make_unique<FileViewPage>(user, session, rootFolder));
    });

    // Workaround for https://redmine.emweb.be/issues/7645 on Wt < 4.10.1
    auto* createAccountLink = addNew<Wt::WAnchor>(Wt::WLink("javascript:void(0);"));
    createAccountLink->setText("Don't have an account?");
    createAccountLink->clicked().connect([this] {
        auto* application = StorageApplication::instance();
        application->switchPage(std::make_unique<CreateAccountPage>(*m_databaseSession));
    });
}

Wt::Dbo::ptr<User> LoginPage::login(const std::string& username, const std::string& password)
{
    Wt::Dbo::Transaction transaction(*m_databaseSession);

    auto user = User::findByUsername(*m_databaseSession, username);
    if (!user || !user->isPasswordCorrect(password)) {
        return nullptr;
    }

    return user;
}
