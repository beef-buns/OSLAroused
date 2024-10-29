#pragma once

#include <utility>

#include "PCH.h"

class Config : public ISingleton<Config> {
public:
    struct KeywordEntry {
        RE::FormID FormId;
        std::string EditorId;

        KeywordEntry(RE::FormID formId, std::string editorId)
                : FormId(formId), EditorId(std::move(editorId)) {
        }
    };

    void LoadINIs();

    std::map<std::string, RE::FormID> GetRegisteredKeywords() {
        return m_RegisteredKeywordEditorIds;
    }

    bool RegisterKeyword(std::string keywordEditorId);

private:
    bool LoadINI(std::string filename);

    bool m_ConfigLoaded;

    std::map<std::string, RE::FormID> m_RegisteredKeywordEditorIds;
};
