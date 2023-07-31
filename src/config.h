#pragma once

#include <Nlohmann/json.hpp>
#include <llapi/LoggerAPI.h>

using namespace nlohmann;

inline Logger cfgLog("RegionConfig");
inline json settings;
inline json blocks;

namespace Settings {
	inline string regionPrefix;
	json globalSettings();
	json globalBlocks();
	void WriteDefaultConfigs();
	void LoadConfigsFromJson();
	void loadCfg();
	bool isExplodeOn();
	int getBlockRadius(string block_id);
	bool issetBlock(string block_id);
	string getLanguage();
}