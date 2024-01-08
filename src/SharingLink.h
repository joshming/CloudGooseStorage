/**
 * \class SharingLink
 *
 * \brief Represents a link for sharing files.
 *
 * This class manages the creation and registration for sharing links
 * associated with files. It generates unique URLs and facilitates file access through
 * these links within a database context.
 *
 * \authors Connor Cummings, Matthew Lucas Otchet
 * \date 2023-11-28 (last updated)
 */

#pragma once

#include <Wt/Dbo/Dbo.h>
#include <Wt/Dbo/Field.h>
#include <Wt/Dbo/ptr.h>
#include <string>
#include <utility>
#include "User.h"

class File;

class SharingLink {
private:
    std::string m_urlID;
    Wt::Dbo::ptr<File> m_file;

public:
    /**
     * Creates a new file sharing link.
     *
     * This creates the file sharing link to download a file.
     *
     * \param file  The file that the sharing link points to.
     *
     */
    SharingLink(Wt::Dbo::ptr<File> file);

    /**
     * Creates a new file sharing link with default values for all metadata.
     *
     * This should never be used directly by application code, but it is
     * required by `Wt::Dbo`.
     */
    [[deprecated("only for use by Wt::Dbo")]] SharingLink() = default;

    /**
     * Registers this link to receive requests from the web server.
     */
    void registerResource() const;

    /**
     * Unregisters this link from the web server.
     *
     * After calling this function, the link will no longer be accessible.
     * However, it may be registered again if the server is restarted and the
     * link is still in the database.
     */
    void unregisterResource() const;
    /**
     * Generates a random alphanumeric ID for the URL.
     *
     * This function creates a unique identifier consisting of alphanumeric characters,
     * used for the URL of the sharing link.
     *
     * \return A randomly generated alphanumeric URL ID.
     */
    static std::string generateRandomUrlID();

    /**
     * Retrieves the URL ID associated with the sharing link.
     *
     * This function retrieves and returns the URL ID of the sharing link.
     *
     * \return The URL ID of the sharing link.
     */
    std::string getURLID() const;

    /**
     * Creates a sharing link for the specified file.
     *
     * This function generates a sharing link for a given file within the provided database session.
     * It registers the link to be accessible via a URL and persists it in the database.
     *
     * \param m_databaseSession Pointer to the database session.
     * \return The URL ID of the created sharing link.
     */
    std::string createLink(Wt::Dbo::Session* m_databaseSession) const;

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
        Wt::Dbo::field(action, m_urlID, "url_id");
        Wt::Dbo::belongsTo(action, m_file, "file", Wt::Dbo::NotNull | Wt::Dbo::OnDeleteCascade);
    }
};
