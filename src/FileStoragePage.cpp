/**
 * \class FileStoragePage
 *
 * The page where users can upload files.
 *
 * \authors Connor Cummings, Joshua Nathan Ming
 * \date 2023-11-25 (last updated)
 */

#include "FileStoragePage.h"

#include <Wt/Dbo/Transaction.h>
#include <Wt/WBreak.h>
#include <Wt/WGlobal.h>
#include <Wt/WLabel.h>
#include <Wt/WLineEdit.h>
#include <Wt/WMessageBox.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include <filesystem>
#include <fstream>
#include <optional>
#include "FileViewPage.h"
#include "Folder.h"
#include "StorageApplication.h"

FileStoragePage::FileStoragePage(Wt::Dbo::ptr<User> user, Wt::Dbo::Session& session, Wt::Dbo::ptr<Folder> parentFolder)
    : m_loggedInUser(std::move(user))
    , m_databaseSession(&session)
    , m_parentFolder(std::move(parentFolder))
{

    addNew<Wt::WText>("File Upload")->addStyleClass("header");
    auto* homeButton = addNew<Wt::WPushButton>("Home");
    homeButton->addStyleClass("button");

    homeButton->clicked().connect([this] {
        auto* application = StorageApplication::instance();
        application->switchPage(std::make_unique<FileViewPage>(m_loggedInUser, *m_databaseSession, m_parentFolder));
    });

    setStyleClass("file-storage-page");

    addNew<Wt::WLabel>("Please select a file");
    auto* fileUpload = addNew<Wt::WFileUpload>();
    fileUpload->addStyleClass("file-upload");

    auto* filenameLabel = addNew<Wt::WLabel>("Would you like to name your file? ");
    auto* filenameInput = addNew<Wt::WLineEdit>();
    filenameLabel->setBuddy(filenameInput);

    auto* uploadButton = addNew<Wt::WPushButton>("Upload!");

    uploadButton->clicked().connect(fileUpload, &Wt::WFileUpload::upload);
    fileUpload->uploaded().connect([this, fileUpload, uploadButton, filenameInput] {
        uploadButton->disable();
        auto filename = FileStoragePage::uploadFile(fileUpload, filenameInput->text().toUTF8(), m_parentFolder);
        if (!(fileUpload->empty()) && filename) {
            auto* messageBox = addChild(std::make_unique<Wt::WMessageBox>(
                "File uploaded as: " + *filename,
                "Press home to view file or you can upload more files",
                Wt::Icon::Information,
                Wt::StandardButton::Ok));

            filenameInput->setText("");

            messageBox->setModal(false);
            messageBox->buttonClicked().connect([this, messageBox, &uploadButton] {
                uploadButton->enable();
                removeChild(messageBox);
            });
            messageBox->show();
        } else {
            if (fileUpload->empty()) {
                auto* messageBox = addChild(std::make_unique<Wt::WMessageBox>(
                    "No file selected",
                    "<p>Please select a file prior to upload.</p>",
                    Wt::Icon::Information,
                    Wt::StandardButton::Ok));
                messageBox->setModal(false);
                messageBox->buttonClicked().connect([this, messageBox, &uploadButton] {
                    uploadButton->enable();
                    removeChild(messageBox);
                });
                messageBox->show();
            } else if (!filename) {
                auto* messageBox = addChild(std::make_unique<Wt::WMessageBox>(
                    "File couldn't be uploaded",
                    "<p>There already existed a file with that name.</p>"
                    "<p>Please Try again</p>",
                    Wt::Icon::Information,
                    Wt::StandardButton::Ok));
                messageBox->setModal(false);
                messageBox->buttonClicked().connect([this, messageBox, &uploadButton] {
                    uploadButton->enable();
                    removeChild(messageBox);
                });
                messageBox->show();
            }
        }
    });

    fileUpload->fileTooLarge().connect([this] {
        auto* messageBox = addChild(std::make_unique<Wt::WMessageBox>(
            "File couldn't be uploaded",
            "<p>Your file is too large.</p>"
            "<p>Please Try again</p>",
            Wt::Icon::Warning,
            Wt::StandardButton::Ok));
        messageBox->setModal(false);
        messageBox->buttonClicked().connect([this, messageBox] {
            removeChild(messageBox);
        });
        messageBox->show();
    });

    fileUpload->changed().connect([uploadButton, fileUpload] {
        if (!fileUpload->canUpload()) {
            return;
        }
        uploadButton->enable();
    });
}

std::optional<std::string> FileStoragePage::uploadFile(Wt::WFileUpload* fileUpload, const std::string& fileName, Wt::Dbo::ptr<Folder> m_parentFolder)
{
    Wt::Dbo::Transaction transaction(*m_databaseSession);

    const std::string tempFileName = fileUpload->spoolFileName(); // The uploaded filename
    const std::string defaultName = fileUpload->clientFileName().toUTF8();
    const size_t fileExtensionPosition = defaultName.find_last_of('.');
    std::string name = (fileName.empty() ? defaultName : fileName);

    if (fileUpload->empty()) {
        return "Error";
    }

    if (fileExtensionPosition != std::string::npos) {
        name += (fileName.empty() ? "" : defaultName.substr(fileExtensionPosition));
    }

    auto possFile = m_parentFolder->getFileByName(name);
    if (possFile || possFile.isDirty()) {
        return std::nullopt;
    }

    // getting file size
    std::filesystem::path tempFilePath { tempFileName };
    auto fsize = static_cast<int64_t>(std::filesystem::file_size(tempFilePath));

    auto savedFile = m_databaseSession->addNew<File>(name, m_loggedInUser, std::move(m_parentFolder), fsize);
    m_databaseSession->flush();

    std::string fileId = std::string(File::FILE_SYSTEM_ROOT) + std::to_string(savedFile.id());
    std::ifstream sourceFile(tempFileName, std::ios::binary);
    std::ofstream destinationFile(fileId, std::ios::binary);

    destinationFile << sourceFile.rdbuf();
    sourceFile.close();
    destinationFile.close();
    return name;
}
