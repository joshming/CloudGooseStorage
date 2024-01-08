#include "SharingLink.h"

#include <Wt/Dbo/Session.h>
#include <Wt/Dbo/Transaction.h>
#include <Wt/WServer.h>
#include <random>
#include <string>
#include <utility>
#include "File.h"
#include "Wt/Dbo/ptr.h"

SharingLink::SharingLink(Wt::Dbo::ptr<File> file)
    : m_urlID(generateRandomUrlID())
    , m_file(std::move(file))
{
}

void SharingLink::registerResource() const
{
    auto* server = Wt::WServer::instance();
    server->addResource(File::createResource(m_file), m_urlID);
}

void SharingLink::unregisterResource() const
{
    auto* server = Wt::WServer::instance();
    // FIXME: In a multi-threaded environment, this may only be done when the
    //        server is not running.
    server->removeEntryPoint(m_urlID);
}

std::string SharingLink::generateRandomUrlID()
{
    const std::string chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_";

    std::random_device rdevice;
    std::mt19937 gen(rdevice());
    std::uniform_int_distribution<> dis(0, static_cast<int>(chars.size()) - 1);
    constexpr int length = 15;

    std::string randomID;
    randomID.reserve(length);

    for (int i = 0; i < length; ++i) {
        randomID += chars[dis(gen)];
    }
    return randomID;
}

std::string SharingLink::getURLID() const
{
    return m_urlID;
}

std::string SharingLink::createLink(Wt::Dbo::Session* m_databaseSession) const
{
    Wt::Dbo::Transaction transaction(*m_databaseSession);
    auto savedLink = m_databaseSession->addNew<SharingLink>(m_file);

    savedLink->registerResource();

    m_databaseSession->flush();

    return savedLink->m_urlID;
}
