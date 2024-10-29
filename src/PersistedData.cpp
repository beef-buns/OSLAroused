#include "PersistedData.h"
#include "Managers/SceneManager.h"
#include "Managers/ActorStateManager.h"

namespace PersistedData {
    //BaseData is based off how powerof3's did it in Afterlife
    template<typename T>
    bool
    BaseData<T>::Save(SKSE::SerializationInterface *serializationInterface, std::uint32_t type, std::uint32_t version) {
        if (!serializationInterface->OpenRecord(type, version)) {
            logger::error("Failed to open record for Data Serialization!");
            return false;
        }

        return Save(serializationInterface);
    }

    template<typename T>
    bool BaseData<T>::Save(SKSE::SerializationInterface *serializationInterface) {
        assert(serializationInterface);
        Locker locker(m_Lock);

        const auto numRecords = m_Data.size();
        if (!serializationInterface->WriteRecordData(numRecords)) {
            logger::error("Failed to save {} data records", numRecords);
            return false;
        }

        for (const auto &[formId, value]: m_Data) {
            if (!serializationInterface->WriteRecordData(formId)) {
                logger::error("Failed to save data for FormID: ({:X})", formId);
                return false;
            }

            if (!serializationInterface->WriteRecordData(value)) {
                logger::error("Failed to save value data for form: {}", formId);
                return false;
            }
        }
        return true;
    }

    template<typename T>
    bool BaseData<T>::Load(SKSE::SerializationInterface *serializationInterface) {
        assert(serializationInterface);

        std::size_t recordDataSize;
        serializationInterface->ReadRecordData(recordDataSize);

        Locker locker(m_Lock);
        m_Data.clear();

        RE::FormID formId;
        T value;

        for (auto i = 0; i < recordDataSize; i++) {
            auto formIdResult = stl::ReadFormID(serializationInterface);
            if (!formIdResult) {
                logger::error("Failed to read form ID for data record {}"sv, i);
                continue;
            }
            formId = formIdResult.value();

            serializationInterface->ReadRecordData(value);
            m_Data[formId] = value;
        }
        return true;
    }

    template<typename T>
    void BaseData<T>::Clear() {
        Locker locker(m_Lock);
        m_Data.clear();
    }

    bool BaseFormArrayData::Save(SKSE::SerializationInterface *serializationInterface, std::uint32_t type,
                                 std::uint32_t version) {
        if (!serializationInterface->OpenRecord(type, version)) {
            logger::error("Failed to open record for Data Serialization!");
            return false;
        }

        return Save(serializationInterface);
    }

    bool BaseFormArrayData::Save(SKSE::SerializationInterface *serializationInterface) {
        assert(serializationInterface);
        Locker locker(m_Lock);

        const auto numRecords = m_Data.size();
        if (!serializationInterface->WriteRecordData(numRecords)) {
            logger::error("Failed to save {} data records", numRecords);
            return false;
        }

        for (const auto &[formId, formList]: m_Data) {
            if (!serializationInterface->WriteRecordData(formId)) {
                logger::error("Failed to save data for FormID: ({:X})", formId);
                return false;
            }

            const auto numForms = formList.size();
            if (!serializationInterface->WriteRecordData(numForms)) {
                logger::error("Failed to save {} sub form list records", numForms);
                return false;
            }

            for (const auto &subFormId: formList) {
                if (!serializationInterface->WriteRecordData(subFormId)) {
                    logger::error("Failed to save data for sub FormID: ({})", subFormId);
                    return false;
                }
            }
        }
        return true;
    }

    bool BaseFormArrayData::Load(SKSE::SerializationInterface *serializationInterface) {
        assert(serializationInterface);

        std::size_t recordDataSize;
        serializationInterface->ReadRecordData(recordDataSize);

        Locker locker(m_Lock);
        m_Data.clear();

        RE::FormID formId;
        std::size_t subFormIdCount;
        RE::FormID subFormId;

        for (auto i = 0; i < recordDataSize; i++) {
            serializationInterface->ReadRecordData(formId);
            //Ensure form still exists
            RE::FormID fixedId;
            if (!serializationInterface->ResolveFormID(formId, fixedId)) {
                logger::error("Failed to resolve formID {} {}"sv, formId, fixedId);
                continue;
            }

            std::set<RE::FormID> subFormIds;
            serializationInterface->ReadRecordData(subFormIdCount);
            for (auto subFormIndex = 0; subFormIndex < subFormIdCount; subFormIndex++) {
                serializationInterface->ReadRecordData(subFormId);
                subFormIds.insert(subFormId);
            }
            m_Data[formId] = subFormIds;
        }
        return true;
    }


    std::string DecodeTypeCode(std::uint32_t a_typeCode) {
        constexpr std::size_t SIZE = sizeof(std::uint32_t);

        std::string sig;
        sig.resize(SIZE);
        const char *iter = reinterpret_cast<char *>(&a_typeCode);
        for (std::size_t i = 0, j = SIZE - 2; i < SIZE - 1; ++i, --j) {
            sig[j] = iter[i];
        }

        return sig;
    }

    void SaveCallback(SKSE::SerializationInterface *serializationInterface) {
        const auto arousalData = ArousalData::GetSingleton();
        if (!arousalData->Save(serializationInterface, kArousalDataKey, kSerializationVersion)) {
            logger::critical("Failed to save Arousal Data");
        }

        const auto exposureData = ExposureData::GetSingleton();
        if (!exposureData->Save(serializationInterface, kExposureDataKey, kSerializationVersion)) {
            logger::critical("Failed to save Base Libido Data");
        }

        const auto arousalMultiplierData = ArousalMultiplierData::GetSingleton();
        if (!arousalMultiplierData->Save(serializationInterface, kArousalMultiplierDataKey, kSerializationVersion)) {
            logger::critical("Failed to save Arousal Multiplier Data");
        }

        const auto lastCheckData = LastCheckTimeData::GetSingleton();
        if (!lastCheckData->Save(serializationInterface, kLastCheckTimeDataKey, kSerializationVersion)) {
            logger::critical("Failed to save Arousal Last Check Time Data");
        }

        const auto lastOrgasmData = LastOrgasmTimeData::GetSingleton();
        if (!lastOrgasmData->Save(serializationInterface, kLastOrgasmTimeDataKey, kSerializationVersion)) {
            logger::critical("Failed to save Last Orgasm Time Data");
        }

        const auto armorKeywordData = ArmorKeywordData::GetSingleton();
        if (!armorKeywordData->Save(serializationInterface, kArmorKeywordDataKey, kSerializationVersion)) {
            logger::critical("Failed to save Armor Keyword Data");
        }

        const auto lastEventData = LastEventData::GetSingleton();
        logger::info("Saving lastEventData");
        if (!lastEventData->Save(serializationInterface, kLastEventDataKey, kSerializationVersion)) {
            logger::critical("Failed to save Last Event Data");
        }

        logger::info("Saving Actor state data");
        if (!ActorStateManager::GetSingleton()->Save(serializationInterface)) {
            logger::critical("Failed to save Actor state Data");
        }

        logger::info("OSLArousal Data Saved");
    }

    void LoadCallback(SKSE::SerializationInterface *serializationInterface) {
        std::uint32_t type;
        std::uint32_t version;
        std::uint32_t length;
        logger::info("OSLArousal Load Start");

        while (serializationInterface->GetNextRecordInfo(type, version, length)) {
            logger::info("Trying Load for {}", DecodeTypeCode(type));

            if (version != kSerializationVersion) {
                logger::critical("Loaded data has incorrect version. Received ({}) for Data Key ({})"sv, version,
                                 DecodeTypeCode(type));
//                logger::critical("Loaded data has incorrect version. Received ({}) - Expected ({}) for Data Key ({})"sv,
//                                 version,  DecodeTypeCode(type).c_str());
                continue;
            }

            switch (type) {
                case kArousalDataKey: {
                    auto arousalData = ArousalData::GetSingleton();
                    if (!arousalData->Load(serializationInterface)) {
                        logger::critical("Failed to Load Arousal Data"sv);
                    }
                }
                    break;
                case kExposureDataKey: {
                    auto baseLibidoData = ExposureData::GetSingleton();
                    if (!baseLibidoData->Load(serializationInterface)) {
                        logger::critical("Failed to Load Base Libido Data"sv);
                    }
                }
                    break;
                case kArousalMultiplierDataKey: {
                    auto arousalMultiplierData = ArousalMultiplierData::GetSingleton();
                    if (!arousalMultiplierData->Load(serializationInterface)) {
                        logger::critical("Failed to Load Arousal Multiplier Data"sv);
                    }
                }
                    break;
                case kLastCheckTimeDataKey: {
                    auto lastCheckData = LastCheckTimeData::GetSingleton();
                    if (!lastCheckData->Load(serializationInterface)) {
                        logger::critical("Failed to Load LastCheckTime Data"sv);
                    }
                }
                    break;
                case kLastOrgasmTimeDataKey: {
                    auto lastOrgasmData = LastOrgasmTimeData::GetSingleton();
                    if (!lastOrgasmData->Load(serializationInterface)) {
                        logger::critical("Failed to Load LastOrgasm Data"sv);
                    }
                }
                    break;
                case kArmorKeywordDataKey: {
                    auto armorKeywordData = ArmorKeywordData::GetSingleton();
                    if (!armorKeywordData->Load(serializationInterface)) {
                        logger::critical("Failed to Load armorKeywordData Data"sv);
                    }
                }
                case kLastEventDataKey: {
                    auto lastEventData = LastEventData::GetSingleton();
                    logger::info("Loading lastEventData");
                    if (!lastEventData->Load(serializationInterface)) {
                        logger::critical("Failed to Load lastEventData Data"sv);
                    }
                }
                    break;
                case kActorStateDataKey: {
                    logger::info("Loading Actor State Data");
                    if (!ActorStateManager::GetSingleton()->Load(serializationInterface)) {
                        logger::critical("Failed to load Actor State Data"sv);
                    }
                }
                    break;
                default:
                    logger::critical("Unrecognized Record Type: {}"sv, DecodeTypeCode(type));
                    break;
            }
        }

        logger::info("OSLArousal Data loaded");
    }

    void RevertCallback(SKSE::SerializationInterface *) {
        auto arousalData = ArousalData::GetSingleton();
        arousalData->Clear();
        auto baseLibidoData = ExposureData::GetSingleton();
        baseLibidoData->Clear();
        auto arousalMultiplierData = ArousalMultiplierData::GetSingleton();
        arousalMultiplierData->Clear();
        auto lastCheckData = LastCheckTimeData::GetSingleton();
        lastCheckData->Clear();
        auto lastOrgasmData = LastOrgasmTimeData::GetSingleton();
        lastOrgasmData->Clear();
        auto armorKeywordData = ArmorKeywordData::GetSingleton();
        armorKeywordData->Clear();
        auto lastEventData = LastEventData::GetSingleton();
        lastEventData->Clear();

        //End All Scenes as well
        SceneManager::GetSingleton()->ClearScenes();

        logger::info("Reverting Data State...");
    }
}
