#include "File.h"

#include <Wt/WFileResource.h>
#include <Wt/WGlobal.h>
#include <Wt/WResource.h>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include "StorageElement.h"
#include "User.h"

File::File(std::string name, Wt::Dbo::ptr<User> owner, Wt::Dbo::ptr<Folder> parent, int64_t fileSize)
    : StorageElement(std::move(name), std::move(owner), std::move(parent))
    , m_fileSize(fileSize)
{
}

std::shared_ptr<Wt::WResource> File::createResource(Wt::Dbo::ptr<File> file)
{
    std::string filePath = std::string(FILE_SYSTEM_ROOT) + std::to_string(file.id());
    auto resource = std::make_shared<Wt::WFileResource>("application/octet-stream", filePath);
    resource->suggestFileName(file->getName());
    return resource;
}
