#include "hyp_network_manager.hpp"

#include <phosphor-logging/log.hpp>
#include <sdeventplus/event.hpp>
#include <stdplus/print.hpp>

using phosphor::logging::entry;
using phosphor::logging::level;
using phosphor::logging::log;

constexpr char DEFAULT_HYP_NW_OBJPATH[] =
    "/xyz/openbmc_project/network/hypervisor";
constexpr char HYP_DEFAULT_NETWORK_BUSNAME[] =
    "xyz.openbmc_project.Network.Hypervisor";

namespace phosphor
{
namespace network
{

int main()
{
    auto bus = sdbusplus::bus::new_default();

    // Add sdbusplus ObjectManager
    sdbusplus::server::manager_t objManager(bus, DEFAULT_HYP_NW_OBJPATH);

    // Get default event loop
    auto event = sdeventplus::Event::get_default();

    // Attach the bus to sd_event to service user requests
    bus.attach_event(event.get(), SD_EVENT_PRIORITY_NORMAL);

    // Create hypervisor network manager dbus object
    auto manager = std::make_unique<HypNetworkMgr>(bus, DEFAULT_HYP_NW_OBJPATH);

    // Create the hypervisor eth interface objects
    manager->createIfObjects();

    // Create the hypervisor system config object
    manager->createSysConfObj();
    const SystemConfPtr& systemConfigObj = manager->getSystemConf();
    systemConfigObj->setHostName();

    bus.request_name(HYP_DEFAULT_NETWORK_BUSNAME);

    return event.loop();
}

} // namespace network
} // namespace phosphor

int main(int /*argc*/, char** /*argv*/)
{
    try
    {
        return phosphor::network::main();
    }
    catch (const std::exception& e)
    {
        stdplus::print(stderr, "FAILED: {}", e.what());
        fflush(stderr);
        return 1;
    }
}
