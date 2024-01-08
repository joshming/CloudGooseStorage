/**
 * \class CreateAccountPage
 *
 * A page in the application that allows the user to create a new account.
 *
 * \authors Arjun Sharma, Connor Cummings, Joshua Nathan Ming
 * \date 2023-11-21 (last updated)
 */

#pragma once

#include <Wt/Dbo/Dbo.h>
#include <Wt/WContainerWidget.h>
#include "User.h"

class CreateAccountPage : public Wt::WContainerWidget {
private:
    Wt::Dbo::Session* m_databaseSession;

public:
    /**
     * Creates a new `CreateAccountPage`.
     *
     * \param session The database session to use.
     */
    explicit CreateAccountPage(Wt::Dbo::Session& session);

    /**
     * Creates an account in the database.
     *
     * \param username The username of the new account.
     * \param password The password of the new account.
     * \return         The newly-created account.
     */
    Wt::Dbo::ptr<User> createAccount(std::string username, const std::string& password);
};
