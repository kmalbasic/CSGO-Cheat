#include "../features.hpp"
#include "../misc/globals.hpp"

int hitmarker_time = 0;
using namespace std;
void misc::movement::bunny_hop(c_usercmd* cmd) {
	static bool last_jumped = false, should_fake = false;

	if (!last_jumped && should_fake) {
		should_fake = false;
		cmd->buttons |= in_jump;
	}
	else if (cmd->buttons & in_jump) {
		if (csgo::local_player->flags() & fl_onground) {
			last_jumped = true;
			should_fake = true;
		}
		else {
			cmd->buttons &= ~in_jump;
			last_jumped = false;
		}
	}
	else {
		last_jumped = false;
		should_fake = false;
	};
	
};
void misc::movement::offscreen_esp() noexcept
{
	if (!config_system.item.offscreen)
		return;

	for (int i = 1; i <= interfaces::globals->max_clients; i++) {
		auto entity = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(i));
	
		if (entity != nullptr && entity->is_player())
		{

			vec3_t min, max;
			vec3_t pos, pos3D, top, top3D;
			pos3D = entity->abs_origin() - vec3_t(0, 0, 10);
			top3D = pos3D + vec3_t(0, 0, max.z + 10);

			int screen_x, screen_y;
			interfaces::engine->get_screen_size(screen_x, screen_y);

			if (math.world_to_screen(pos3D, pos)) {



				if (entity->team() == csgo::local_player->team())
				{
					render::draw_rect(pos.x, top.y, 10, 10, color::green(255));
				}
				else
				{
					render::draw_rect(pos.x, top.y, 10, 10, color::red(255));
				}

			}
		}

	}
}
void misc::movement::third_person() noexcept
{
	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));
	static bool enabledtp = false, check = false;


	auto GetCorrectDistance = [&local_player](float ideal_distance) -> float
	{
		vec3_t inverse_angles;
		interfaces::engine->get_view_angles_alternative(inverse_angles);

		inverse_angles.x *= -1.f, inverse_angles.y += 180.f;

		vec3_t direction;
		math.angle_vectors(inverse_angles, direction);

		trace_world_only filter;
		trace_t trace;
		ray_t ray;

		ray.initialize(local_player->origin() + local_player->view_offset(), (local_player->origin() + local_player->view_offset()) + (direction * (ideal_distance + 5.f)));
		interfaces::trace_ray->trace_ray(ray, MASK_ALL, &filter, &trace);

		return ideal_distance * trace.flFraction;
	};

	if (config_system.item.third_person && in_thirdperson)
	{
		if (local_player->health() <= 0)
			local_player->observer_mode() = 5;

		if (!interfaces::input->m_fCameraInThirdPerson)
		{
			interfaces::input->m_fCameraInThirdPerson = true;
			interfaces::input->m_vecCameraOffset = vec3_t(g::real_angles.x, g::real_angles.y, GetCorrectDistance(100));

			vec3_t camForward;
			math.angle_vectors_alternative(vec3_t(interfaces::input->m_vecCameraOffset.x, interfaces::input->m_vecCameraOffset.y, 0), &camForward);
		}
	}
	else
	{
		interfaces::input->m_fCameraInThirdPerson = false;
		interfaces::input->m_vecCameraOffset = vec3_t(g::real_angles.x, g::real_angles.y, 0);
	}
}

void misc::movement::rank_reveal() noexcept {
	if (!config_system.item.rank_reveal)
		return;
	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));
	if (!local_player)
		return;

	static auto weapon_debug_spread_show = interfaces::console->get_convar("weapon_debug_spread_show");

	if (local_player && local_player->health() > 0) {
		weapon_debug_spread_show->set_value(2);
	}
	if (GetAsyncKeyState(VK_TAB))
		interfaces::client->dispatch_user_message(cs_um_serverrankrevealall, 0, 0, nullptr);
}
void misc::movement::relay_cluster() {
	if (!config_system.item.relay_cluster)
		return;
	if (interfaces::engine->is_connected() && interfaces::engine->is_in_game())
		return;

     const char* DatacentersNames[] = { "Don't force", "Australia", "Austria", "Brazil", "Chile", "Dubai", "France", "Germany", "Hong Kong", "India (Chennai)", "India (Mumbai)", "Japan", "Luxembourg",
			"Netherlands", "Peru", "Philipines", "Poland", "Singapore", "South Africa", "Spain", "Sweden", "UK", "USA (Atlanta)", "USA (Seattle)", "USA (Chicago)", "USA (Los Angeles)", "USA (Moses Lake)",
			"USA (Oklahoma)", "USA (Seattle)", "USA (Washington DC)" };
	
	static std::string* ForceRelayClusterValue = *(std::string**)(utilities::pattern_scan(GetModuleHandleA("steamnetworkingsockets.dll"), "B8 ? ? ? ? B9 ? ? ? ? 0F 43") + 1);

	
	switch (config_system.item.relay_cluster_select) {
	case 0:
		*ForceRelayClusterValue = DatacentersNames[7]; //germany
		break;
	case 1:
		*ForceRelayClusterValue = DatacentersNames[5]; //dubai
		break;
	case 2:
		*ForceRelayClusterValue = DatacentersNames[22]; //atlanta
		break;
	case 3:
		*ForceRelayClusterValue = DatacentersNames[14]; //peru
		break;
	case 4:
		*ForceRelayClusterValue = DatacentersNames[18]; //africa
		break;
	case 5:
		*ForceRelayClusterValue = DatacentersNames[10]; //india
		break;
	case 6:
		*ForceRelayClusterValue = DatacentersNames[15]; //phillipines
		break;
	case 7:
		*ForceRelayClusterValue = DatacentersNames[17]; // singapore
		break;
	};
}

void misc::movement::viewmodel() noexcept {

	if (!config_system.item.viewmodeltoggle)
	{
		interfaces::console->get_convar("viewmodel_fov")->set_value(68);
		interfaces::console->get_convar("viewmodel_offset_x")->set_value(2);
		interfaces::console->get_convar("viewmodel_offset_y")->set_value(2);
		interfaces::console->get_convar("viewmodel_offset_z")->set_value(-1);
		return;
	}

	interfaces::console->get_convar("viewmodel_fov")->callbacks.set_size(false);
	interfaces::console->get_convar("viewmodel_offset_x")->callbacks.set_size(false);
	interfaces::console->get_convar("viewmodel_offset_y")->callbacks.set_size(false);
	interfaces::console->get_convar("viewmodel_offset_z")->callbacks.set_size(false);
	interfaces::console->get_convar("viewmodel_fov")->set_value(config_system.item.viewmodel_fov);
	interfaces::console->get_convar("viewmodel_offset_x")->set_value(config_system.item.viewmodel_x);
	interfaces::console->get_convar("viewmodel_offset_y")->set_value(config_system.item.viewmodel_y);
	interfaces::console->get_convar("viewmodel_offset_z")->set_value(config_system.item.viewmodel_z);

}



void misc::movement::ragdoll() {

	if (!config_system.item.ragdoll) {
		interfaces::console->get_convar("phys_pushscale")->set_value(100);
		return;
	}
	if (config_system.item.ragdoll)
	
    {
	   interfaces::console->get_convar("phys_pushscale")->set_value(1000);
	}
}



void misc::movement:: spectators()  {
	if (config_system.item.spectators_list)
		return;

	if (!interfaces::engine->is_connected() && !interfaces::engine->is_in_game())
		return;

	int spectator_index = 0;
	int width, height;
	interfaces::engine->get_screen_size(width, height);

	render::draw_text_string(width - 80, height / 2 - 10, render::fonts::watermark_font, "spectators:", true, color(255, 255, 255));
	for (int i = 0; i < interfaces::entity_list->get_highest_index(); i++) {
		auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));
		auto entity = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(i));
		player_info_t info;

		if (entity && entity != local_player) {
			interfaces::engine->get_player_info(i, &info);
			if (!entity->is_alive() && !entity->dormant()) {
				auto target = entity->observer_target();
				if (target) {
					auto spectator_target = interfaces::entity_list->get_client_entity_handle(target);
					if (spectator_target == local_player) {

						std::string player_name = info.name;
						std::transform(player_name.begin(), player_name.end(), player_name.begin(), ::tolower);
						player_info_t spectator_info;
						interfaces::engine->get_player_info(i, &spectator_info);
						render::draw_text_string(width - 80, height / 2 + (10 * spectator_index), render::fonts::watermark_font, player_name.c_str(), true, color(255, 255, 255));
						spectator_index++;
					}
				}
			}
		}
	}
}
int count1 = 0;
void misc::movement::clantag_spammer() {
	
	if (!config_system.item.clan_tag)
		return;
	static std::string tag = "$";
	static float last_time = 0;

	if (interfaces::globals->cur_time > last_time) {
		count1++;
		switch (count1) {
		case 1:
			tag = "g";
			break;
		case 2:
			tag = "ga";
			break;
		case 3:
			tag = "gam";
			break;
		case 4:
			tag = "game";
			break;
		case 5:
			tag = "games";
			break;
		case 6:
			tag = "gamese";
			break;
		case 7:
			tag = "gamesen";
			break;
		case 8:
			tag = "gamesens";
			break;
		case 9:
			tag = "gamesense";
			break;
		case 10:
			tag = "gamesens";
			break;
		case 11:
			tag = "gamesen";
			break;
		case 12:
			tag = "gamese";
			break;
		case 13:
			tag = "games";
			break;
		case 14:
			tag = "game";
			break;
		case 15:
			tag = "gam";
			break;
		case 16:
			tag = "ga";
			break;
		case 17:
			tag = "g";
			break;
		case 18:
			count1 = 1;

			break;
		}


		utilities::apply_clan_tag(tag.c_str());

		last_time = interfaces::globals->cur_time + 0.9f;
	}

	if (fabs(last_time - interfaces::globals->cur_time) > 1.f)
		last_time = interfaces::globals->cur_time;
}

void misc::movement::fduck(c_usercmd* cmd) {
	
		cmd->buttons |= in_bullrush;

}
void misc::movement::watermark() 
{
	if (!config_system.item.watermark)
		return;

	int width, height;
	interfaces::engine->get_screen_size(width, height);

	static int fps, old_tick_count;


	std::stringstream ss;
	std::stringstream ss2;
	std::string ss3;
	std::string ss4;
	std::string ss5;

	auto net_channel = interfaces::engine->get_net_channel_info();
	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));

	if (!local_player)
		return;
	auto red = config_system.item.clr_logs[0] * 255;
	auto green = config_system.item.clr_logs[1] * 255;
	auto blue = config_system.item.clr_logs[2] * 255;

	std::string incoming = local_player ? std::to_string((int)(net_channel->get_latency(FLOW_INCOMING) * 1000)) : "0";
	std::string outgoing = local_player ? std::to_string((int)(net_channel->get_latency(FLOW_OUTGOING) * 1000)) : "0";

	ss << "  nazisoft | fps: " + std::to_string(getfps());
	ss2 << " | incoming: " << incoming.c_str() << "ms" << " | outgoing: " << outgoing.c_str() << "ms";
	ss3 = ss.str().c_str();
	ss4 = ss2.str().c_str();
	ss5 = ss3 + ss4;
	//235, 52, 219, 255
	render::draw_filled_rect(width - 277, 2, 264, 27, color(0, 0, 0, 255));
	render::draw_filled_rect(width - 275, 4, 260, 23, color(red,green,blue, 255));
	render::draw_filled_rect(width - 275, 4, 260, 20, color(30, 30, 39, 255));
	//render::draw_outline(width - 275, 4, 260, 20, color(30, 30, 41, 255));
	render::draw_text_string(width - 270, 7, render::fonts::watermark_font,  ss5 , false, color(red,green,blue, 255));
}
void misc::movement::run() noexcept {
	if (!interfaces::engine->is_connected() && !interfaces::engine->is_in_game())
		return;

	if (config_system.item.hitmarker || config_system.item.hitmarker_sound) {
		misc::movement::draw();
	}
}

void misc::movement::additional_info_data(c_usercmd* cmd)
{
	
	static int fps1, old_tick_count1;
	float pitch1 = 0.0f;
	float yaw1 = 0.0f;
	int packets2 = 0;
	if ((interfaces::globals->tick_count - old_tick_count1) > 50) {
		fps1 = static_cast<int>(1.f / interfaces::globals->frame_time);
		old_tick_count1 = interfaces::globals->tick_count;
		if (config_system.item.desync)
		{
			packets2++;
			if (packets2 >= 2)
			{
				packets2 = 0;
				
			}
		};
		yaw1 = cmd->viewangles.y;
		pitch1 = cmd->viewangles.x;
		config_system.item.yaw = yaw1;
		config_system.item.pitch = pitch1;
		config_system.item.packets = packets2;

	}
}
int misc::movement::getfps()
{
	return static_cast<int>(1.f / interfaces::globals->frame_time);
}
void misc::movement::additional_info()
{
	if (!config_system.item.additional_info)
		return;

	int width, height;
	interfaces::engine->get_screen_size(width, height);

	static int fps, old_tick_count;
	
	if ((interfaces::globals->tick_count - old_tick_count) > 50) {
		fps = static_cast<int>(1.f / interfaces::globals->frame_time);
		old_tick_count = interfaces::globals->tick_count;
		

	}
	auto net_channel = interfaces::engine->get_net_channel_info();
	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));

	if (!local_player)
		return;

	auto red = config_system.item.clr_logs[0] * 255;
	auto green = config_system.item.clr_logs[1] * 255;
	auto blue = config_system.item.clr_logs[2] * 255;
	render::draw_filled_rect(width / 2 - 50, height - 60, 100, 65, color(30, 30, 39, 255));
	render::draw_filled_rect(width / 2 - 50, height - 3, 100, 3, color(red, green, blue, 255));
	if (config_system.item.desync)
	{
		render::draw_text_string(width / 2 - 47, height - 57, render::fonts::watermark_font, "  LBY: Breaking", 0, color(red, green, blue, 255));
		render::draw_text_string(width / 2 - 47, height - 47, render::fonts::watermark_font, " Packets choked: 1~2", 0, color(red, green, blue, 255));
		render::draw_text_string(width / 2 - 47, height - 37, render::fonts::watermark_font, "  Yaw:  " +  std::to_string(config_system.item.yaw), 0, color(red, green, blue, 255));
		render::draw_text_string(width / 2 - 47, height - 27, render::fonts::watermark_font, "  Pitch:  " + std::to_string(config_system.item.pitch), 0, color(red, green, blue, 255));
	}
	else
	{
		render::draw_text_string(width / 2 - 47, height - 57, render::fonts::watermark_font, "  LBY: Normal", 0, color(red, green, blue, 255));
		render::draw_text_string(width / 2 - 47, height - 47, render::fonts::watermark_font, " Packets choked: 0", 0, color(red, green, blue, 255));
		render::draw_text_string(width / 2 - 47, height - 37, render::fonts::watermark_font, "  Yaw:  " + std::to_string(config_system.item.yaw), 0, color(red, green, blue, 255));
		render::draw_text_string(width / 2 - 47, height - 27, render::fonts::watermark_font, "  Pitch:  " + std::to_string(config_system.item.pitch), 0, color(red, green, blue, 255));
	}
}

void misc::movement::force_crosshair() noexcept {
	if (!config_system.item.force_crosshair)
		return;
		int w, h;

		auto red = config_system.item.clr_crosshair[0] * 255;
		auto green = config_system.item.clr_crosshair[1] * 255;
		auto blue = config_system.item.clr_crosshair[2] * 255;

		interfaces::engine->get_screen_size(w, h);
		render::draw_line(w / 2- 3, h / 2, w / 2 + 3, h / 2, color(255, 255, 255));
		render::draw_line(w / 2, h / 2 - 3, w / 2, h / 2 + 3, color(255, 255, 255));
}

void misc::movement::event(i_game_event* event) noexcept {
	if (!event)
		return;

	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));
	if (!local_player)
		return;

	auto attacker = interfaces::entity_list->get_client_entity(interfaces::engine->get_player_for_user_id(event->get_int("attacker")));
	if (!attacker)
		return;

	if (attacker == local_player) {
		hitmarker_time = 255;

		switch (config_system.item.hitmarker_sound) {
		case 0:
			break;
		case 1:
			interfaces::surface->play_sound("buttons\\arena_switch_press_02.wav");
			break;
		case 2:
			interfaces::surface->play_sound("survival\\money_collect_01.wav");
			break;
		case 3:
			interfaces::surface->play_sound("survival\\turret_idle_01.wav");
			break;
		}
	}
}

void misc::movement::draw() noexcept {
	if (!config_system.item.hitmarker)
		return;

	int width, height;
	interfaces::engine->get_screen_size(width, height);
	int width_mid = width / 2;
	int height_mid = height / 2;

	if (hitmarker_time > 0) {
		float alpha = hitmarker_time;

		render::draw_line(width_mid + 6, height_mid + 6, width_mid + 3, height_mid + 3, color(255, 255, 255, alpha));
		render::draw_line(width_mid - 6, height_mid + 6, width_mid - 3, height_mid + 3, color(255, 255, 255, alpha));
		render::draw_line(width_mid + 6, height_mid - 6, width_mid + 3, height_mid - 3, color(255, 255, 255, alpha));
		render::draw_line(width_mid - 6, height_mid - 6, width_mid - 3, height_mid - 3, color(255, 255, 255, alpha));

		hitmarker_time -= 2;
	}
}



