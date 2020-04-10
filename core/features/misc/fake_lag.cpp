#include "fake_lag.hpp"
#include "globals.hpp"
anim_state* anim;
int CFakeLag::lag_comp_break()
{
	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));

	if (!local_player || local_player->health() <= 0)
		return 1;

	auto velocity = local_player->velocity();
	velocity.z = 0.f;
	auto speed = velocity.length();
	auto distance_per_tick = speed * interfaces::globals->interval_per_tick;
	int choked_ticks = std::ceilf(64.f / distance_per_tick);
	return std::min<int>(choked_ticks, 14.f);
}
bool CFakeLag::break_lby(c_usercmd* cmd)
{
	player_t* local = (player_t*)interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player());

	if (!local)
		return false;

	static float next_lby_update_time = 0;
	float curtime = interfaces::globals->cur_time;
	auto animstate = local->get_anim_state();

	if (!animstate)
		return false;

	if (!(local->flags() & fl_onground))
		return false;

	if (local->velocity().length() > 0.1)
		next_lby_update_time = curtime + 0.22f;

	if (next_lby_update_time < curtime)
	{
		next_lby_update_time = curtime + 1.1f;
		return true;
	}

	return false;
}



	

void CFakeLag::do_fakelag(c_usercmd* user_cmd, bool& send_packet)
{
	send_packet = true;
	int choke_amount;

	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));
	if (!local_player || local_player->health() <= 0)
		return;

	auto net_channel = interfaces::engine->get_net_channel_info();
	if (!net_channel)
		return;


	if (config_system.item.fake_lag_enable)
	{
		choke_amount = 1;
		if (!(local_player->flags() & fl_onground))
		{
			switch (config_system.item.fake_lag_type % 2)
			{
			case 0: choke_amount = config_system.item.fake_lag_amount; break;
			case 1: choke_amount = lag_comp_break(); break;
			}
		}
		else if (local_player->velocity().Length2D() > 0.1)
		{
			switch (config_system.item.fake_lag_type % 2)
			{
			case 0: choke_amount = config_system.item.fake_lag_amount; break;
			case 1: choke_amount = lag_comp_break(); break;
			}
		}
		else if (local_player->velocity().Length2D() < 0.1)
		{
			switch (config_system.item.fake_lag_type % 2)
			{
			case 0: choke_amount = config_system.item.fake_lag_amount; break;
			case 1: choke_amount = lag_comp_break(); break;
			}
		}
	}
	else
		choke_amount = 1;

	if (net_channel->m_nChokedPackets >= min(14, choke_amount))
		send_packet = true;
	else
		send_packet = false;
}

CFakeLag* fakelag = new CFakeLag();