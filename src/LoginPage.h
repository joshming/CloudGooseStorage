/**
 * \class LoginPage
 *
 * A page where the user can log in to an existing account.
 *
 * \authors Arjun Sharma, Connor Cummings, Joshua Nathan Ming
 * \date 2023-11-21 (last updated)
 */

#pragma once

#include <Wt/Dbo/Dbo.h>
#include <Wt/WContainerWidget.h>
#include "User.h"

class LoginPage : public Wt::WContainerWidget {

private:
    Wt::Dbo::Session* m_databaseSession;

public:
    /**
     * Creates a new login page.
     *
     * \param session The database session to use.
     */
    explicit LoginPage(Wt::Dbo::Session& session);

    /**
     * Logs into an account in the database.
     *
     * \param username The username of the account to log into.
     * \param password The password of the account to log into.
     * \return         The account that was logged into, or `nullptr` if the
     *                 account does not exist.
     */
    Wt::Dbo::ptr<User> login(const std::string& username, const std::string& password);
};
