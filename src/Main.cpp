#include "PersistedData.h"

#include "Papyrus/Papyrus.h"
#include "Papyrus/PapyrusActor.h"
#include "Papyrus/PapyrusConfig.h"
#include "Papyrus/PapyrusInterface.h"

#include "RuntimeEvents.h"
#include "Utilities/Utils.h"
#include "Utilities/LRUCache.h"
#include "Hooks.h"
#include "Plugin.h"
#include "Config.h"
#include "GameForms.h"
#include "Settings.h"

using namespace RE::BSScript;
using namespace SKSE::log;
using namespace SKSE::stl;

namespace {

    void InitializeLogging() {
#ifndef NDEBUG
        auto sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
#else
        auto path = logger::log_directory();
        if (!path) {
            util::report_and_fail("Failed to find standard logging directory"sv);
        }

        *path /= fmt::format("{}.log"sv, Plugin::NAME);
        auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
#endif

#ifndef NDEBUG
        const auto level = spdlog::level::trace;
#else
        const auto level = spdlog::level::trace;
#endif

        auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));
        log->set_level(level);
        log->flush_on(level);

        spdlog::set_default_logger(std::move(log));
        spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%t] [%s:%#] %v"s);
    }

    void InitializeSerialization() {
        SKSE::log::trace("Initializing cosave serialization...");
        auto *serialization = SKSE::GetSerializationInterface();
        serialization->SetUniqueID(PersistedData::kArousalDataKey);
        serialization->SetSaveCallback(PersistedData::SaveCallback);
        serialization->SetRevertCallback(PersistedData::RevertCallback);
        serialization->SetLoadCallback(PersistedData::LoadCallback);
        SKSE::log::trace("Co-save serialization initialized.");
    }

    void InitializePapyrus() {
        SKSE::log::trace("Initializing Papyrus binding...");
        const auto papyrus = SKSE::GetPapyrusInterface();
        papyrus->Register(Papyrus::RegisterFunctions);
        papyrus->Register(PapyrusInterface::RegisterFunctions);
        papyrus->Register(PapyrusConfig::RegisterFunctions);
        papyrus->Register(PapyrusActor::RegisterFunctions);
    }

    void InitializeMessaging() {
        if (!SKSE::GetMessagingInterface()->RegisterListener([](SKSE::MessagingInterface::Message *message) {
                                                                 switch (message->type) {
                                                                     case SKSE::MessagingInterface::kDataLoaded:  // All ESM/ESL/ESP plugins have loaded, main menu is now active
                                                                         if (!GameForms::LoadData()) {
                                                                             logger::critical("Unable to load esp objects");
                                                                             std::_Exit(EXIT_FAILURE);
                                                                         }
                                                                         RuntimeEvents::OnEquipEvent::RegisterEvent();
//				WorldChecks::ArousalUpdateTicker::GetSingleton()->Start();
                                                                         Config::GetSingleton()->LoadINIs();
                                                                         break;
                                                                     case SKSE::MessagingInterface::kPostLoadGame:
                                                                         //Distribute Persisted Keywords
                                                                         Utilities::Keywords::DistributeKeywords();
                                                                         break;
                                                                 }
                                                             }
        )) {
            SKSE::stl::report_and_fail("Unable to register message listener.");
        }
    }
}
extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface *a_skse) {
    InitializeLogging();

    auto *plugin = SKSE::PluginDeclaration::GetSingleton();
    auto version = plugin->GetVersion();
    SKSE::log::info("{} {} is loading...", plugin->GetName(), version);

    SKSE::Init(a_skse);

    Settings::Load();

    Hooks::Install();

    InitializeMessaging();
    InitializeSerialization();
    InitializePapyrus();

    SKSE::log::info("{} has finished loading.", plugin->GetName());
    return true;
}

extern "C" DLLEXPORT constinit auto SKSEPlugin_Version = []() noexcept {
    SKSE::PluginVersionData v;
    v.PluginName(Plugin::NAME.data());
    v.PluginVersion(Plugin::VERSION);
    v.UsesAddressLibrary(true);
    v.UsesNoStructs(true);
    return v;
}();

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Query(const SKSE::QueryInterface *, SKSE::PluginInfo *pluginInfo) {
    pluginInfo->name = SKSEPlugin_Version.pluginName;
    pluginInfo->infoVersion = SKSE::PluginInfo::kVersion;
    pluginInfo->version = SKSEPlugin_Version.pluginVersion;
    return true;
}
