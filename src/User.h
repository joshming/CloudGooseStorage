/**
 * \class User
 *
 * A user account with a cloud storage drive.
 *
 * \authors Arjun Sharma, Connor Cummings, Joshua Nathan Ming
 * \date 2023-11-24 (last updated)
 */

#pragma once

#include <Wt/Auth/HashFunction.h>
#include <Wt/Dbo/Dbo.h>
#include <array>
#include <cstdint>
#include <string>
#include <utility>

class Folder;

class User {
private:
    Wt::Auth::BCryptHashFunction m_hashFunction;

    std::string m_username;
    std::string m_passwordHash;
    Wt::Dbo::ptr<Folder> m_rootFolder;

public:
    /**
     * Creates a new user.
     *
     * \param username The username of the new user.
     * \param password The password of the new user.
     */
    User(std::string username, const std::string& password);

    /**
     * Creates a new user with default values for all metadata.
     *
     * This should never be used directly by application code, but it is
     * required by `Wt::Dbo`.
     */
    [[deprecated("only for use by Wt::Dbo")]] User() = default;

    /**
     * Looks up a user by their username.
     *
     * \param databaseSession The database session to use.
     * \param username        The username to look up.
     * \return                The found user, or `nullptr` if the user was not
     *                        found.
     */
    static Wt::Dbo::ptr<User> findByUsername(Wt::Dbo::Session& databaseSession, const std::string& username);

    /**
     * Gets the username of this user.
     *
     * \return A string containing the username.
     */
    const std::string& getUsername() const { return m_username; }

    /**
     * Gets this user's root folder.
     *
     * \return The root folder.
     */
    Wt::Dbo::ptr<Folder> getRootFolder() const { return m_rootFolder; }

    /**
     * Sets this user's root folder.
     *
     * This must be called as soon as possible after creating a new user.
     * Otherwise, `getRootFolder` will return `nullptr`, which other code may
     * not be expecting.
     *
     * \param rootFolder The new root folder for this user.
     */
    void setRootFolder(Wt::Dbo::ptr<Folder> rootFolder) { m_rootFolder = std::move(rootFolder); }

    /**
     * Checks if the given password is correct for this user.
     *
     * \param password The password to check.
     * \return `true` if the password is correct, or `false` otherwise.
     */
    bool isPasswordCorrect(const std::string& password) const;

    /**
     * Changes this user's password.
     *
     * \param password The new password for this user.
     */
    void setPassword(const std::string& password);

    /**
     * Persists changes to the database.
     *
     * This should never be used directly by application code, but it is
     * required by `Wt::Dbo`.
     *
     * \param action The database action to perform.
     */
    template <class Action>
    void persist(Action& action)
    {
        Wt::Dbo::field(action, m_username, "username");
        Wt::Dbo::field(action, m_passwordHash, "password_hash");
        Wt::Dbo::belongsTo(action, m_rootFolder, "root_folder"); // required to have a root_folder_id column in the users table
    }
};
