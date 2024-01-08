#include "StorageElement.h"

StorageElement::StorageElement(std::string name, Wt::Dbo::ptr<User> owner, Wt::Dbo::ptr<Folder> parent)
    : m_name(std::move(name))
    , m_owner(std::move(owner))
    , m_parent(std::move(parent))
{
}
