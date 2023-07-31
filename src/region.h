#pragma once

#include <llapi/mc/BlockPos.hpp>
#include <llapi/mc/Player.hpp>

// Class collecting information about members in region.
class RegionMemberData {
private:
	string playerName;
	string regionName;
	string permLevel;
public:
	string getName() const;
	string getPermission() const;
	void _delete();
	RegionMemberData(string playerName, string regionName, string permLevel)
		: playerName(playerName), regionName(regionName), permLevel(permLevel) {}
};

// Class collecting full region information, but without members.
class RegionData {
private:
	std::set<double> coords;
	string regionName;
	string ownerName;
	string worldName;
public:
	std::set<double> getCoords() const;
	bool isOwner(string playerName) const;
	bool isMember(string playerName) const;
	string getWorldName() const;
	vector<RegionMemberData*> getMembers() const;
	RegionMemberData* getMember(string playerName) const;
	string getName() const;
	void addMember(string playerName) const;
	void _delete();
	void removeMember(string playerName) const;
	RegionData(string regionName, string ownerName, std::set<double> coords, string worldName)
		: coords(coords), regionName(regionName), ownerName(ownerName), worldName(worldName) {}
};

RegionData* getRegionByBlockPos(BlockPos pos, string worldName);
RegionData* getRegionByVector(Vec3 vec, string worldName);
RegionData* getRegionByName(const string& regionName);

bool isValidRegionName(string regionName);
bool isNewRegionIncludeOther(BlockPos block, string blockId, string& worldName);
bool isRegionNameExists(string name);
bool isCanCreate(Player*& player);
void createRegion(Player*& player, string name, BlockPos pos, string blockId);

namespace RegionEvents {
	void registerEvents();
}
