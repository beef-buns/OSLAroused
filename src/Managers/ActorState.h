#pragma once

namespace Utilities {
    float GenerateRandomFloat(float min, float max);
}

namespace Arousal {
    struct ActorState {
        float arousal{0.0f};
        float timeRate{1.0f};
        float exposure{-1.0f};
        float exposureRate{1.0f};
        float lastOrgasm{-1.0f};
        float lastUpdate{-1.0f};

        /* TODO: Sum(SingleExposure * ExposureRate) */
        std::map<std::string_view, float> exposureModifiers{};

        [[nodiscard]] inline float GetArousal() const { return arousal; }

        [[nodiscard]] inline float GetExposure() const { return exposure; }

        [[nodiscard]] inline float GetExposureRate() const { return exposureRate; }

        [[nodiscard]] inline float GetTimeRate() const { return timeRate; }

        [[nodiscard]] inline float GetLastUpdate() const { return lastUpdate; }

        inline void SetExposure(float val) { exposure = std::clamp(val, 0.f, 100.f); };

        inline void SetExposureModifier(const std::string_view &name, float value) {
            exposureModifiers[name] = value;
        }

        inline void RemoveExposureModifier(const std::string_view &name) {
            exposureModifiers.erase(name);
        }

        [[nodiscard]] inline float GetTimeSinceLastOrgasm() const {
            auto delta = RE::Calendar::GetSingleton()->GetCurrentGameTime() - lastOrgasm;
            return std::max(delta, 0.f);
        }

        ActorState() : exposure(0.f) {
            auto currentTime = RE::Calendar::GetSingleton()->GetCurrentGameTime();
            lastOrgasm = Utilities::GenerateRandomFloat(currentTime - 7.0f, currentTime);

            CalculateArousal();

            lastUpdate = currentTime;
        }

        inline void CalculateArousal() {
            float gameDaysPassed = GetTimeSinceLastOrgasm();

            CalculateExposure();

            arousal = std::clamp((gameDaysPassed * timeRate) + exposure, 0.f, 100.f);
        }

        inline void CalculateExposure() {
            exposure = 0.0f;
            for (const auto &[modifierName, modifierValue]: exposureModifiers) {
                exposure += modifierValue * exposureRate;
            }
        }

        inline bool Load(SKSE::SerializationInterface *a_intfc) {
            a_intfc->ReadRecordData(arousal);
            a_intfc->ReadRecordData(timeRate);
            a_intfc->ReadRecordData(exposure);
            a_intfc->ReadRecordData(exposureRate);
            a_intfc->ReadRecordData(lastOrgasm);
            a_intfc->ReadRecordData(lastUpdate);

            return true;
        }

        inline bool Save(SKSE::SerializationInterface *a_intfc) {
            if (!a_intfc->WriteRecordData(arousal)) {
                logger::error("Failed to save arousal ({})", arousal);
                return false;
            }
            if (!a_intfc->WriteRecordData(timeRate)) {
                logger::error("Failed to save timeRate ({})", timeRate);
                return false;
            }
            if (!a_intfc->WriteRecordData(exposure)) {
                logger::error("Failed to save exposure ({})", exposure);
                return false;
            }
            if (!a_intfc->WriteRecordData(exposureRate)) {
                logger::error("Failed to save exposureRate ({})", exposureRate);
                return false;
            }
            if (!a_intfc->WriteRecordData(lastOrgasm)) {
                logger::error("Failed to save lastOrgasm ({})", lastOrgasm);
                return false;
            }
            if (!a_intfc->WriteRecordData(lastUpdate)) {
                logger::error("Failed to save lastUpdate ({})", lastUpdate);
                return false;
            }

            return true;
        }
    };
}
