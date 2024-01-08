/**
 * \class StorageApplication
 *
 * The main `Wt::WApplication` implementation for Cloud Goose Storage.
 *
 * \authors Connor Cummings, Joshua Nathan Ming
 * \date 2023-11-27 (last updated)
 */

#pragma once

#include <Wt/Dbo/Session.h>
#include <Wt/WApplication.h>
#include <Wt/WGlobal.h>

class StorageApplication : public Wt::WApplication {
private:
    std::unique_ptr<Wt::Dbo::Session> m_databaseSession;

public:
    /**
     * Creates a new `StorageApplication`.
     *
     * There will be one instance of `StorageApplication` for each user that is
     * using the application simultaneously.
     *
     * \param env The `WEnvironment` to create the application with.
     */
    explicit StorageApplication(const Wt::WEnvironment& env);

    /**
     * Returns the current instance of `StorageApplication`.
     *
     * This is a wrapper around `Wt::WApplication::instance` that does a
     * `dynamic_cast` to `StorageApplication`. It will return a different object
     * depending on which user session it is called from.
     *
     * \return The application instance for the active user session.
     */
    static StorageApplication* instance();

    /**
     * Creates a new Wt::Dbo database session.
     *
     * \return The created session.
     */
    static std::unique_ptr<Wt::Dbo::Session> createDatabaseSession();

    /**
     * Switch this application to a different page.
     *
     * \param newPage The page to switch to.
     */
    void switchPage(std::unique_ptr<Wt::WWidget> newPage);
};
