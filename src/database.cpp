#include "database.h"

// The function deleting vector with objects which created using dynamic memory.
void deleteVector(vector<RegionMemberData*>& vec) {
	for (auto ptr : vec) {
		delete ptr;
	}
	vec.clear();
}

// The function collecting information from db to vector.
vector<RegionMemberData*> queryMemberData(SQLite::Statement& statement) {
	vector<RegionMemberData*> rmdV;
	while (statement.executeStep()) {
		RegionMemberData* rmd = new RegionMemberData(statement.getColumn(2).getString(), statement.getColumn(1).getString(), statement.getColumn(3).getString());
		rmdV.push_back(rmd);
	}
	statement.reset();
	statement.clearBindings();
	return rmdV;
}

// The function collecting information from db to RegionData object.
RegionData* queryRegionData(SQLite::Statement& statement) {
	std::set<double> coords;
	while (statement.executeStep()) {
		for (int i = 2; i < 8; i++)
			coords.insert(statement.getColumn(i).getDouble());
		RegionData* rd = new RegionData(statement.getColumn(1).getString(), statement.getColumn(2).getString(), coords, statement.getColumn(8).getString());
		statement.reset();
		statement.clearBindings();
		return rd;
	}
	return nullptr;
}

void simpleQuery(SQLite::Statement& statement) {
	try {
		statement.exec();
	}
	catch (std::exception const& e) {
		dbLog.error("Ошибка запроса: {}", e.what());
	}
	statement.reset();
	statement.clearBindings();
}

bool initDB() {
	try {
		db = std::make_unique<SQLite::Database>("plugins\\Region\\database.db",
			SQLite::OPEN_CREATE | SQLite::OPEN_READWRITE);
		db->exec("PRAGMA journal_mode = MEMORY");
		db->exec("PRAGMA synchronous = NORMAL");
		db->exec("CREATE TABLE IF NOT EXISTS regions ( \
			id INTEGER PRIMARY KEY AUTOINCREMENT, \
			regionName VARCHAT(8) NOT NULL, \
			ownerName TEXT NOT NULL, \
			minX FLOAT NOT NULL, \
			minY FLOAT NOT NULL, \
			minZ FLOAT NOT NULL, \
			maxX FLOAT NOT NULL, \
			maxY FLOAT NOT NULL, \
			maxZ FLOAT NOT NULL, \
			world TEXT NOT NULL \
			);");
		db->exec("CREATE TABLE IF NOT EXISTS regionMembers ( \
			id INTEGER PRIMARY KEY AUTOINCREMENT, \
			regionName VARCHAT(8) NOT NULL, \
			playerName TEXT NOT NULL, \
			permissionLevel TEXT NOT NULL \
			);");
	}
	catch (std::exception const& e) {
		dbLog.error("Ошибка бд: {}", e.what());
		return false;
	}
	dbLog.info("База данных готова к работе!");
	return true;
}