/**
 * \class FileViewPage
 *
 * A page where users can see their files and folders
 *
 * \authors Arjun Sharma, Joshua Nathan Ming, Matthew Lucas Otchet, Raj Brahmbhatt
 * \date 2023-11-28 (last updated)
 */

#pragma once

#include <Wt/WContainerWidget.h>
#include <Wt/WPushButton.h>
#include "File.h"
#include "FileWidget.h"
#include "User.h"

class FileViewPage : public Wt::WContainerWidget {
public:
    /**
     * The MIME type used for dragging and dropping files.
     *
     * Wt requires this to specify the type of data that is being dragged.
     */
    constexpr static std::string_view FILE_MIME_TYPE = "application/x.cloud-goose-storage.file";

    /**
     * Creates a new `FileViewPage`.
     *
     * \param user The logged-in user, who will see all their files and folders
     * \param session The database session to use.
     * \param parentFolder The current folder that the FileViewPage is on
     */
    explicit FileViewPage(const Wt::Dbo::ptr<User>& user, Wt::Dbo::Session& session, Wt::Dbo::ptr<Folder> parentFolder);

private:
    Wt::Dbo::Session* m_databaseSession;
    Wt::Dbo::ptr<User> m_user;
    Wt::Dbo::ptr<Folder> m_parentFolder;
    bool m_hasSorted { false };

    /**
     * Adds ths files into the file container to be viewed
     *
     * \param fileContainer The container that holds all the FileWidget Elements
     * \param files The sorted files
     */
    void addFiles(Wt::WContainerWidget* fileContainer, const Wt::Dbo::collection<Wt::Dbo::ptr<File>>& files) const;

    /**
     * Deletes a file from the database
     * \param name the name of the file to be deleted
     * \param fileContainer the file container object that needs to be redisplayed without the deleted file
     * \param fileWidget the widget that will be deleted in the file view page
     */
    void deleteFile(const std::string& name, Wt::WContainerWidget* fileContainer, FileWidget* fileWidget) const;

    /**
    * Filters files based on a query and displays them in the file container.
     *
     * \param fileContainer The container that holds all the FileWidget Elements
     * \param query The query string to filter files by name
     */
    void filterFiles(Wt::WContainerWidget* fileContainer, const std::string& query) const;

    /**
     * A button that links to the parent folder.
     *
     * This class is an implementation detail of FileViewPage, so that's why
     * it's a nested class. Coupling is not a concern here.
     */
    class ParentFolderButton : public Wt::WPushButton {
    public:
        /**
         * Creates a new "Parent Folder" button.
         *
         * \param page The FileViewPage that contains this button.
         */
        explicit ParentFolderButton(FileViewPage* page);

    protected:
        /**
         * Handles a drop event.
         *
         * This implementation will handle drop events from FileWidget by moving
         * the file to the page's parent folder.
         *
         * \param dropEvent The event to handle.
         */
        void dropEvent(Wt::WDropEvent dropEvent) override;

    private:
        FileViewPage* m_page;
    };
};
