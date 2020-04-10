#include "fake_lag.hpp"
#include "../../../dependencies/common_includes.hpp"




namespace desync
{
	float yaw_offset;

	bool flip_yaw = false;
	bool flip_packet = false;
	anim_state* anim;
	

	bool is_enabled(c_usercmd* cmd)
	{
		player_t* local = (player_t*)interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player());
		if (!local || !local->is_alive())
			return false;

		if (!config_system.item.desync || (cmd->buttons & in_use))
			return false;

		auto* channel_info = interfaces::engine->get_net_channel_info();
		if (channel_info && (channel_info->get_average_loss(1) > 0.f || channel_info->get_average_loss(0) > 0.f))
			return false;

		if (local->has_gun_game_immunity() || local->flags() & fl_frozen)
			return false;

		if (local->move_type() == movetype_ladder|| local->move_type() == movetype_noclip)
			return false;

		return true;
	}

	bool is_firing(c_usercmd* cmd)
	{
		player_t* local = (player_t*)interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player());
		auto weapon = local->active_weapon();
		if (!weapon)
			return false;

		const auto weapon_type = weapon->GetCSWpnData()->m_iWeaponType;
		if (weapon_type == WEAPONTYPE_GRENADE)
		{
			return true;

			//if (!weapon->m_bPinPulled() && (cmd->buttons & IN_ATTACK || cmd->buttons & IN_ATTACK2))
				//return false;
		}
		else if (weapon_type == WEAPONTYPE_KNIFE)
		{
			if (cmd->buttons & in_attack || cmd->buttons & in_attack2)
				return true;
		}
		
		else if (cmd->buttons & in_attack && weapon_type != WEAPONTYPE_C4)
			return true;

		return false;
	}

	

	float get_max_desync_delta()
	{
		const auto speed_factor = std::max<float>(0.f, std::min<float>(1, anim->speed));
		const auto speed_fraction = std::max<float>(0.f, std::min<float>(anim->m_flSpeedFraction, 1.f));

		const auto unk1 = (anim->landing_duck_amount * -0.30000001 - 0.19999999) * speed_fraction;
		float unk2 = unk1 + 1.f;

		if (anim->duck_amount > 0)
			unk2 += anim->duck_amount * speed_factor * (0.5f - unk2);

		return anim->m_flMaxBodyYawDegrees * unk2;
	}

	void handle(c_usercmd* cmd, bool& send_packet)
	{
		player_t* local = (player_t*)interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player());

		if (!send_packet)
			return;

		if (!is_enabled(cmd))
		{
			yaw_offset = 0.f;

			return;
		}

		anim = local->get_anim_state();

		assert(anim);

		yaw_offset = get_max_desync_delta();

		const auto old_angles = cmd->viewangles;

		flip_packet = !flip_packet;
		send_packet = flip_packet;
		if (!flip_packet)
		{
			if (config_system.item.yaw_flip)
				flip_yaw = !flip_yaw;
		}

#ifdef _DEBUG
		//console::print("=== desync ===");
		//console::print("anim state: %p", anim_state);
		//console::print("max desync delta: %.2f", yaw_offset);
#endif

		static float last_lby = 0.f;
		static float last_update = 0.f;

		const auto current_lby = local->lower_body_yaw();
		const float current_time = local->get_tick_base() * interfaces::globals->interval_per_tick;

		const float delta = ceilf((current_time - last_update) * 100) / 100;
		const auto next_delta = ceilf((delta + interfaces::globals->interval_per_tick) * 100) / 100;

		if (local->velocity().Length2D() <= 0.f)
		{
			if (current_lby != 180.f && last_lby != current_lby)
			{
				//console::print("lby updated after %.4f", delta);

				last_lby = current_lby;
				last_update = current_time - interfaces::globals->interval_per_tick;
			} 
			else if (next_delta >= 1.1f)
			{
				//console::print("curr: %.4f; next: %.4f", delta, next_delta);

				send_packet = flip_packet = true;

				last_update = current_time;
			} 
		}
		else
		{
			last_lby = current_lby;
			last_update = current_time;
		}
		auto old_viewangles = cmd->viewangles;
		auto old_forwardmove = cmd->forwardmove;
		auto old_sidemove = cmd->sidemove;
		
		const auto low_fps = interfaces::globals->interval_per_tick * 0.9f < interfaces::globals->absolute_frametime;
		if (low_fps || is_firing(cmd))
			send_packet = flip_packet = true;
		//if (LBYUpdate() /*m_bBreakLowerBody*/)
		//{
		//	send_packet = false;

		//	cmd->viewangles.y += 56.0f;

		//	send_packet = true;
		//	return;
		//}*/

		if (send_packet)
			anim->goal_feet_yaw += flip_yaw ? yaw_offset : -yaw_offset;
		else
		{
			cmd->buttons &= ~(in_forward | in_back | in_moveright | in_moveleft);
			
			cmd->viewangles.y += 180.f;
			cmd->viewangles.normalize();
			math.correct_movement(old_angles, cmd, old_forwardmove, old_sidemove);
			
		}
		if (fakelag->break_lby(cmd))
		{
			send_packet = false;
			
			cmd->viewangles.y -= 125.f;
			return;

		}
	}
}