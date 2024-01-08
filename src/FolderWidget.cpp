#include "FolderWidget.h"

#include <Wt/Dbo/Transaction.h>
#include <Wt/WEvent.h>
#include <stdexcept>
#include "FileViewPage.h"
#include "FileWidget.h"
#include "StorageApplication.h"

FolderWidget::FolderWidget(Wt::Dbo::Session& session, Wt::Dbo::ptr<Folder> folder)
    : Wt::WText(folder->getName())
    , m_databaseSession(&session)
    , m_folder(std::move(folder))
{
    setStyleClass("section-element file-element");
    acceptDrops(std::string(FileViewPage::FILE_MIME_TYPE));
}

void FolderWidget::dropEvent(Wt::WDropEvent dropEvent)
{
    // FileWidget has a separate drag handle widget that is draggable, so the
    // drop events come from that.
    auto* sourceWidget = dynamic_cast<Wt::WWidget*>(dropEvent.source());
    if (!sourceWidget) {
        std::cerr << "FolderWidget: Received drop that wasn't from a widget of any kind" << std::endl;
        return;
    }
    auto* fileWidget = dynamic_cast<FileWidget*>(sourceWidget->parent());
    if (!fileWidget) {
        std::cerr << "FolderWidget: Received drop that wasn't from a FileWidget" << std::endl;
        return;
    }

    Wt::Dbo::Transaction transaction(*m_databaseSession);
    try {
        fileWidget->moveFile(m_folder);
    } catch (const std::runtime_error& ex) {
        std::cerr << "FolderWidget: Failed to move file: " << ex.what() << std::endl;
        transaction.rollback();
        return;
    }
}
