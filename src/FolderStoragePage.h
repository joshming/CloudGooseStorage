/**
 * \class FolderStoragePage
 *
 * A page where users can add folders
 *
 * \authors Raj Brahmbhatt
 * \date 2023-11-28 (last updated)
 */

#pragma once

#include <Wt/WContainerWidget.h>
#include "Folder.h"
#include "User.h"

class FolderStoragePage : public Wt::WContainerWidget {
private:
    /**
     * The folder in the real filesystem where all user folders will be stored.
     */
    constexpr static const std::string_view FILE_SYSTEM_ROOT = "./userFiles/";

    Wt::Dbo::ptr<User> m_loggedInUser;
    Wt::Dbo::Session* m_databaseSession;
    Wt::Dbo::ptr<Folder> m_parentFolder;

public:
    /**
     * Creates a new `FolderStoragePage`.
     *
     * \param loggedInUser The logged-in user, who will own all uploaded folders.
     * \param session      The database session to use.
     * \param parentFolder The parent folder to use.
     */
    explicit FolderStoragePage(Wt::Dbo::ptr<User> loggedInUser, Wt::Dbo::Session& session, Wt::Dbo::ptr<Folder> parentFolder);
};
