#include "config.h"

json Settings::globalSettings() {
	json json;
	json["language(soon...)"] = "ru";
	json["regionPrefix"] = "[Регионы]";
	json["canExplodeRegions(soon...)"] = "n";
	json["textForRgCommand"] = "Тут должна быть информация о создании региона игроком(какие блоки нужно использовать и тп)";
	return json;
}

json Settings::globalBlocks() {
	json json;
	json["myBlock"]["id"] = "minecraft:diamond_block";
	json["myBlock"]["radius"] = 15;
	return json;
}

void Settings::WriteDefaultConfigs() {
	std::ofstream settingsFileIn("plugins/Region/settings.json");
	if (!settingsFileIn.is_open()) {
		cfgLog.error("Невозможно открыть файл {}", "plugins/Region/settings.json");
		settingsFileIn.close();
		return;
	}
	settingsFileIn << globalSettings().dump(4);
	settingsFileIn.close();

	std::ifstream settingsFileOut("plugins/Region/settings.json");
	if (!settingsFileOut.is_open()) {
		cfgLog.error("Невозможно открыть файл {}", "plugins/Region/settings.json");
		settingsFileOut.close();
		return;
	}
	settingsFileOut >> settings;
	settingsFileOut.close();

	std::ofstream blocksFileIn("plugins/Region/blocks.json");
	if (!blocksFileIn.is_open()) {
		cfgLog.error("Невозможно открыть файл {}", "plugins/Region/blocks.json");
		blocksFileIn.close();
		return;
	}
	blocksFileIn << globalBlocks().dump(4); 
	blocksFileIn.close();

	std::ifstream blocksFileOut("plugins/Region/blocks.json");
	if (!blocksFileOut.is_open()) {
		cfgLog.error("Невозможно открыть файл {}", "plugins/Region/blocks.json");
		blocksFileOut.close();
		return;
	}
	blocksFileOut >> blocks;
	blocksFileOut.close();

    regionPrefix = settings["regionPrefix"];
}

void Settings::LoadConfigsFromJson() {
	std::ifstream settingsFileOut("plugins/Region/settings.json");
	if (!settingsFileOut.is_open()) {
		cfgLog.error("Невозможно открыть файл {}", "plugins/Region/settings.json");
		settingsFileOut.close();
		return;
	}
	settingsFileOut >> settings;
	settingsFileOut.close();

	std::ifstream blocksFileOut("plugins/Region/blocks.json");
	if (!blocksFileOut.is_open()) {
		cfgLog.error("Невозможно открыть файл {}", "plugins/Region/blocks.json");
		blocksFileOut.close();
		return;
	}
	blocksFileOut >> blocks;
	blocksFileOut.close();

	regionPrefix = settings["regionPrefix"];
}

void Settings::loadCfg() {

	if (!std::filesystem::exists("plugins/Region"))
		std::filesystem::create_directories("plugins/Region");

	if (std::filesystem::exists("plugins/Region/settings.json") && std::filesystem::exists("plugins/Region/blocks.json")) {
		try {
			LoadConfigsFromJson();
		}
		catch (std::exception& e) {
			cfgLog.error("Невозможно получить конфигурацию файла! Ошибка: {}", e.what());
		}
		catch (...) {
			cfgLog.error("Неверная конфигурация файла!");
		}

		if (!settings["language(soon...)"].is_string() || !settings["canExplodeRegions(soon...)"].is_string() || blocks.empty())
			cfgLog.error("Ошибки в оформлении конфига:\nLanguage должен быть строкой;\ncanExplodeRegions должно быть строковым типом;\n \
				конфиг-файл blocks не должен быть пустым.");
	}
	else
		WriteDefaultConfigs();
}


// This function'll working soon!
bool Settings::isExplodeOn() {
	return settings["canExplodeRegions(soon...)"] == "y" || settings["canExplodeRegions(soon...)"] == "Y";
}

// This function'll working soon!
string Settings::getLanguage() {
	return settings["language(soon...)"];
}

int Settings::getBlockRadius(string block_id) {
	for (const auto& x : blocks) {
		if (x["id"] == block_id) return x["radius"];
	}
	return 0;
}

bool Settings::issetBlock(string block_id) {
	for (const auto& x : blocks) {
		if (x["id"] == block_id) return true;
	}
	return false;
}