#include "Folder.h"

#include "StorageElement.h"

Folder::Folder(std::string name, Wt::Dbo::ptr<User> owner, Wt::Dbo::ptr<Folder> parent)
    : StorageElement(std::move(name), std::move(owner), std::move(parent))
{
}

Wt::Dbo::ptr<File> Folder::getFileByName(const std::string& name) const
{
    auto fileQuery = m_files.find().where("name = ?").bind(name).limit(1);
    return fileQuery.resultValue();
}

Wt::Dbo::ptr<Folder> Folder::getFolderByName(const std::string& name) const
{
    auto fileQuery = m_folders.find().where("name = ?").bind(name).limit(1);
    return fileQuery.resultValue();
}
