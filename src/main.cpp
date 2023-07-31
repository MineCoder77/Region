#include "pch.h"

inline Logger mainLog("Region");

void PluginInit() {
	Settings::loadCfg();
	if (!initDB()) return;
	RegionEvents::registerEvents();
	mainLog.info("Плагин успешно запустился и готов к работе!");
}