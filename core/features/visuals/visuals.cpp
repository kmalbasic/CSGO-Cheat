#include "visuals.hpp"
#include "../../../dependencies/common_includes.hpp"
#include "../../menu/menu.hpp"
#include "../backtrack/backtrack.hpp"
#define TIME_TO_TICKS(dt) ((int)( 0.5f + (float)(dt) / interfaces::globals->interval_per_tick))

c_visuals visuals;

void c_visuals::run() noexcept {
	if (!config_system.item.visuals_enabled || (config_system.item.anti_screenshot && interfaces::engine->is_taking_screenshot()))
		return;

	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));
	if (!local_player)
		return;

	//player drawing loop
	for (int i = 1; i <= interfaces::globals->max_clients; i++) {
		auto entity = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(i));

		if (!entity || entity == local_player || entity->health() <= 0)
			continue;

		if (config_system.item.radar)
			entity->spotted() = true;


		if (config_system.item.visuals_visible_only)
			if (!local_player->can_see_player_pos(entity, entity->get_eye_pos()) && local_player->is_alive())
				continue;

		if (config_system.item.visuals_on_key && !GetAsyncKeyState(config_system.item.visuals_key))
			continue;

		const int fade = (int)((6.66666666667f * interfaces::globals->frame_time) * 255);

		auto new_alpha = alpha[i];
		new_alpha += entity->dormant() ? -fade : fade;

		if (new_alpha > (entity->has_gun_game_immunity() ? 130 : 210))
			new_alpha = (entity->has_gun_game_immunity() ? 130 : 210);
		if (new_alpha < config_system.item.player_dormant ? 50 : 0)
			new_alpha = config_system.item.player_dormant ? 50 : 0;

		alpha[i] = new_alpha;

		player_rendering(entity);

		last_dormant[i] = entity->dormant();
	}

}





	void c_visuals::player_rendering(player_t * entity) noexcept {
		if ((entity->dormant() && alpha[entity->index()] == 0) && !config_system.item.player_dormant)
			return;

		player_info_t info;
		interfaces::engine->get_player_info(entity->index(), &info);

		box bbox;
		if (!get_playerbox(entity, bbox))
			return;

		if (config_system.item.player_box1) {
			auto red = config_system.item.clr_box[0] * 255;
			auto green = config_system.item.clr_box[1] * 255;
			auto blue = config_system.item.clr_box[2] * 255;

			render::draw_outline(bbox.x - 1, bbox.y - 1, bbox.w + 2, bbox.h + 2, color(0, 0, 0, 255 + alpha[entity->index()]));
			render::draw_rect(bbox.x, bbox.y, bbox.w, bbox.h, color(red, green, blue, alpha[entity->index()]));
			render::draw_outline(bbox.x + 1, bbox.y + 1, bbox.w - 2, bbox.h - 2, color(0, 0, 0, 255 + alpha[entity->index()]));
		}

		if (config_system.item.player_name) {
			auto red = config_system.item.clr_name[0] * 255;
			auto green = config_system.item.clr_name[1] * 255;
			auto blue = config_system.item.clr_name[2] * 255;

			std::string print(info.fakeplayer ? std::string("bot ").append(info.name).c_str() : info.name);
			std::transform(print.begin(), print.end(), print.begin(), ::tolower);

			render::draw_text_string(bbox.x + (bbox.w / 2), bbox.y - 13, render::fonts::name_font, print, true, color(red, green, blue, alpha[entity->index()]));
		}
		{
			std::vector<std::pair<std::string, color>> flags;
			//std::string posi = (entity->get_callout());
			//std::transform(posi.begin(), posi.end(), posi.begin(), ::tolower);

			if (config_system.item.player_flags_armor && entity->has_helmet() && entity->armor() > 0)
				flags.push_back(std::pair<std::string, color>("hk", color(255, 255, 255, alpha[entity->index()])));
			else if (config_system.item.player_flags_armor && !entity->has_helmet() && entity->armor() > 0)
				flags.push_back(std::pair<std::string, color>("k", color(255, 255, 255, alpha[entity->index()])));

			if (config_system.item.player_flags_money && entity->money())
				flags.push_back(std::pair<std::string, color>(std::string("$").append(std::to_string(entity->money())), color(120, 180, 10, alpha[entity->index()])));

			if (config_system.item.player_flags_scoped && entity->is_scoped())
				flags.push_back(std::pair<std::string, color>(std::string("zoom"), color(80, 160, 200, alpha[entity->index()])));

			if (config_system.item.player_flags_c4 && entity->has_c4())
				flags.push_back(std::pair<std::string, color>(std::string("bomb"), color(255, 127, 36, alpha[entity->index()])));


			if (config_system.item.player_flags_flashed && entity->is_flashed())
				flags.push_back(std::pair<std::string, color>(std::string("flashed"), color(255, 255, 0, alpha[entity->index()])));




		
		}
	}


	





void c_visuals::chams() noexcept {
	if (!config_system.item.visuals_enabled || (!config_system.item.vis_chams_vis && !config_system.item.vis_chams_invis))
		return;

	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));
	if (!local_player)
		return;

	static i_material * mat = nullptr;
	auto textured = interfaces::material_system->find_material("aristois_material", TEXTURE_GROUP_MODEL, true, nullptr);
	auto metalic = interfaces::material_system->find_material("aristois_reflective", TEXTURE_GROUP_MODEL, true, nullptr);
	auto flat = interfaces::material_system->find_material("debug/debugdrawflat", TEXTURE_GROUP_MODEL, true, nullptr);
	auto dogtag = interfaces::material_system->find_material("models/inventory_items/dogtags/dogtags_outline", TEXTURE_GROUP_MODEL, true, nullptr);
	textured->increment_reference_count();
	metalic->increment_reference_count();
	flat->increment_reference_count();
	dogtag->increment_reference_count();

	switch (config_system.item.vis_chams_type) {
	case 0:
		mat = textured;
		break;
	case 1:
		mat = flat;
		break;
	case 2:
		mat = metalic;
		break;
	case 3:
		mat = dogtag;
		break;
	}

	for (int i = 1; i <= interfaces::globals->max_clients; i++) {
		auto entity = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(i));
		if (!entity || !entity->is_alive() || entity->dormant())
			continue;

		bool is_teammate = entity->team() == local_player->team();
		bool is_enemy = entity->team() != local_player->team();



		//needed for healthbased chams	
		float life_color[3] = { 0.f };
		float health = entity->health();
		int red = 255 - (health * 2.55);
		int green = health * 2.55;
		life_color[0] = red / 255.f;
		life_color[1] = green / 255.f;
		life_color[2] = 0.f / 255.f;

		if (is_enemy) {

			if (config_system.item.vis_chams_invis) {
				if (utilities::is_behind_smoke(local_player->get_eye_pos(), entity->get_hitbox_position(entity, hitbox_head)) && config_system.item.vis_chams_smoke_check)
					continue;
				interfaces::render_view->modulate_color(config_system.item.clr_chams_invis);
				interfaces::render_view->set_blend(config_system.item.clr_chams_invis[3]);
				mat->set_material_var_flag(material_var_ignorez, true);

				interfaces::model_render->override_material(mat);
				entity->draw_model(1, 255);
			}
			if (config_system.item.vis_chams_vis) {
				if (utilities::is_behind_smoke(local_player->get_eye_pos(), entity->get_hitbox_position(entity, hitbox_head)) && config_system.item.vis_chams_smoke_check)
					continue;

				interfaces::render_view->modulate_color(config_system.item.clr_chams_vis);
				interfaces::render_view->set_blend(config_system.item.clr_chams_vis[3]);
				mat->set_material_var_flag(material_var_ignorez, false);
				interfaces::model_render->override_material(mat);
				entity->draw_model(1, 255);
			}
		}

		if (is_teammate) {
			if (config_system.item.vis_chams_invis_teammate) {
				interfaces::render_view->modulate_color(config_system.item.clr_chams_invis_teammate);
				interfaces::render_view->set_blend(config_system.item.clr_chams_invis_teammate[3]);
				mat->set_material_var_flag(material_var_ignorez, true);

				interfaces::model_render->override_material(mat);
				entity->draw_model(1, 255);
			}
			if (config_system.item.vis_chams_vis_teammate) {
				interfaces::render_view->modulate_color(config_system.item.clr_chams_vis_teammate);
				interfaces::render_view->set_blend(config_system.item.clr_chams_vis_teammate[3]);
				mat->set_material_var_flag(material_var_ignorez, false);

				interfaces::model_render->override_material(mat);
				entity->draw_model(1, 255);
			}
		}
		interfaces::model_render->override_material(nullptr);
	}

	
}

void c_visuals::chams_misc(const model_render_info_t & info) noexcept {

	auto model_name = interfaces::model_info->get_model_name((model_t*)info.model);
	if (!model_name)
		return;

	static i_material* mat = nullptr;
	auto textured = interfaces::material_system->find_material("aristois_material", TEXTURE_GROUP_MODEL, true, nullptr);
	auto metalic = interfaces::material_system->find_material("aristois_reflective", TEXTURE_GROUP_MODEL, true, nullptr);
	auto flat = interfaces::material_system->find_material("debug/debugdrawflat", TEXTURE_GROUP_MODEL, true, nullptr);
	auto dogtag = interfaces::material_system->find_material("models/inventory_items/dogtags/dogtags_outline", TEXTURE_GROUP_MODEL, true, nullptr);
	textured->increment_reference_count();
	metalic->increment_reference_count();
	flat->increment_reference_count();
	dogtag->increment_reference_count();

	switch (config_system.item.vis_chams_type1) {
	case 0:
		mat = textured;
		break;
	case 1:
		mat = flat;
		break;
	case 2:
		mat = metalic;
		break;
	case 3:
		mat = dogtag;
		break;
	}

	if (config_system.item.sleeve_chams && strstr(model_name, "sleeve")) {

		interfaces::render_view->set_blend(config_system.item.clr_sleeve_chams[3]);
		interfaces::render_view->modulate_color(config_system.item.clr_sleeve_chams);
		mat->set_material_var_flag(material_var_ignorez, false);
		interfaces::model_render->override_material(mat);
	}
	if (config_system.item.hand_chams && strstr(model_name, "arms")
		&& !strstr(model_name, "sleeve")) {

		interfaces::render_view->set_blend(config_system.item.clr_hand_chams[3]);
		interfaces::render_view->modulate_color(config_system.item.clr_hand_chams);
		mat->set_material_var_flag(material_var_ignorez, false);
		interfaces::model_render->override_material(mat);
	}
	if (config_system.item.weapon_chams && strstr(model_name, "models/weapons/v_")
		&& !strstr(model_name, "arms") && !strstr(model_name, "sleeve")) {

		interfaces::render_view->set_blend(config_system.item.clr_weapon_chams[3]);
		interfaces::render_view->modulate_color(config_system.item.clr_weapon_chams);
		mat->set_material_var_flag(material_var_ignorez, false);
		interfaces::model_render->override_material(mat);
	}
	if (config_system.item.weapon_chams && strstr(model_name, "models/weapons/w_")
		&& !strstr(model_name, "arms") && !strstr(model_name, "sleeve")) {

		interfaces::render_view->set_blend(config_system.item.clr_weapon_chams[3]);
		interfaces::render_view->modulate_color(config_system.item.clr_weapon_chams);
		mat->set_material_var_flag(material_var_ignorez, false);
		interfaces::model_render->override_material(mat);
	}
	/*if (config_system.item.vis_chams_smoke_check && strstr(model_name, "models/player")) {
		interfaces::render_view->set_blend(config_system.item.clr_dropped_weapon_chams[3]);
		interfaces::render_view->modulate_color(config_system.item.clr_dropped_weapon_chams);
		mat->set_material_var_flag(MATERIAL_VAR_IGNOREZ, false);
		interfaces::model_render->override_material(mat);
	}*/
}



