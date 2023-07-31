#pragma once

#include <llapi/RegCommandAPI.h>

class RegionCommand : public Command {
public:
	enum Args : int {
		info = 1,
		addmember = 2,
		removemember = 3,
		help = 4
	} arg;
	string regionName;
	string recipientName;
	bool regionNameIsset;
	bool recipientNameIsset;
	void execute(CommandOrigin const& ori, CommandOutput& output) const;
	static void setup(CommandRegistry* registry);
};