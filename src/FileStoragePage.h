/**
 * \class FileStoragePage
 *
 * The page where users can upload files.
 *
 * \authors Arjun Sharma, Connor Cummings, Joshua Nathan Ming, Raj Brahmbhatt
 * \date 2023-11-28 (last updated)
 */

#pragma once

#include <Wt/WContainerWidget.h>
#include <Wt/WFileUpload.h>
#include "Folder.h"
#include "User.h"

class FileStoragePage : public Wt::WContainerWidget {
private:
    Wt::Dbo::ptr<User> m_loggedInUser;
    Wt::Dbo::Session* m_databaseSession;
    Wt::Dbo::ptr<Folder> m_parentFolder;

public:
    /**
     * Creates a new `FileStoragePage`.
     *
     * \param loggedInUser The logged-in user, who will own all uploaded files.
     * \param session      The database session to use.
     * \param parentFolder The parent folder to which these files are being uploaded
     */
    explicit FileStoragePage(Wt::Dbo::ptr<User> loggedInUser, Wt::Dbo::Session& session, Wt::Dbo::ptr<Folder> parentFolder);

private:
    /**
     * Uploads a file selected by the user.
     *
     * In addition to storing the file metadata in the database, this function
     * stores the file content to the server's file system.
     *
     * \param fileUpload A file upload widget where the file to upload has been
     *                   selected.
     * \param fileName   The name to use for the uploaded file, or an empty string
     *                   to use the default.
     * \param parentFolder   The parent folder being passed to which the file will be uploaded
     * \return           The name of the uploaded file.
     */
    std::optional<std::string> uploadFile(Wt::WFileUpload* fileUpload, const std::string& fileName, Wt::Dbo::ptr<Folder> parentFolder);
};
