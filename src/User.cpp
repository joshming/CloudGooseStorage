#include "User.h"

#include <Wt/Dbo/Dbo.h>
#include <Wt/Dbo/Transaction.h>
#include <memory>
#include <stdexcept>
#include "Folder.h"

User::User(std::string username, const std::string& password)
    : m_username(std::move(username))
    , m_passwordHash(m_hashFunction.compute(password, ""))
{
}

Wt::Dbo::ptr<User> User::findByUsername(Wt::Dbo::Session& databaseSession, const std::string& username)
{
    auto userQuery = databaseSession.find<User>().where("username = ?").bind(username);
    Wt::Dbo::ptr<User> user = userQuery.resultValue();

    return user;
}

bool User::isPasswordCorrect(const std::string& password) const
{
    return m_hashFunction.verify(password, "", m_passwordHash);
}

void User::setPassword(const std::string& password)
{
    m_passwordHash = m_hashFunction.compute(password, "");
}
