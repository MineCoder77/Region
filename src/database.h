#pragma once

#include <SQLiteCpp/SQLiteCpp.h>
#include <llapi/LoggerAPI.h>

#include "region.h"

inline Logger dbLog("RegionDatabase");

inline std::unique_ptr<SQLite::Database> db;
bool initDB();

void deleteVector(vector<RegionMemberData*>& vec);
vector<RegionMemberData*> queryMemberData(SQLite::Statement& statement);
RegionData* queryRegionData(SQLite::Statement& statement);
void simpleQuery(SQLite::Statement& statement);