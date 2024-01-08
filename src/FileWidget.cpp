#include "FileWidget.h"

#include <Wt/Dbo/Transaction.h>
#include <Wt/Dbo/ptr.h>
#include <Wt/WAnchor.h>
#include <Wt/WDialog.h>
#include <Wt/WEnvironment.h>
#include <Wt/WGlobal.h>
#include <Wt/WLabel.h>
#include <Wt/WLineEdit.h>
#include <Wt/WLink.h>
#include <Wt/WMessageBox.h>
#include <Wt/WPopupMenu.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include <cstddef>
#include <memory>
#include <string>
#include <utility>
#include "File.h"
#include "FileViewPage.h"
#include "Folder.h"
#include "SharingLink.h"
#include "StorageApplication.h"
#include "User.h"

FileWidget::FileWidget(Wt::Dbo::ptr<User> user, Wt::Dbo::Session& session, const Wt::Dbo::ptr<File>& file, Wt::Dbo::ptr<Folder> parentFolder)
    : Wt::WAnchor(Wt::WLink(File::createResource(file)))
    , m_databaseSession(&session)
    , m_user(std::move(user))
    , m_file(file)
    , m_parentFolder(std::move(parentFolder))
{
    setStyleClass("file-element section-element");
    setAttributeValue("download", file->getName());

    auto* dragHandle = addNew<Wt::WText>(u"\u2261"); // Looks like three horizontal lines
    dragHandle->setStyleClass("drag-handle");
    dragHandle->setDraggable(std::string(FileViewPage::FILE_MIME_TYPE), this);

    // Wt has it's own drag and drop system that's separate from HTML, but
    // HTML's default behavior is to make links draggable. This causes some
    // conflicting behavior, so we tell HTML not to make the link draggable.
    setAttributeValue("draggable", "false");

    // Allows the user to see the full file name on hover even if it's too long
    // to show on screen normally.
    setToolTip(m_file->getName());

    auto* fileName = addNew<Wt::WText>(m_file->getName());
    fileName->setStyleClass("file-element");
    // Use the Unicode "Midline Horizontal Ellipsis" character to make the ...
    // vertically centered.
    auto* fileAction = addNew<Wt::WPushButton>(u"\u22ef");
    fileAction->setStyleClass("file-more-button");
    auto popUpMenu = std::make_unique<Wt::WPopupMenu>();
    popUpMenu->addItem("rename")->triggered().connect([this, fileName] {
        auto* renameBox = addChild(std::make_unique<Wt::WDialog>("What would you like to rename your file to?"));
        renameBox->contents()->addNew<Wt::WLabel>("New name:");
        auto* name = renameBox->contents()->addNew<Wt::WLineEdit>();
        auto* dialogText = renameBox->contents()->addNew<Wt::WText>("");
        auto* submit = renameBox->footer()->addNew<Wt::WPushButton>("Rename");
        auto* cancel = renameBox->footer()->addNew<Wt::WPushButton>("Cancel");
        renameBox->rejectWhenEscapePressed();
        submit->clicked().connect([this, name, renameBox, fileName, dialogText] {
            renameFile(name->text().toUTF8(), *fileName, *renameBox, dialogText);
            fileName->setText(m_file->getName());
        });

        cancel->clicked().connect(renameBox, &Wt::WDialog::accept);
        renameBox->finished().connect([this, renameBox] {
            removeChild(renameBox);
        });
        renameBox->show();
    });

    popUpMenu->addItem("move")->triggered().connect([this] {
        auto moveBox = std::make_shared<Wt::WDialog>("What folder would you like to move your file to?");
        moveBox->contents()->addNew<Wt::WLabel>("Folder name:");
        auto* name = moveBox->contents()->addNew<Wt::WLineEdit>();
        auto* dialogText = moveBox->contents()->addNew<Wt::WText>("");
        auto* submit = moveBox->footer()->addNew<Wt::WPushButton>("Move");
        auto* cancel = moveBox->footer()->addNew<Wt::WPushButton>("Cancel");
        moveBox->rejectWhenEscapePressed();
        submit->clicked().connect([this, name, moveBox, dialogText] {
            moveFile(name->text().toUTF8(), *moveBox, dialogText);
        });

        cancel->clicked().connect([moveBox] {
            moveBox->accept();
        });

        moveBox->show();
    });

    popUpMenu->addItem("delete")->triggered().connect([this] {
        auto deleteBox = std::make_shared<Wt::WMessageBox>();
        deleteBox->setWindowTitle("Are you sure you want to delete this file?");
        deleteBox->setText("This action cannot be undone.");
        deleteBox->setStandardButtons(Wt::StandardButton::Yes | Wt::StandardButton::No);
        deleteBox->rejectWhenEscapePressed();

        deleteBox->buttonClicked().connect([this, deleteBox](Wt::StandardButton button) {
            if (button == Wt::StandardButton::Yes) {
                m_deleteFile.emit();
            }
            deleteBox->accept();
        });

        deleteBox->show();
    });
    popUpMenu->addItem("share")->triggered().connect([this] {
        SharingLink link(m_file);
        std::string url = link.createLink(m_databaseSession);

        auto* sharePopup = addChild(std::make_unique<Wt::WDialog>("Share Link"));
        sharePopup->contents()->addNew<Wt::WText>("This link can be used to download your file: ");

        const auto& environment = StorageApplication::instance()->environment();
        std::string urlLink = environment.urlScheme() + "://" + environment.hostName() + "/" + url;
        auto* hyperlink = sharePopup->contents()->addNew<Wt::WAnchor>(urlLink, urlLink);
        hyperlink->setAttributeValue("target", "_blank");

        auto* okButton = sharePopup->footer()->addNew<Wt::WPushButton>("Okay");

        okButton->clicked().connect([sharePopup] {
            sharePopup->accept();
        });

        sharePopup->show();
    });

    fileAction->setMenu(std::move(popUpMenu));

    // Don't download the file when clicking on the ... menu.
    fileAction->clicked().preventDefaultAction(true);
    fileAction->clicked().preventPropagation(true);
}

void FileWidget::renameFile(const std::string& name, Wt::WText& fileName, Wt::WDialog& renameBox, Wt::WText* dialogText)
{
    const size_t fileExtensionPosition = m_file->getName().find_last_of('.');
    std::string nameWithExtension;
    nameWithExtension = fileExtensionPosition != std::string::npos ? name + m_file->getName().substr(fileExtensionPosition) : name;
    Wt::Dbo::Transaction transaction(*m_databaseSession);

    auto duplicateQuery = m_parentFolder->getFileByName(nameWithExtension);

    if (duplicateQuery) {
        dialogText->setText("Another file already has this name.");
        return;
    }
    if (fileExtensionPosition != std::string::npos) {
        m_file.modify()->setName(name + m_file->getName().substr(fileExtensionPosition));
    } else {
        m_file.modify()->setName(name);
    }
    fileName.setText(m_file->getName());
    this->setLink(File::createResource(m_file));
    renameBox.accept();
}

void FileWidget::moveFile(const std::string& name, Wt::WDialog& moveBox, Wt::WText* dialogText)
{
    Wt::Dbo::Transaction transaction(*m_databaseSession);
    auto folderQuery = m_databaseSession->find<Folder>().where("name = ?").bind(name).limit(1);

    if (folderQuery.resultList().empty()) {
        dialogText->setText("A folder with that name does not exist.");
        return;
    }

    Wt::Dbo::ptr<Folder> foundFolder = folderQuery.resultValue();

    try {
        moveFile(foundFolder);
    } catch (const std::runtime_error& ex) {
        dialogText->setText(ex.what());
        return;
    }

    moveBox.accept();
}

void FileWidget::moveFile(Wt::Dbo::ptr<Folder> folder)
{
    if (m_file->getParent() == folder) {
        throw std::runtime_error("The file is already in this folder. Please specify a different folder.");
    }

    auto duplicateFile = folder->getFileByName(m_file->getName());
    if (duplicateFile) {
        throw std::runtime_error("Another file with this name exists in your destination folder. Please specify a different folder or rename this file.");
    }

    m_file.modify()->setParent(folder);
    m_moveFile.emit();
}
