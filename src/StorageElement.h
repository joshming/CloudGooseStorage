/**
 * \class StorageElement
 *
 * The base class for `File` and `Folder`.
 *
 * Since `Wt::Dbo` does not support inheritance, `File` and `Folder` must be
 * kept separate whenever interacting with the database. However, inheritance
 * can (and is) still used to factor out common fields and reduce duplication.
 *
 * \authors Connor Cummings, Joshua Nathan Ming
 * \date 2023-11-21 (last updated)
 */

#pragma once

#include <Wt/Dbo/Dbo.h>
#include <utility>
#include "User.h"

class Folder;

class StorageElement {
private:
    std::string m_name;
    Wt::Dbo::ptr<User> m_owner;
    Wt::Dbo::ptr<Folder> m_parent;

protected:
    /**
     * Creates a new storage element.
     *
     * \param name   The name of this storage element.
     * \param owner  The owner of this storage element.
     * \param parent The folder that this storage element is contained in.
     */
    StorageElement(std::string name, Wt::Dbo::ptr<User> owner, Wt::Dbo::ptr<Folder> parent);

    /**
     * Creates a new storage element with default values for all metadata.
     *
     * This should never be used directly by application code, but it is
     * required by `Wt::Dbo`.
     */
    StorageElement() = default;

public:
    /**
     * Gets the name of this storage element.
     *
     * \return A string containing the name
     */
    const std::string& getName() const { return m_name; }

    /**
     * Gets the owner of this storage element.
     *
     * \return The user that owns this storage element.
     */
    Wt::Dbo::ptr<User> getOwner() const { return m_owner; }

    /**
     * Gets the folder that contains this storage element.
     *
     * \return The containing folder, or `nullptr` if this is a root folder.
     */
    Wt::Dbo::ptr<Folder> getParent() const { return m_parent; }

    /**
     * Renames this storage element.
     *
     * \param name The new name for this storage element.
     */
    void setName(std::string name) { m_name = std::move(name); }

    /**
     * Moves this storage element to a different folder.
     *
     * \param parent The new containing folder.
     */
    void setParent(Wt::Dbo::ptr<Folder> parent) { m_parent = std::move(parent); }

protected:
    /**
     * Persists changes to the database.
     *
     * This should never be used directly by application code, but it is
     * required by `Wt::Dbo`.
     *
     * \param action The database action to perform.
     */
    template <class Action>
    void persist(Action& action)
    {
        Wt::Dbo::field(action, m_name, "name");
        Wt::Dbo::belongsTo(action, m_owner, "owner");
        Wt::Dbo::belongsTo(action, m_parent, "parent");
    }
};
