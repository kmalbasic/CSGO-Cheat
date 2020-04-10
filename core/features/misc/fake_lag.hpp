#pragma once

#include "../../../dependencies/common_includes.hpp"


#pragma once
#pragma once

namespace SDK
{
	class CUserCmd;
}

namespace desync
{
	extern bool flip_yaw;
	void handle(c_usercmd* cmd, bool& send_packet);
}

class CFakeLag
{
public:
	int lag_comp_break();
	void do_fakelag(c_usercmd* user_cmd, bool& send_packet);
	bool break_lby(c_usercmd* user_cmd);
	
};

extern CFakeLag* fakelag;