#include "main.hpp"

#include "UnityEngine/SceneManagement/SceneManager.hpp"
#include "UnityEngine/SceneManagement/LoadSceneMode.hpp"
#include "UnityEngine/Camera.hpp"
#include "UnityEngine/QualitySettings.hpp"
#include "UnityEngine/SceneManagement/Scene.hpp"
#include "questui/shared/QuestUI.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/IncrementSetting.hpp"
#include "questui/shared/CustomTypes/Components/IncrementSettingValues.hpp"
#include "questui/shared/CustomTypes/Components/SliderSetting.hpp"
#include "questui/shared/CustomTypes/Components/SliderSettingValues.hpp"
#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"
#include "beatsaber-hook/shared/config/Configuration.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/utils/utils.h"

#include <string>

using namespace UnityEngine;
using namespace UnityEngine::SceneManagement;
using namespace QuestUI;
using namespace QuestUI::BeatSaberUI;
using namespace QuestUI::CustomTypes::Components;

static Configuration config{"CamPro"};

struct ModSettings {
    float menuFOV = 70.0f;
    float gameFOV = 90.0f;
    int aaLevel = 4;
};

static ModSettings settings;

#define INFO(value, ...) getLogger().info(value, ##__VA_ARGS__)

void SaveSettings() {
    config.SetFloat("menuFOV", settings.menuFOV);
    config.SetFloat("gameFOV", settings.gameFOV);
    config.SetInt("aaLevel", settings.aaLevel);
    config.Save();
}

void LoadSettings() {
    settings.menuFOV = config.GetFloat("menuFOV", settings.menuFOV);
    settings.gameFOV = config.GetFloat("gameFOV", settings.gameFOV);
    settings.aaLevel = config.GetInt("aaLevel", settings.aaLevel);
}

void ApplySettingsForScene(const Scene& scene) {
    auto sceneName = scene.get_name();
    auto mainCamera = Camera::get_main();
    if (!mainCamera) {
        INFO("CamPro: main camera not found for scene %s", sceneName.c_str());
        return;
    }

    if (sceneName == "MainMenu") {
        mainCamera->set_fieldOfView(settings.menuFOV);
        INFO("CamPro: Applied Menu FOV %.1f", settings.menuFOV);
    } else if (sceneName == "GameCore") {
        mainCamera->set_fieldOfView(settings.gameFOV);
        INFO("CamPro: Applied Game FOV %.1f", settings.gameFOV);
    }

    QualitySettings::set_antiAliasing(settings.aaLevel);
    INFO("CamPro: Applied MSAA %d", settings.aaLevel);
}

MAKE_HOOK_MATCH(SceneManager_Internal_SceneLoaded, &SceneManager::Internal_SceneLoaded, void, Scene scene, LoadSceneMode mode) {
    SceneManager_Internal_SceneLoaded(scene, mode);
    ApplySettingsForScene(scene);
}

namespace CamPro {
    static std::optional<HMUI::ModalView*> modMenuView;

    static void SetupUI() {
        QuestUI::Register::RegisterModMenuSettingsViewController("CamPro", []() {
            auto* container = MakeScrollableSettingContainer();

            auto* menuFovSlider = CreateSliderSetting(container, "Menu FOV", settings.menuFOV, 50.0f, 110.0f, 1.0f, [](float value) {
                settings.menuFOV = value;
                SaveSettings();
            });

            auto* gameFovSlider = CreateSliderSetting(container, "Game FOV", settings.gameFOV, 50.0f, 130.0f, 1.0f, [](float value) {
                settings.gameFOV = value;
                SaveSettings();
            });

            auto* aaIncrement = CreateIncrementSetting(container, "Anti-Aliasing (MSAA)", std::vector<int>{0, 2, 4, 8}, settings.aaLevel, [](int value) {
                settings.aaLevel = value;
                SaveSettings();
            });

            return container;
        });
    }
}

extern "C" void setup(ModInfo& info) {
    info.id = "campro";
    info.version = "1.0.0";
}

extern "C" void load() {
    LoadSettings();
    CamPro::SetupUI();
    INFO("CamPro: settings loaded. menuFOV=%.1f gameFOV=%.1f aa=%d", settings.menuFOV, settings.gameFOV, settings.aaLevel);
}

extern "C" void initialize() {
    INSTALL_HOOK(getLogger(), SceneManager_Internal_SceneLoaded);
}
