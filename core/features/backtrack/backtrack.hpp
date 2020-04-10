#pragma once
#include "../../../dependencies/common_includes.hpp"
struct backtrack_tick {
	float simulation_time;
	vec3_t hitbox_position;

};
struct convars {
	convar* update_rate;
	convar* max_update_rate;
	convar* interp;
	convar* interp_ratio;
	convar* min_interp_ratio;
	convar* max_interp_ratio;
	convar* max_unlag;
};

class lagcomp : public singleton<lagcomp> {

public:
	void handle(player_t* local, c_usercmd* user_cmd);
	float get_lerp_time() noexcept;
	int time_to_ticks(float time) noexcept;
	bool valid_tick(float simtime) noexcept;
	int delay = 0;

	vec3_t angle_vector(vec3_t to_convert) {
		auto y_sin = sin(to_convert.y / 180.f * static_cast<float>(M_PI));
		auto y_cos = cos(to_convert.y / 180.f * static_cast<float>(M_PI));

		auto x_sin = sin(to_convert.x / 180.f * static_cast<float>(M_PI));
		auto x_cos = cos(to_convert.x / 180.f * static_cast<float>(M_PI));

		return vec3_t(x_cos * y_cos, x_cos * y_sin, -x_sin);
	}
	float distance_point_to_line(vec3_t point, vec3_t origin, vec3_t direction) {
		auto delta = point - origin;

		auto temp = delta.dot(direction) / (direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
		if (temp < 0.000001f)
			return FLT_MAX;

		auto temp_meme = origin + (direction * temp);
		return (point - temp_meme).length();
	}
};


extern backtrack_tick backtrack[64][14];


