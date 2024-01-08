#include "FolderStoragePage.h"

#include <Wt/Dbo/Transaction.h>
#include <Wt/WBreak.h>
#include <Wt/WGlobal.h>
#include <Wt/WLabel.h>
#include <Wt/WLineEdit.h>
#include <Wt/WMessageBox.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include <fstream>
#include <optional>
#include "FileViewPage.h"
#include "Folder.h"
#include "StorageApplication.h"

FolderStoragePage::FolderStoragePage(Wt::Dbo::ptr<User> user, Wt::Dbo::Session& session, Wt::Dbo::ptr<Folder> parentFolder)
    : m_loggedInUser(std::move(user))
    , m_databaseSession(&session)
    , m_parentFolder(std::move(parentFolder))
{
    auto* homeButton = addNew<Wt::WPushButton>("Home");
    homeButton->addStyleClass("back-button");

    homeButton->clicked().connect([this] {
        auto* application = StorageApplication::instance();
        application->switchPage(std::make_unique<FileViewPage>(m_loggedInUser, *m_databaseSession, m_parentFolder));
    });

    setStyleClass("file-storage-page");
    // TODO: better css

    auto* folderNameLabel = addNew<Wt::WLabel>("Would you like to name your folder? "); // needs styling
    auto* folderNameInput = addNew<Wt::WLineEdit>();

    folderNameLabel->setBuddy(folderNameInput);

    auto* uploadButton = addNew<Wt::WPushButton>("Add Folder!");

    uploadButton->clicked().connect([this, folderNameInput] {
        Wt::Dbo::Transaction transaction(*m_databaseSession);
        auto folderNameString = folderNameInput->text().toUTF8();
        bool isDuplicateName = false;

        for (const auto& folder : m_parentFolder->getFolders()) {
            if (folder->getName() == folderNameString) {
                isDuplicateName = true;
                break;
            }
        }

        if (isDuplicateName || folderNameString.empty()) {
            auto* messageBox = addChild(std::make_unique<Wt::WMessageBox>(
                "Folder cannot be added",
                isDuplicateName ? "<p>There already exists a folder with that name.</p>" : "<p>You must enter a fileName.</p>"
                                                                                           "<p>Please try again</p>",
                Wt::Icon::Information,
                Wt::StandardButton::Ok));
            messageBox->setModal(false);
            messageBox->buttonClicked().connect([this, messageBox] {
                removeChild(messageBox);
            });
            messageBox->show();
        } else {
            auto* messageBox = addChild(std::make_unique<Wt::WMessageBox>(
                "Folder added as: " + folderNameString,
                "Press home to go back or you can add more folders.",
                Wt::Icon::Information,
                Wt::StandardButton::Ok));
            messageBox->setModal(false);
            messageBox->buttonClicked().connect([this, messageBox] {
                removeChild(messageBox);
            });
            messageBox->show();

            m_databaseSession->addNew<Folder>(std::move(folderNameString), m_loggedInUser, m_parentFolder);
            m_databaseSession->flush();
        }
    });
}
