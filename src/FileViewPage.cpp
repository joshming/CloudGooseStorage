#include "FileViewPage.h"

#include <Wt/Dbo/Session.h>
#include <Wt/Dbo/Transaction.h>
#include <Wt/Dbo/ptr.h>
#include <Wt/WLabel.h>
#include <Wt/WLineEdit.h>
#include <Wt/WMessageBox.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <memory>
#include <utility>
#include <string>
#include <vector>
#include "File.h"
#include "FileStoragePage.h"
#include "FileWidget.h"
#include "Folder.h"
#include "FolderStoragePage.h"
#include "FolderWidget.h"
#include "LoginPage.h"
#include "StorageApplication.h"
#include "User.h"

FileViewPage::FileViewPage(const Wt::Dbo::ptr<User>& user, Wt::Dbo::Session& session, Wt::Dbo::ptr<Folder> parentFolder)
    : m_databaseSession(&session)
    , m_user(user)
    , m_parentFolder(std::move(parentFolder))
{
    setStyleClass("fileview-page");

    // Header
    auto* headerContainer = addNew<Wt::WContainerWidget>();
    headerContainer->setStyleClass("header-container");

    auto* logo = headerContainer->addNew<Wt::WImage>("/CloudGooseStorageLogoWithoutText.png");
    logo->addStyleClass("logo-without-text");

    auto* userText = headerContainer->addNew<Wt::WText>(user->getUsername() + "'s Cloud Goose Storage");
    userText->setStyleClass("header");

    auto* logOutButton = headerContainer->addNew<Wt::WPushButton>("Log Out");
    logOutButton->addStyleClass("back-button");

    // Sidebar
    auto* sidebar = addNew<Wt::WContainerWidget>();
    sidebar->setStyleClass("sidebar");

    auto* fileUploadButton = sidebar->addNew<Wt::WPushButton>("Upload File");
    fileUploadButton->setStyleClass("upload-button");

    auto* addFolderButton = sidebar->addNew<Wt::WPushButton>("Add Folder");
    addFolderButton->setStyleClass("upload-button");

    auto* tagText = sidebar->addNew<Wt::WText>("Filters");
    tagText->setStyleClass("section-text");
    auto* tagContainer = sidebar->addNew<Wt::WContainerWidget>();
    tagContainer->setStyleClass("filters-container");

    auto* nameSort = tagContainer->addNew<Wt::WPushButton>("Sort by Name");
    auto* fileSizeSort = tagContainer->addNew<Wt::WPushButton>("Sort by File Size");
    auto* typeSort = tagContainer->addNew<Wt::WPushButton>("Sort by Type");
    nameSort->setStyleClass("filter-element");
    fileSizeSort->setStyleClass("filter-element");
    typeSort->setStyleClass("filter-element");

    auto* mainContainer = addNew<Wt::WContainerWidget>();
    mainContainer->addStyleClass("main-container");

    std::string folderPath;
    {
        Wt::Dbo::Transaction transaction(*m_databaseSession);
        auto rootFolder = user->getRootFolder();
        std::string rootFolderName = rootFolder->getName();

        auto pathParent = m_parentFolder;

        while (pathParent != rootFolder) {
            folderPath = pathParent->getName() + "/" + folderPath;
            pathParent = pathParent->getParent();
        }

        if (folderPath.empty()) {
            folderPath = rootFolderName;
        } else {
            folderPath = rootFolderName + "/" + folderPath;
        }
    }

    auto* currentPathContainer = mainContainer->addNew<Wt::WContainerWidget>();
    currentPathContainer->setStyleClass("current-path-container");

    auto* parentFolderButton = currentPathContainer->addNew<ParentFolderButton>(this);

    auto* currentPathText = currentPathContainer->addNew<Wt::WText>(folderPath);
    currentPathText->setStyleClass("current-path");

    auto* folderText = mainContainer->addNew<Wt::WText>("Folders");
    folderText->setStyleClass("section-text");

    parentFolderButton->clicked().connect([this] {
        auto* application = StorageApplication::instance();

        if (!(m_parentFolder->getParent())) {
            application->switchPage(std::make_unique<FileViewPage>(m_user, *m_databaseSession, m_parentFolder));
        } else {
            auto grandParent = m_parentFolder->getParent();
            application->switchPage(std::make_unique<FileViewPage>(m_user, *m_databaseSession, grandParent));
        }
    });

    auto* folderContainer = mainContainer->addNew<Wt::WContainerWidget>();
    folderContainer->setStyleClass("section-container");
    {
        Wt::Dbo::Transaction transaction(*m_databaseSession);
        const auto& folders = m_parentFolder->getFolders();

        if (folders.empty()) {
            folderContainer->addNew<Wt::WText>("No Folders");
        } else {
            for (const auto& folder : folders) {
                auto* folderWidget = folderContainer->addNew<FolderWidget>(*m_databaseSession, folder);

                folderWidget->clicked().connect([this, folder] {
                    auto* application = StorageApplication::instance();
                    application->switchPage(std::make_unique<FileViewPage>(m_user, *m_databaseSession, folder));
                });
            }
        }
    }

    auto* fileText = mainContainer->addNew<Wt::WText>("Files");
    fileText->setStyleClass("section-text");
    auto* fileContainer = mainContainer->addNew<Wt::WContainerWidget>();
    fileContainer->setStyleClass("section-container");

    {
        Wt::Dbo::Transaction transaction(*m_databaseSession);
        auto files = m_parentFolder->getFiles();

        addFiles(fileContainer, files);
    }

    nameSort->clicked().connect([this, user, fileContainer] {
        Wt::Dbo::Transaction transaction(*m_databaseSession);
        auto files = m_parentFolder->getFiles();
        files = (m_hasSorted) ? files.find().orderBy("name ASC") : files.find().orderBy("name DESC");
        m_hasSorted = !m_hasSorted;

        addFiles(fileContainer, files);
    });

    typeSort->clicked().connect([this, user, fileContainer] {
        Wt::Dbo::Transaction transaction(*m_databaseSession);
        auto files = m_parentFolder->getFiles();

        files = (m_hasSorted) ? files.find().orderBy("CASE WHEN INSTR(name, '.') > 0 THEN SUBSTR(name, INSTR(name, '.') + 1) ELSE name END ASC") : files.find().orderBy("CASE WHEN INSTR(name, '.') > 0 THEN SUBSTR(name, INSTR(name, '.') + 1) ELSE name END DESC");
        m_hasSorted = !m_hasSorted;

        addFiles(fileContainer, files);
    });

    fileSizeSort->clicked().connect([this, user, fileContainer] {
        Wt::Dbo::Transaction transaction(*m_databaseSession);
        auto files = m_parentFolder->getFiles();

        files = (m_hasSorted) ? files.find().orderBy("file_size ASC") : files.find().orderBy("file_size DESC");
        m_hasSorted = !m_hasSorted;

        addFiles(fileContainer, files);
    });
    logOutButton->clicked().connect([this] {
        auto* application = StorageApplication::instance();
        application->switchPage(std::make_unique<LoginPage>(*m_databaseSession));
    });

    fileUploadButton->clicked().connect([this] {
        auto* application = StorageApplication::instance();
        application->switchPage(std::make_unique<FileStoragePage>(m_user, *m_databaseSession, m_parentFolder));
    });

    addFolderButton->clicked().connect([this] {
        auto* application = StorageApplication::instance();
        application->switchPage(std::make_unique<FolderStoragePage>(m_user, *m_databaseSession, m_parentFolder));
    });

    sidebar->addNew<Wt::WText>("Search")->setStyleClass("section-text");
    auto* searchInput = sidebar->addNew<Wt::WLineEdit>();
    searchInput->setPlaceholderText("Search Files");
    searchInput->setStyleClass("search-input");

    auto* searchButton = sidebar->addNew<Wt::WPushButton>("Search");
    searchButton->setStyleClass("search-button");

    searchButton->clicked().connect([this, searchInput, fileContainer] {
        std::string query = searchInput->text().toUTF8();
        filterFiles(fileContainer, query);
    });
}

void FileViewPage::addFiles(Wt::WContainerWidget* fileContainer, const Wt::Dbo::collection<Wt::Dbo::ptr<File>>& files) const
{
    fileContainer->clear();
    if (files.empty()) {
        fileContainer->addNew<Wt::WText>("No Files");
    } else {
        for (const auto& file : files) {
            auto* fileWidget = fileContainer->addNew<FileWidget>(m_user, *m_databaseSession, file, m_parentFolder);

            fileWidget->deleteFile().connect([this, file, fileContainer, fileWidget] {
                deleteFile(file->getName(), fileContainer, fileWidget);
            });

            fileWidget->moveFile().connect([this, file, fileContainer, fileWidget] {
                fileContainer->removeWidget(fileWidget);
                {
                    Wt::Dbo::Transaction transaction(*m_databaseSession);
                    auto files = m_parentFolder->getFiles();

                    if (files.empty()) {
                        fileContainer->addNew<Wt::WText>("No Files");
                    }
                }
            });
        }
    }
}

void FileViewPage::deleteFile(const std::string& name, Wt::WContainerWidget* fileContainer, FileWidget* fileWidget) const
{
    // removing from database
    Wt::Dbo::Transaction transaction(*m_databaseSession);
    Wt::Dbo::ptr<File> fileToDelete = m_parentFolder->getFileByName(name);
    fileToDelete.remove();

    // removing from internal storage
    std::string fileid = std::string(File::FILE_SYSTEM_ROOT) + std::to_string(fileToDelete.id());

    std::filesystem::path filePath { fileid };
    std::filesystem::remove(filePath);

    // re-rendering files
    fileContainer->removeWidget(fileWidget);
    {
        Wt::Dbo::Transaction transaction(*m_databaseSession);
        auto files = m_parentFolder->getFiles();

        if (files.empty()) {
            fileContainer->addNew<Wt::WText>("No Files");
        }
    }
}

FileViewPage::ParentFolderButton::ParentFolderButton(FileViewPage* page)
    : Wt::WPushButton("Parent Folder")
    , m_page(page)
{
    addStyleClass("back-button");
    acceptDrops(std::string(FILE_MIME_TYPE));
}

void FileViewPage::ParentFolderButton::dropEvent(Wt::WDropEvent dropEvent)
{
    // FileWidget has a separate drag handle widget that is draggable, so the
    // drop events come from that.
    auto* sourceWidget = dynamic_cast<Wt::WWidget*>(dropEvent.source());
    if (!sourceWidget) {
        std::cerr << "ParentFolderButton: Received drop that wasn't from a widget of any kind" << std::endl;
        return;
    }
    auto* fileWidget = dynamic_cast<FileWidget*>(sourceWidget->parent());
    if (!fileWidget) {
        std::cerr << "ParentFolderButton: Received drop that wasn't from a FileWidget" << std::endl;
        return;
    }

    Wt::Dbo::Transaction transaction(*m_page->m_databaseSession);
    try {
        fileWidget->moveFile(m_page->m_parentFolder->getParent());
    } catch (const std::runtime_error& ex) {
        std::cerr << "ParentFolderButton: Failed to move file: " << ex.what() << std::endl;
        transaction.rollback();
        return;
    }
}

void FileViewPage::filterFiles(Wt::WContainerWidget* fileContainer, const std::string& query) const
{
    fileContainer->clear();
    Wt::Dbo::Transaction transaction(*m_databaseSession);
    auto files = m_parentFolder->getFiles();

    std::vector<Wt::Dbo::ptr<File>> filteredFiles;

    for (const auto& file : files) {
        std::string fileName = file->getName();
        std::transform(fileName.begin(), fileName.end(), fileName.begin(), ::tolower);
        std::string lowerQuery = query;
        std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);

        if (fileName.find(lowerQuery) != std::string::npos) {
            filteredFiles.push_back(file);
        }
    }

    if (filteredFiles.empty()) {
        fileContainer->addNew<Wt::WText>("No Files");
    } else {
        for (const auto& file : filteredFiles) {
            fileContainer->addNew<FileWidget>(m_user, *m_databaseSession, file, m_parentFolder)->setStyleClass("section-element file-element");
        }
    }
}
