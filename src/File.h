/**
 * \class File
 *
 * A regular file stored in the database.
 *
 * \authors Arjun Sharma, Connor Cummings, Joshua Nathan Ming, Matthew Lucas Otchet
 * \date 2023-11-28 (last updated)
 */

#pragma once

#include <Wt/Dbo/Dbo.h>
#include <Wt/WResource.h>
#include <cstdint>
#include <memory>
#include "SharingLink.h"
#include "StorageElement.h"

class File : public StorageElement {
public:
    /**
     * The folder in the real filesystem where all user files will be stored.
     */
    constexpr static const std::string_view FILE_SYSTEM_ROOT = "./userFiles/";

private:
    // int64_t is chosen due to uint64_t not being supported in sqlite
    int64_t m_fileSize { 0 };
    Wt::Dbo::collection<Wt::Dbo::ptr<SharingLink>> m_sharingLinks;

public:
    /**
     * Creates a new file.
     *
     * This only creates the file in the database, and does not create an associated
     * real file with the content. This must be done separately.
     *
     * \param name   The name of this file.
     * \param owner  The owner of this file.
     * \param parent The folder that this file is contained in.
     * \param fileSize The size of the file.
     *
     * \see FileUploadPage::uploadFile
     */
    File(std::string name, Wt::Dbo::ptr<User> owner, Wt::Dbo::ptr<Folder> parent, int64_t fileSize);

    /**
     * Creates a new file with default values for all metadata.
     *
     * This should never be used directly by application code, but it is
     * required by `Wt::Dbo`.
     */
    [[deprecated("only for use by Wt::Dbo")]] File() = default;

    /**
     * Creates a WResource that can be used to download a file.
     *
     * This method is static because it needs to access the ID of the file,
     * which is only available with a Wt::Dbo::ptr.
     *
     * The MIME type (which browsers use to determine file type) will be set to
     * `application/octet-stream`, which is the file type for generic data.
     * This is done for two reasons:
     *
     *  1. The browser will download the file instead of attempting to display
     *     it, even if the actual data is a file type that the browser
     *     recognizes.
     *  2. We don't keep track of the MIME type, nor do we have a lookup table
     *     to find one from the file extension.
     *
     * \param file The file to create a resource for.
     * \return A WResource that will respond with this file.
     */
    static std::shared_ptr<Wt::WResource> createResource(Wt::Dbo::ptr<File> file);

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
        Wt::Dbo::field(action, m_fileSize, "file_size");
        Wt::Dbo::hasMany(action, m_sharingLinks, Wt::Dbo::ManyToOne, "file");
    }
};
