/**
 * \class Folder
 *
 * A folder that can contain other files and folders.
 *
 * \authors Connor Cummings, Joshua Nathan Ming
 * \date 2023-11-21 (last updated)
 */

#pragma once

#include <Wt/Dbo/Dbo.h>
#include <Wt/Dbo/Field.h>
#include <Wt/Dbo/collection.h>
#include <string_view>
#include "File.h"

class Folder : public StorageElement {
private:
    Wt::Dbo::collection<Wt::Dbo::ptr<File>> m_files;
    Wt::Dbo::collection<Wt::Dbo::ptr<Folder>> m_folders;

public:
    /**
     * Creates a new folder.
     *
     * \param name   The name of this folder.
     * \param owner  The owner of this folder.
     * \param parent The folder that this folder is contained in.
     */
    Folder(std::string name, Wt::Dbo::ptr<User> owner, Wt::Dbo::ptr<Folder> parent);

    /**
     * Creates a new folder with default values for all metadata.
     *
     * This should never be used directly by application code, but it is
     * required by `Wt::Dbo`.
     */
    [[deprecated("only for use by Wt::Dbo")]] Folder() = default;

    /**
     * Gets the list of the files contained in this folder.
     *
     * \return A `Wt::Dbo` collection of files.
     */
    Wt::Dbo::collection<Wt::Dbo::ptr<File>> getFiles() const { return m_files; }

    /**
     * Gets the list of the folders contained in this folder.
     *
     * \return A `Wt::Dbo` collection of folders.
     */
    Wt::Dbo::collection<Wt::Dbo::ptr<Folder>> getFolders() const { return m_folders; }

    /**
     * Looks up a contained file by name.
     *
     * \param name The file name to look up.
     * \return     The requested file, or `nullptr` if the file was not found.
     */
    Wt::Dbo::ptr<File> getFileByName(const std::string& name) const;

    /**
     * Looks up a contained folder by name.
     *
     * \param name The name of the folder to look up.
     * \return     The requested folder, or `nullptr` if the folder was not found.
     */
    Wt::Dbo::ptr<Folder> getFolderByName(const std::string& name) const;

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
        StorageElement::persist(action);
        Wt::Dbo::hasMany(action, m_files, Wt::Dbo::ManyToOne, "parent");
        Wt::Dbo::hasMany(action, m_folders, Wt::Dbo::ManyToOne, "parent");
    }
};
