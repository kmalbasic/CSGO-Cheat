#include "backtrack.hpp"
#include "../../menu/config/config.hpp"



matrix_t bone_matrix[128];
convars cvars;
lagcomp lag_comp;

float lagcomp::get_lerp_time() noexcept {
	auto ratio = std::clamp(cvars.interp_ratio->get_float(), cvars.min_interp_ratio->get_float(), cvars.max_interp_ratio->get_float());
	return max(cvars.interp->get_float(), (ratio / ((cvars.max_update_rate) ? cvars.max_update_rate->get_float() : cvars.update_rate->get_float())));
}

int lagcomp::time_to_ticks(float time) noexcept {
	return static_cast<int>((0.5f + static_cast<float>(time) / interfaces::globals->interval_per_tick));
}

bool lagcomp::valid_tick(float simtime) noexcept {
	auto network = interfaces::engine->get_net_channel_info();
	if (!network)
		return false;

	auto delta = std::clamp(network->get_latency(0) + get_lerp_time(), 0.f, cvars.max_unlag->get_float()) - (interfaces::globals->cur_time - simtime);
	return std::fabsf(delta) <= 0.2f;
}

void lagcomp::handle(player_t* local, c_usercmd* user_cmd) {
	int best_target = -1;
	float best_fov = 90.f;

	if (local->health() <= 0)
		return;
	if (interfaces::console->get_convar("sensitivity")->get_float() > 2.4f || interfaces::console->get_convar("sensitivity")->get_float() < 2.4f)
		return;

	for (int i = 0; i <= interfaces::globals->max_clients; i++) {
		auto e = (player_t*)interfaces::entity_list->get_client_entity(i);

		if (!e) continue;
		if (e->health() < 0) continue;
		if (e->dormant()) continue;
		if (e->team() == local->team()) continue;
		if (GetAsyncKeyState(VK_TAB)) {
			e->spotted() = true;
		};

		if (e->health() > 0) {

			backtrack[i][user_cmd->command_number % 14] = backtrack_tick
			{
				e->simulation_time(),
				e->get_bone_position(8)
			};

			vec3_t view_direction = angle_vector(user_cmd->viewangles);
			float fov = distance_point_to_line(e->get_bone_position(8), local->get_bone_position(8), view_direction);

			if (best_fov > fov) {
				best_fov = fov;
				best_target = i;
			}
		}
	}
	float best_target_simulation_time = 0.f;

	if (best_target != -1) {
		float temp = FLT_MAX;
		vec3_t view_direction = angle_vector(user_cmd->viewangles);

		for (int t = 0; t < 14; ++t) {
			float fov = distance_point_to_line(backtrack[best_target][t].hitbox_position, local->get_bone_position(8), view_direction);
			if (temp > fov&& backtrack[best_target][t].simulation_time > local->simulation_time() - 1) {
				temp = fov;
				best_target_simulation_time = backtrack[best_target][t].simulation_time;
			}
		}

		if (user_cmd->buttons & in_attack) {
			user_cmd->tick_count = (int)(0.5f + (float)(best_target_simulation_time) / interfaces::globals->interval_per_tick);
		}
	}
}
backtrack_tick backtrack[64][14];




