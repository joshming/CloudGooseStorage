/**
 * \class FileWidget
 *
 * A WContainerWidget representing a File in the home page.
 *
 * \authors Arjun Sharma, Connor Cummings, Joshua Nathan Ming, Matthew Otchet, Raj Brahmbhatt
 * \date 2023-11-28 (last updated)
 */

#pragma once

#include <Wt/WAnchor.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WGlobal.h>
#include <Wt/WResource.h>
#include <string>
#include "File.h"
#include "Folder.h"
#include "User.h"

class FileWidget : public Wt::WAnchor {
public:
    /**
     * Creates a new `FileWidget`.
     *
     * \param user The logged-in user, who will see all their files and folders
     * \param session The database session to use.
     * \param file the current file of the widget
     * \param parentFolder the folder this file is stored in
     */
    explicit FileWidget(Wt::Dbo::ptr<User> user, Wt::Dbo::Session& session, const Wt::Dbo::ptr<File>& file, Wt::Dbo::ptr<Folder> parentFolder);

    /**
     * Getting the signal to delete file: used by the file view page to display deleted files
     *
     * @return signal to delete file
     */
    Wt::Signal<>& deleteFile() { return m_deleteFile; }

    /**
     * Getting the signal when file is moved: used by the file view page to display the moved files
     *
     * @return signal to delete file
     */
    Wt::Signal<>& moveFile() { return m_moveFile; }

    /**
     * Moves the file of this 'FileWidget' to the specified folder.
     *
     * This requires a Wt::Dbo::Transaction to be currently active.
     *
     * \param folder The destination folder.
     * \exception std::runtime_error If there was a problem moving the file.
     */
    void moveFile(Wt::Dbo::ptr<Folder> folder);

private:
    Wt::Dbo::Session* m_databaseSession;
    Wt::Dbo::ptr<User> m_user;
    Wt::Dbo::ptr<File> m_file;
    Wt::Dbo::ptr<Folder> m_parentFolder;
    Wt::Signal<> m_deleteFile;
    Wt::Signal<> m_moveFile;

    /**
     * moves the file of this 'FileWidget' to the specified foler
     *
     * \param name the name of the folder
     * \param moveBox the dialog box shown when a user wants to move a file
     * \param dialogText the text to displayed in the dialog box
     */
    void moveFile(const std::string& name, Wt::WDialog& moveBox, Wt::WText* dialogText);

    /**
     * renames the file of this 'FileWidget' to the specified name
     *
     * \param name the new name of the file
     * \param fileName the displayed name of the file on the file-widget itself
     * \param renameBox the dialog box shown when a user wants to move a file
     * \param dialogText the text to displayed in the dialog box
     */
    void renameFile(const std::string& name, Wt::WText& fileName, Wt::WDialog& renameBox, Wt::WText* dialogText);
};
