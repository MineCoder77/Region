#include "command.h"
#include "config.h"
#include "database.h"

#include <llapi/mc/Level.hpp>
#include <llapi/mc/ColorFormat.hpp>
#include <llapi/FormUI.h>

string trim(const string& str) {
	size_t first = str.find_first_not_of(' ');
	if (string::npos == first)
		return str;
	size_t last = str.find_last_not_of(' ');
	return str.substr(first, (last - first + 1));
}

void RegionCommand::execute(CommandOrigin const& ori, CommandOutput& output) const {
	if (ori.getOriginType() != (CommandOriginType)OriginType::Player) {
		output.error("Команда используется только в игре!");
		return;
	}
	switch (arg) {
		case info:
		{
			if (!recipientNameIsset && !regionNameIsset) {
				RegionData* region = getRegionByVector(ori.getPlayer()->getPos(), ori.getPlayer()->getLevel().getCurrentLevelName());
				if (region != nullptr) {
					string allMemberNames;
					vector<RegionMemberData*> members = region->getMembers();
					string entryString;
					int count = 0;
					for (int i = 0; i < members.size(); i++) {
						string permission = members[i]->getPermission();
						string entry;
						if (permission == "owner") {
							entry = "\n" + std::to_string(count) + ". " + ColorFormat::GREEN + members[i]->getName() + " — Владелец, ";
							entryString.insert(0, entry);
						}
						else {
							entry = "\n" + std::to_string(count) + ". " + members[i]->getName() + " — Участник, ";
							entryString.append(entry);
						}
						count++;
					}
					allMemberNames = trim(entryString);
					allMemberNames.erase(allMemberNames.size() - 1);
					output.success(
						Settings::regionPrefix + " Информация о регионе: " + ColorFormat::GREEN + region->getName() +
						"\n" + ColorFormat::GRAY + "•" + ColorFormat::RESET + " Название: " + ColorFormat::GREEN + region->getName() +
						"\n" + ColorFormat::GRAY + "•" + ColorFormat::RESET + " Участники[" + std::to_string(count) + "]: " + ColorFormat::GOLD + allMemberNames
					);
					delete region;
					deleteVector(members);
					break;
				}
				else {
					output.error(Settings::regionPrefix + " Регионов в этом месте нет.");
					break;
				}
			}
			else {
				output.error("Используй: /rg info");
				break;
			}
		}
		case addmember:
		{
			if (recipientNameIsset && regionNameIsset) {
				// Add player without form
				Player* recipient = Global<Level>->getPlayer(recipientName);
				RegionData* region = getRegionByName(regionName);
				if (region == nullptr) {
					output.error(Settings::regionPrefix + " Региона " + ColorFormat::GREEN + regionName + ColorFormat::RED + " не существует!");
					break;
				}
				if (recipient != nullptr) {
					if (!region->isOwner(ori.getPlayer()->getRealName())) {
						output.error(Settings::regionPrefix + " Ты не можешь добавить участников в этот регион, т.к ты не являешься владельцем!");
						delete region;
						break;
					}
					if (ori.getPlayer()->getRealName() == recipientName) {
						output.error(Settings::regionPrefix + " Ты не можешь добавить самого себя в регион!");
						delete region;
						break;
					}
					if (region->isMember(recipientName)) {
						output.error(Settings::regionPrefix + " Этот игрок уже находится в регионе!");
						delete region;
						break;
					}
					region->addMember(recipientName);
					output.success(Settings::regionPrefix + " Игрок " + recipientName + " успешно добавлен в регион!");
					recipient->sendFormattedText("{} Вы были добавлены в регион {} игроком {}", Settings::regionPrefix, region->getName(), ori.getPlayer()->getRealName());
					delete region;
					break;
				}
				else {
					output.error(Settings::regionPrefix + " Игрока " + recipientName + " нет в сети!");
					break;
				}
			}
			else {
				output.error(Settings::regionPrefix + " Используйте строковые аргументы(/rg addmember <регион> <ник>\nВ данной версии недоступна функция добавления игрока в приват с помощью формы!");
				break;
			}
		}
		case removemember:
		{
			if (recipientNameIsset && regionNameIsset) {
				// Delete player without form
				Player* recipient = Global<Level>->getPlayer(recipientName);
				RegionData* region = getRegionByName(regionName);
				if (region == nullptr) {
					output.error(Settings::regionPrefix + " Региона " + ColorFormat::GREEN + regionName + ColorFormat::RED + " не существует!");
					break;
				}
				if (recipient != nullptr) {
					if (!region->isOwner(ori.getPlayer()->getRealName())) {
						output.error(Settings::regionPrefix + " Ты не можешь удалять участников из этого региона, т.к ты не являешься владельцем!");
						delete region;
						break;
					}
					if (ori.getPlayer()->getRealName() == recipientName) {
						output.error(Settings::regionPrefix + " Ты не можешь удалить самого себя из региона!");
						delete region;
						break;
					}
					if (!region->isMember(recipientName) && !region->isOwner(recipientName)) {
						output.error(Settings::regionPrefix + " Игрок " + recipientName + " не является участником региона!");
						delete region;
						break;
					}
					else {
						region->getMember(recipientName)->_delete();
						output.success(Settings::regionPrefix + " Ты успешно удалил игрока " + recipientName + " из региона!");
						recipient->sendFormattedText("{} Вы были удалены из региона {} игроком {}", Settings::regionPrefix, region->getName(), ori.getPlayer()->getRealName());
						delete region;
						break;
					}
				}
				else {
					if (!region->isMember(recipientName) && !region->isOwner(recipientName))
						output.error(Settings::regionPrefix + " Игрок " + recipientName + " не является участником региона!");
					else {
						region->getMember(recipientName)->_delete();
						output.success(Settings::regionPrefix + " Ты успешно удалил игрока " + recipientName + " из региона!");
					}
					delete region;
					break;
				}
			}
			else {
				output.error(Settings::regionPrefix + " Используйте строковые аргументы(/rg removemember <регион> <ник>)\nВ данной версии недоступна функция удаления игрока из привата с помощью формы!");
				break;
			}
		}
		case help:
		{
			if (!recipientNameIsset && !regionNameIsset) {
				Form::CustomForm form("Регионы");
				form.addLabel("Информация о создании привата: ", settings["textForRgCommand"]);
				form.sendTo(ori.getPlayer(), [](Player* player, auto result) {});
			} 
			else
				output.error("Используй: /rg help");
			break;
		}
	}
}

void RegionCommand::setup(CommandRegistry* registry) {
	registry->registerCommand("rg", "Регионы",
		CommandPermissionLevel::Any, { (CommandFlagValue)0 }, { (CommandFlagValue)0x80 });
	registry->addEnum<Args>("Args", { {"info", Args::info}, {"addmember", Args::addmember}, {"removemember", Args::removemember}, {"help", Args::help} });
	registry->registerOverload<RegionCommand>("rg", RegisterCommandHelper::makeMandatory<CommandParameterDataType::ENUM>(&RegionCommand::arg, "Аргументы", "Args"),
		RegisterCommandHelper::makeOptional(&RegionCommand::regionName, "Название региона", &RegionCommand::regionNameIsset), 
		RegisterCommandHelper::makeOptional(&RegionCommand::recipientName, "Ник игрока", &RegionCommand::recipientNameIsset));
}