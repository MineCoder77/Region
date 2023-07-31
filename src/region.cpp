#include "command.h"
#include "database.h"
#include "config.h"

#include <regex>

#include <llapi/mc/ColorFormat.hpp>
#include <llapi/FormUI.h>
#include <llapi/mc/Block.hpp>
#include <llapi/EventAPI.h>
#include <llapi/mc/Level.hpp>
#include <llapi/mc/ItemStack.hpp>

std::set<double> RegionData::getCoords() const {
	return coords;
}

bool RegionData::isOwner(string playerName) const {
	return ownerName == playerName;
}

vector<RegionMemberData*> RegionData::getMembers() const {
	SQLite::Statement selectStatement{ *db, "SELECT * FROM regionMembers WHERE regionName = ?;" };
	selectStatement.bindNoCopy(1, regionName);
	return queryMemberData(selectStatement);
}

bool RegionData::isMember(string playerName) const {
	vector<RegionMemberData*> membersData = getMembers();
	for (const auto& x : membersData) {
		if (x->getName() == playerName) {
			deleteVector(membersData);
			return true;
		}
	}
	deleteVector(membersData);
	return false;
}

string RegionData::getWorldName() const {
	return worldName;
}

RegionMemberData* RegionData::getMember(std::string playerName) const {
	vector<RegionMemberData*> membersData = getMembers();
	for (const auto& x : membersData) {
		if (x->getName() == playerName) {
			deleteVector(membersData);
			return x;
		}
	}
	deleteVector(membersData);
	return nullptr;
}

string RegionData::getName() const {
	return regionName;
}

void RegionData::addMember(string playerName) const {
	SQLite::Statement insertStatement{ *db, "INSERT INTO regionMembers(regionName, playerName, permissionLevel) VALUES (?, ?, ?);" };
	insertStatement.bindNoCopy(1, regionName);
	insertStatement.bindNoCopy(2, playerName);
	insertStatement.bindNoCopy(3, "member");
	simpleQuery(insertStatement);
}

void RegionData::removeMember(string playerName) const {
	SQLite::Statement deleteStatement{ *db, "DELETE FROM regionMembers WHERE playerName = ?;" };
	deleteStatement.bindNoCopy(1, playerName);
	simpleQuery(deleteStatement);
}

// The function delete all information about region.
void RegionData::_delete() {
	SQLite::Statement firstDeleteStatement{ *db, "DELETE FROM regions WHERE regionName = ?;"};
	firstDeleteStatement.bindNoCopy(1, regionName);
	SQLite::Statement secondDeleteStatement{ *db, "DELETE FROM regionMembers WHERE regionName = ?; " };
	secondDeleteStatement.bindNoCopy(1, regionName);
	simpleQuery(firstDeleteStatement);
	simpleQuery(secondDeleteStatement);
}

bool isValidRegionName(string regionName) {
	std::regex pattern("^[a-zA-Z0-9_]+$");
	return regex_match(regionName, pattern);
}

bool isNewRegionIncludeOther(BlockPos block, string blockId, string &worldName) {
	int radius = Settings::getBlockRadius(blockId);
	SQLite::Statement selectStatement{ *db, "SELECT * FROM regions WHERE (maxX >= ? AND minX <= ?) AND (maxY >= ? AND minY <= ?) AND (maxZ >= ? AND minZ <= ?) AND world = ?;" };
	selectStatement.bind(1, float(block.x - radius));
	selectStatement.bind(2, float(block.x + radius));
	selectStatement.bind(3, float(block.y - radius));
	selectStatement.bind(4, float(block.y + radius));
	selectStatement.bind(5, float(block.z - radius));
	selectStatement.bind(6, float(block.z + radius));
	selectStatement.bindNoCopy(7, worldName);
	return queryRegionData(selectStatement) != nullptr;
}

bool isRegionNameExists(string regionName) {
	SQLite::Statement selectStatement{ *db, "SELECT * FROM regions WHERE regionName = ?;" };
	selectStatement.bindNoCopy(1, regionName);
	return queryRegionData(selectStatement) != nullptr;
}

// The function checking count of regions, which created by user.
bool isCanCreate(Player* &player) {
	SQLite::Statement selectStatement{ *db, "SELECT * FROM regionMembers WHERE playerName = ? AND permissionLevel = ?;" };
	selectStatement.bindNoCopy(1, player->getRealName());
	selectStatement.bindNoCopy(2, "owner");
	vector<RegionMemberData*> response = queryMemberData(selectStatement);
	bool isEmptyMas = response.empty();
	size_t sizeMas = response.size();
	deleteVector(response);
	if (!isEmptyMas) return sizeMas < 3;
	else return false;
}

void createRegion(Player* &player, string regionName, BlockPos pos, string blockId) {
	SQLite::Statement insertRegionStatement{ *db, "INSERT INTO regions(regionName, ownerName, minX, minY, minZ, maxX, maxY, maxZ, world) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?);" };
	string playerName = player->getRealName();
	int radius = Settings::getBlockRadius(blockId);
	float minX = pos.x - radius;
	float maxX = pos.x + radius;
	float minY = pos.y - radius;
	float maxY = pos.y + radius;
	float minZ = pos.z - radius;
	float maxZ = pos.z + radius;
	insertRegionStatement.bindNoCopy(1, regionName);
	insertRegionStatement.bindNoCopy(2, playerName);
	insertRegionStatement.bind(3, minX);
	insertRegionStatement.bind(4, minY);
	insertRegionStatement.bind(5, minZ);
	insertRegionStatement.bind(6, maxX);
	insertRegionStatement.bind(7, maxY);
	insertRegionStatement.bind(8, maxZ);
	insertRegionStatement.bindNoCopy(9, player->getLevel().getCurrentLevelName());
	simpleQuery(insertRegionStatement);

	SQLite::Statement insertMemberStatement{ *db, "INSERT INTO regionMembers(regionName, playerName, permissionLevel) VALUES (?, ?, ?);" };
	insertMemberStatement.bindNoCopy(1, regionName);
	insertMemberStatement.bindNoCopy(2, playerName);
	insertMemberStatement.bindNoCopy(3, "owner");
	simpleQuery(insertMemberStatement);
}

RegionData* getRegionByBlockPos(BlockPos pos, string worldName) {
	SQLite::Statement selectStatement{ *db, "SELECT * FROM regions WHERE (minX <= ? AND ? <= maxX) AND (minY <= ? AND ? <= maxY) AND (minZ <= ? AND ? <= maxZ) AND world = ?;" };
	selectStatement.bind(1, float(pos.x));
	selectStatement.bind(2, float(pos.x));
	selectStatement.bind(3, float(pos.y));
	selectStatement.bind(4, float(pos.y));
	selectStatement.bind(5, float(pos.z));
	selectStatement.bind(6, float(pos.z));
	selectStatement.bindNoCopy(7, worldName);
	RegionData* response = queryRegionData(selectStatement);
	return response;
}

RegionData* getRegionByVector(Vec3 vec, string worldName) {
	SQLite::Statement selectStatement{ *db, "SELECT * FROM regions WHERE (minX <= ? AND ? <= maxX) AND (minY <= ? AND ? <= maxY) AND (minZ <= ? AND ? <= maxZ) AND world = ?;" };
	selectStatement.bind(1, vec.x);
	selectStatement.bind(2, vec.x);
	selectStatement.bind(3, vec.y);
	selectStatement.bind(4, vec.y);
	selectStatement.bind(5, vec.z);
	selectStatement.bind(6, vec.z);
	selectStatement.bindNoCopy(7, worldName);
	RegionData* response = queryRegionData(selectStatement);
	return response;
}

RegionData* getRegionByName(const string& regionName) {
	SQLite::Statement selectStatement{ *db, "SELECT * FROM regions WHERE regionName = ?;" };
	selectStatement.bindNoCopy(1, regionName);
	RegionData* response = queryRegionData(selectStatement);
	return response;
}

bool playerExists(const string playerName) {
	SQLite::Statement selectStatement{ *db, "SELECT * FROM regions WHERE ownerName = ?" }; 
	selectStatement.bindNoCopy(1, playerName);
	while (selectStatement.executeStep())
		if (!selectStatement.getColumn(1).getString().empty()) return true;
	return false;
}

namespace RegionEvents {

	// The vector is needed to track the players that privatize the place. 
	vector<string> listPlayers;

	// The function removes players from the vector. 
	void stopInteraction(Player* &player) {
		listPlayers.erase(std::remove(listPlayers.begin(), listPlayers.end(), player->getRealName()), listPlayers.end());
	}

	void registerEvents() {
		Event::BlockPlacedByPlayerEvent::subscribe([](const Event::BlockPlacedByPlayerEvent& event) {
			Player* player = event.mPlayer;
			BlockInstance blockInstance = event.mBlockInstance;
			string levelName = player->getLevel().getCurrentLevelName();
			if (Settings::issetBlock(blockInstance.getBlock()->getName().getString())) {
				if (std::find(listPlayers.begin(), listPlayers.end(), player->getRealName()) != listPlayers.end())
					return false;
				if (isNewRegionIncludeOther(blockInstance.getPosition(), blockInstance.getBlock()->getName().getString(), levelName)) {
					BlockSource* bs = blockInstance.getBlockSource();
					Global<Level>->destroyBlock(*bs, blockInstance.getPosition(), true);
					player->sendText(Settings::regionPrefix + ColorFormat::RED + " Приват не может быть создан, т.к границы этого региона пересекают границы другого!");
					return false;
				}
				listPlayers.push_back(player->getRealName());
				Form::CustomForm form("Регионы");
				form.addLabel(
					" ",
					"Требования к названию региона:\nДлина до 8 символов; \nРазрешено использовать латинский алфавит, а также цифры от 0 до 9!"
				);
				BlockPos blockPos = blockInstance.getPosition();
				string blockId = blockInstance.getBlock()->getName().getString();
				form.addInput("Название региона", "Введите название региона", "MyRegion");
				form.sendTo(player, [blockPos, blockId](Player* player, auto result) {
					if (result.empty()) {
						stopInteraction(player);
						return;
					}
					if (result["Название региона"]->getString().empty()) {
						stopInteraction(player);
						player->sendFormattedText(ColorFormat::YELLOW + "Уважаемый {}, название региона не может быть пустым!", player->getRealName());
						return;
					}
					string regionName = result["Название региона"]->getString();
					if (!isValidRegionName(regionName)) {
						stopInteraction(player);
						player->sendText(ColorFormat::RED + "В названии региона не может быть что-то кроме латинских букв, цифр и пробелов!");
						return;
					}
					if (regionName.length() > 8) {
						stopInteraction(player);
						player->sendText(ColorFormat::RED + "Название региона не может быть больше 8 символов!");
						return;
					}
					if (isRegionNameExists(regionName)) {
						stopInteraction(player);
						player->sendFormattedText(ColorFormat::RED + "Название региона {} уже существует!", regionName);
						return;
					}
					if (playerExists(player->getRealName())) {
						if (!isCanCreate(player)) {
							stopInteraction(player);
							player->sendText(Settings::regionPrefix + ColorFormat::RED + " Ты превысил максимальное кол-во приватов!");
							return;
						}
					}
					createRegion(player, regionName, blockPos, blockId);
					player->sendFormattedText(ColorFormat::GREEN + "Регион {} успешно создан!", regionName);
					stopInteraction(player);
				});
			}
			return true;
		});
		Event::PlayerPlaceBlockEvent::subscribe([](const Event::PlayerPlaceBlockEvent& event) {
			Player* player = event.mPlayer;
			BlockInstance blockInstance = event.mBlockInstance;
			string levelName = player->getLevel().getCurrentLevelName();
			RegionData* region = getRegionByBlockPos(blockInstance.getPosition(), levelName);
			if (region != nullptr) {
				if (!region->isOwner(player->getRealName()) && !region->isMember(player->getRealName())) {
					player->sendText(ColorFormat::RED + "Запрещено!");
					delete region;
					return false;
				}
			}
			return true;
		});
		Event::PlayerDestroyBlockEvent::subscribe([](const Event::PlayerDestroyBlockEvent& event) {
			Player* player = event.mPlayer;
			BlockInstance blockInstance = event.mBlockInstance;
			string levelName = player->getLevel().getCurrentLevelName();
			RegionData* region = getRegionByBlockPos(blockInstance.getPosition(), levelName);
			if (region != nullptr) {
				if (!Settings::getBlockRadius(blockInstance.getBlock()->getName().getString()) && !region->isOwner(player->getRealName()) && !region->isMember(player->getRealName())) {
					player->sendText(ColorFormat::RED + "Запрещено!");
					delete region;
					return false;
				}
				if (Settings::getBlockRadius(blockInstance.getBlock()->getName().getString())) {
					if (!region->isOwner(player->getRealName())) {
						player->sendFormattedText(ColorFormat::RED + "Ты не можешь удалить регион {}, так как ты не его владелец!", region->getName());
						delete region;
						return false;
					}
					region->_delete();
					player->sendFormattedText(ColorFormat::GREEN + "Ты удалил регион {}", region->getName());
					delete region;
				}
			}
			return true;
		});
		Event::PlayerAttackEvent::subscribe([](const Event::PlayerAttackEvent& event) {
			if (!event.mTarget->isPlayer()) return true;
			Player* player = event.mPlayer;
			string levelName = player->getLevel().getCurrentLevelName();
			Vec3 pos = player->getPos();
			RegionData* region = getRegionByVector(pos, levelName);
			if (region != nullptr) {
				player->sendText(ColorFormat::RED + "Запрещено!");
				delete region;
				return false;
			}
			return true;
		});
		Event::PlayerUseItemOnEvent::subscribe([](const Event::PlayerUseItemOnEvent& event) {
			Player* player = event.mPlayer;
			string levelName = player->getLevel().getCurrentLevelName();
			BlockInstance blockInstance = event.mBlockInstance;
			RegionData* region = getRegionByBlockPos(blockInstance.getPosition(), levelName);
			if (region != nullptr) {
				if (!region->isOwner(player->getRealName()) && !region->isMember(player->getRealName())) {
					player->sendText(ColorFormat::RED + "Запрещено!");
					delete region;
					return false;
				}
			}
			return true;
		});
		Event::RegCmdEvent::subscribe([](Event::RegCmdEvent ev) {
			RegionCommand::setup(ev.mCommandRegistry);
			return true;
		});
	}
}

string RegionMemberData::getName() const {
	return playerName;
}

// The function collecting information about permissions users(member || owner).
string RegionMemberData::getPermission() const {
	return permLevel;
}

// Delete all information about user.
void RegionMemberData::_delete() {
	SQLite::Statement deleteStatement{ *db, "DELETE FROM regionMembers WHERE regionName = ? AND playerName = ?;" };
	deleteStatement.bindNoCopy(1, regionName);
	deleteStatement.bindNoCopy(2, playerName);
    simpleQuery(deleteStatement);
}
