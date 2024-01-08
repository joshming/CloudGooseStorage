/**
 * \class FolderWidget
 *
 * A widget representing a folder in the home page.
 *
 * \authors Connor Cummings
 * \date 2023-11-28 (last updated)
 */

#pragma once

#include <Wt/WEvent.h>
#include <Wt/WText.h>
#include "Folder.h"
#include "User.h"

class FolderWidget : public Wt::WText {
public:
    /**
     * Creates a new `FolderWidget`.
     *
     * \param session The database session to use when moving files to this folder.
     * \param folder  The folder that this widget represents.
     */
    explicit FolderWidget(Wt::Dbo::Session& session, Wt::Dbo::ptr<Folder> folder);

protected:
    /**
     * Handles a drop event.
     *
     * This implementation will handle drop events from FileWidget by moving the
     * file to this folder.
     *
     * \param dropEvent The event to handle.
     */
    void dropEvent(Wt::WDropEvent dropEvent) override;

private:
    Wt::Dbo::Session* m_databaseSession;
    Wt::Dbo::ptr<Folder> m_folder;
};
