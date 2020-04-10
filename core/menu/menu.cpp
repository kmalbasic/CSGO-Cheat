#include "menu.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include <D3dx9tex.h>
#pragma comment(lib, "D3dx9")
#include "../features/skinchanger/parser.hpp"

c_menu menu;

#define UNLEN 256
IDirect3DStateBlock9 *state_block;
bool reverse = false;
int offset = 0;
bool show_popup = false;
bool save_config = false;
bool load_config = false;


namespace ImGui {
	long get_mils() {
		auto duration = std::chrono::system_clock::now().time_since_epoch();
		return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
	}

	void begin_popup(const char* text, int on_screen_mils, bool* done) {
		if (!done)
			show_popup = true;

		ImGuiIO &io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		int width = io.DisplaySize.x;
		static long old_time = -1;
		ImGui::SetNextWindowPos(ImVec2(width - offset, 100));
		style.WindowMinSize = ImVec2(100.f, 20.f);
		ImGui::Begin("##PopUpWindow", &show_popup, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
		ImVec2 p = ImGui::GetCursorScreenPos();

		ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(p.x - 15, p.y - 13), ImVec2(p.x + ImGui::GetWindowWidth(), p.y - 16), ImColor(167, 24, 71, 255), ImColor(58, 31, 87, 255), ImColor(58, 31, 87, 255), ImColor(167, 24, 71, 255));

		long current_time_ms = get_mils();

		ImVec2 text_size = ImGui::CalcTextSize(text);
		ImGui::SetCursorPosY(ImGui::GetWindowHeight() / 2 - text_size.y / 2);
		ImGui::Text(text);

		if (!reverse) {
			if (offset < ImGui::GetWindowWidth())
				offset += (ImGui::GetWindowWidth() + 5) * ((1000.0f / ImGui::GetIO().Framerate) / 100);

			if (offset >= ImGui::GetWindowWidth() && old_time == -1) {
				old_time = current_time_ms;
			}
		}

		if (current_time_ms - old_time >= on_screen_mils && old_time != -1)
			reverse = true;

		if (reverse) {
			if (offset > 0)
				offset -= (ImGui::GetWindowWidth() + 5) * ((1000.0f / ImGui::GetIO().Framerate) / 100);
			if (offset <= 0) {
				offset = 0;
				reverse = false;
				*done = true;
				old_time = -1;
				show_popup = false;
			}
		}

		ImGui::End();
	}
}
long c_menu::get_mils() {
	auto duration = std::chrono::system_clock::now().time_since_epoch();
	return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
}
void c_menu::begin_popup(const char* text, int on_screen_mils, bool* done) {
	if (!done)
		show_popup = true;

	ImGuiIO& io = ImGui::GetIO();
	ImGuiStyle& style = ImGui::GetStyle();
	int width = io.DisplaySize.x;
	static long old_time = -1;
	ImGui::SetNextWindowPos(ImVec2(width - offset, 100));
	style.WindowMinSize = ImVec2(100.f, 20.f);
	ImGui::Begin("##PopUpWindow", &show_popup, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
	ImVec2 p = ImGui::GetCursorScreenPos();

	ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(p.x - 15, p.y - 13), ImVec2(p.x + ImGui::GetWindowWidth(), p.y - 16), ImColor(167, 24, 71, 255), ImColor(58, 31, 87, 255), ImColor(58, 31, 87, 255), ImColor(167, 24, 71, 255));

	long current_time_ms = get_mils();

	ImVec2 text_size = ImGui::CalcTextSize(text);
	ImGui::SetCursorPosY(ImGui::GetWindowHeight() / 2 - text_size.y / 2);
	ImGui::Text(text);

	if (!reverse) {
		if (offset < ImGui::GetWindowWidth())
			offset += (ImGui::GetWindowWidth() + 5) * ((1000.0f / ImGui::GetIO().Framerate) / 100);

		if (offset >= ImGui::GetWindowWidth() && old_time == -1) {
			old_time = current_time_ms;
		}
	}

	if (current_time_ms - old_time >= on_screen_mils && old_time != -1)
		reverse = true;

	if (reverse) {
		if (offset > 0)
			offset -= (ImGui::GetWindowWidth() + 5) * ((1000.0f / ImGui::GetIO().Framerate) / 100);
		if (offset <= 0) {
			offset = 0;
			reverse = false;
			*done = true;
			old_time = -1;
			show_popup = false;
		}
	}

	ImGui::End();
}
float rainbowSpeed = 0.001;
static float staticHue = 0;

void c_menu::run(IDirect3DDevice9* pDevice) {
	static int page = 0;

	if (opened) {
		ImGui::GetStyle().Colors[ImGuiCol_CheckMark] = ImVec4(167 / 255.f, 24 / 255.f, 71 / 255.f, 1.f);
		ImGui::GetStyle().Colors[ImGuiCol_SliderGrab] = ImVec4(167 / 255.f, 24 / 255.f, 71 / 255.f, 1.f);
		ImGui::GetStyle().Colors[ImGuiCol_SliderGrabActive] = ImVec4(167 / 255.f, 24 / 71, 247 / 255.f, 1.f);

		ImGui::SetNextWindowSize(ImVec2(320, 320), ImGuiSetCond_FirstUseEver);
		ImGui::Begin("aristois", &opened, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_ShowBorders | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar); {
			//ImGui::SetWindowSize(ImVec2(320, 400), ImGuiSetCond_FirstUseEver);
			ImVec2 p = ImGui::GetCursorScreenPos();
			ImColor c = ImColor(32, 114, 247);
			ImVec2 panelPos = ImGui::GetWindowPos();
			staticHue -= rainbowSpeed;
			float width = 9999;

			ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(p.x, p.y + 30), ImVec2(p.x + ImGui::GetWindowWidth(), p.y - 3), ImColor(30, 30, 39));
			ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(p.x, p.y + 32), ImVec2(p.x + ImGui::GetWindowWidth(), p.y + +30), ImColor(167, 24, 71, 255), ImColor(58, 31, 87, 255), ImColor(58, 31, 87, 255), ImColor(167, 24, 71, 255));
			if (config_system.item.rainbow_theme) {
				if ((staticHue < -1.f)) staticHue += 1.f;
				for (int i = 0; i < width; i++)
				{
					float hue = staticHue + (1.f / (float)width) * i;
					if (hue < 0.f) hue += 1.f;
					ImColor cRainbow = ImColor::HSV(hue, 1.f, 1.f);
					ImColor color = ImVec4(1, 1, 1, 1);
					ImGui::GetStyle().Colors[ImGuiCol_CheckMark] = cRainbow;
					ImGui::GetStyle().Colors[ImGuiCol_SliderGrab] = cRainbow;
					ImGui::GetStyle().Colors[ImGuiCol_SliderGrabActive] = cRainbow;
					ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(p.x, p.y + 32), ImVec2(p.x + ImGui::GetWindowWidth(), p.y + +30), cRainbow);
					ImGui::ColorConvertHSVtoRGB(hue, 1.f, 1.f, color.Value.x, color.Value.y, color.Value.z);
					config_system.item.clr_logs[0] = color.Value.x;
					config_system.item.clr_logs[1] = color.Value.y;
					config_system.item.clr_logs[2] = color.Value.z;
					
				}
			}
			else {
				auto red = config_system.item.clr_theme[0];
				auto green = config_system.item.clr_theme[1];
				auto blue = config_system.item.clr_theme[2];
				ImGui::GetStyle().Colors[ImGuiCol_CheckMark] = ImVec4(red, green, blue, 1.f);
				ImGui::GetStyle().Colors[ImGuiCol_SliderGrab] = ImVec4(red, green, blue, 1.f);
				ImGui::GetStyle().Colors[ImGuiCol_SliderGrabActive] = ImVec4(red, green, blue, 1.f);
			};
			ImGui::PushFont(font_menu2);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 7); 
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 7); 
			ImGui::Text("=");
			ImGui::SameLine();
			ImGui::PopFont();
			ImGui::PushFont(font_icon);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 9);
			if (ImGui::ButtonT("A", ImVec2(40, 30), page, 0, false, false)) page = 0; ImGui::SameLine(0, 0);
			if (ImGui::ButtonT("D", ImVec2(40, 30), page, 1, false, false)) page = 1; ImGui::SameLine(0, 0);
			if (ImGui::ButtonT("B", ImVec2(40, 30), page, 2, false, false)) page = 2; ImGui::SameLine(0, 0);
			if (ImGui::ButtonT("G", ImVec2(40, 30), page, 3, false, false)) page = 3; ImGui::SameLine(0, 0);
			if (ImGui::ButtonT("  ", ImVec2(40, 30), page, 3, false, false)) page = 4; ImGui::SameLine(0, 0);
			if (ImGui::ButtonT("  ", ImVec2(40, 30), page, 3, false, false)) page = 4; ImGui::SameLine(0, 0);
			if (ImGui::ButtonT("  ", ImVec2(40, 30), page, 3, false, false)) page = 4; ImGui::SameLine(0, 0);
			if (ImGui::ButtonT("  ", ImVec2(40, 30), page, 3, false, false)) page = 4; ImGui::SameLine(0, 0);
			if (ImGui::ButtonT("  ", ImVec2(40, 30), page, 3, false, false)) page = 4; ImGui::SameLine(0, 0);
			if (ImGui::ButtonT("  ", ImVec2(40, 30), page, 3, false, false)) page = 4; ImGui::SameLine(0, 0);
			if (ImGui::ButtonT("  ", ImVec2(40, 30), page, 3, false, false)) page = 4; ImGui::SameLine(0, 0);
			if (ImGui::ButtonT("  ", ImVec2(40, 30), page, 3, false, false)) page = 4; ImGui::SameLine(0, 0);
			if (ImGui::ButtonT("  ", ImVec2(40, 30), page, 3, false, false)) page = 3; ImGui::SameLine(0, 0);
			
			if (ImGui::ButtonT(" I", ImVec2(40, 30), page, 3, false, false)) page = 4; ImGui::SameLine(0, 0);
			ImGui::PopFont();

			ImGui::PushFont(font_menu2);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 45);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 222); 

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));
			ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImVec4(30 / 255.f, 30 / 255.f, 39 / 255.f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0 / 255.f, 0 / 255.f, 0 / 255.f, 0.1f));


			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleVar();

			static int test = 0;
			switch (page) {
			case 1:
				ImGui::Columns(2, NULL, false);
				ImGui::Dummy(ImVec2(0, -2)); ImGui::SameLine();
				ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));

				//push window color for child
				ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImVec4(30 / 255.f, 30 / 255.f, 39 / 255.f, 1.0f));
				//push border color for child
				ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0 / 255.f, 0 / 255.f, 0 / 255.f, 0.1f));
				

				ImGui::PopStyleVar();
				ImGui::Dummy(ImVec2(0, -2)); ImGui::SameLine();
				ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));
				ImGui::BeginChild("settings", ImVec2(279, 543), true); {
					ImGui::Checkbox("visuals", &config_system.item.visuals_enabled);
					if (config_system.item.visuals_enabled)
					{
						ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
						ImGui::Checkbox("name", &config_system.item.player_name);
						ImGui::ColorEdit4("name color", config_system.item.clr_name, ImGuiColorEditFlags_NoInputs);
						ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
						ImGui::Checkbox("box", &config_system.item.player_box1);
						ImGui::ColorEdit4("box color", config_system.item.clr_box, ImGuiColorEditFlags_NoInputs);
						ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
						ImGui::Combo("chams type", &config_system.item.vis_chams_type, "textured\0flat\0metallic\0pulsating");
						ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
						ImGui::Checkbox("enemy", &config_system.item.vis_chams_vis);
						ImGui::ColorEdit4("enemy color", config_system.item.clr_chams_vis, ImGuiColorEditFlags_NoInputs);
						ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
						ImGui::Checkbox("enemy (behind wall)", &config_system.item.vis_chams_invis);
						ImGui::ColorEdit4("enemy (behind wall) color", config_system.item.clr_chams_invis, ImGuiColorEditFlags_NoInputs);
						ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
						ImGui::Checkbox("teammate", &config_system.item.vis_chams_vis_teammate);
						ImGui::ColorEdit4("teammate color", config_system.item.clr_chams_vis_teammate, ImGuiColorEditFlags_NoInputs);
						ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
						ImGui::Checkbox("teammate (behind wall)", &config_system.item.vis_chams_invis_teammate);
						ImGui::ColorEdit4("teammate (behind wall) color", config_system.item.clr_chams_invis_teammate, ImGuiColorEditFlags_NoInputs);
					}
					
					
					ImGui::Checkbox("backtrack", &config_system.item.backtrack);
					ImGui::PushStyleColor(ImGuiCol_Text, config_system.item.backtrack ? ImVec4(1.f, 1.f, 1.f, 1) : ImVec4(.6f, .6f, .6f, 1));
					ImGui::SliderFloat("records (ms)", &config_system.item.backtrack_ms, 1.0f, 200.0f, "%.2f");
					ImGui::Checkbox("viewmodel", &config_system.item.viewmodeltoggle);
					if (config_system.item.viewmodeltoggle)
					{
						ImGui::SliderInt("fov", &config_system.item.viewmodel_fov, 60, 180, "%.f");
						ImGui::SliderInt("x", &config_system.item.viewmodel_x, -5, 60, "%.f");
						ImGui::SliderInt("y", &config_system.item.viewmodel_y, -5, 60, "%.f");
						ImGui::SliderInt("z", &config_system.item.viewmodel_z, -5, 60, "%.f");
						ImGui::Checkbox("visual effects", &config_system.item.viewmodel_offset);
						if (config_system.item.viewmodel_offset)
						{
							ImGui::Combo("material", &config_system.item.vis_chams_type1, "textured\0flat\0metallic\0pulsating");
							ImGui::Checkbox(" weapon viewmodel", &config_system.item.weapon_chams);
							ImGui::ColorEdit4("weapon viewmodel color", config_system.item.clr_weapon_chams, ImGuiColorEditFlags_NoInputs);
							ImGui::Checkbox(" hand viewmodel", &config_system.item.hand_chams);
							ImGui::ColorEdit4("hand viewmodel color", config_system.item.clr_hand_chams, ImGuiColorEditFlags_NoInputs);
							ImGui::Checkbox(" sleeve viewmodel", &config_system.item.sleeve_chams);
							ImGui::ColorEdit4("sleeve viewmodel color", config_system.item.clr_sleeve_chams, ImGuiColorEditFlags_NoInputs);
						}
						
					}
					ImGui::Checkbox("hitmarker", &config_system.item.hitmarker);
					ImGui::Checkbox("force crosshair", &config_system.item.force_crosshair);
					if(config_system.item.force_crosshair)
					ImGui::ColorEdit4("crosshair color", config_system.item.clr_crosshair, ImGuiColorEditFlags_NoInputs);
					ImGui::PopStyleColor();
				}
				ImGui::EndChild(true);
				ImGui::NextColumn();

				ImGui::BeginChild("misc", ImVec2(279, 543), true); {
					
					ImGui::Checkbox("console logs", &config_system.item.logs_config_system);
					ImGui::ColorEdit4("console log color", config_system.item.clr_logs, ImGuiColorEditFlags_NoInputs);
					if (config_system.item.logs_config_system)
					{
						ImGui::Checkbox("player_hurt log", &config_system.item.logs_player_hurt);
						ImGui::Checkbox("player_bought log", &config_system.item.logs_player_bought);
					}
					ImGui::Checkbox("ragdoll gravity modifier", &config_system.item.ragdoll);
					ImGui::Checkbox("clantag changer", &config_system.item.clan_tag);
					ImGui::Checkbox("legit aa", &config_system.item.desync);
					ImGui::Checkbox("watermark", &config_system.item.watermark);
					ImGui::Checkbox("additional info", &config_system.item.additional_info);
					ImGui::Checkbox("fakelag", &config_system.item.fake_lag_enable);
					if (config_system.item.fake_lag_enable)
					{
						ImGui::SliderInt("amount", &config_system.item.fake_lag_amount, 0, 10, "%.f");
					}
					ImGui::Checkbox("matchmaking server selection", &config_system.item.relay_cluster);
					if (config_system.item.relay_cluster)
					{
						ImGui::Combo("relay cluster", &config_system.item.relay_cluster_select, "Germany\0UAE, Dubai\0US, Atlanta\0Peru\0South Africa\0India, Mumbai\0Philippines\0Singapore");
					}
					ImGui::Checkbox("offscreen esp", &config_system.item.offscreen);
					
					ImGui::Checkbox("thirdperson", &config_system.item.third_person);
					ImGui::Text(" ");
					ImGui::Text("custom color theme for menu");
					ImGui::ColorEdit4("custom theme color", config_system.item.clr_theme, ImGuiColorEditFlags_NoInputs);
					ImGui::Checkbox("rainbow color theme", &config_system.item.rainbow_theme);
				
				}
				ImGui::EndChild(true);
				

				
				ImGui::PopStyleVar();
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
				ImGui::Columns();
				
				
				break;
			
			
				

			case 2:
				
				ImGui::Columns(2, NULL, false);
				ImGui::Dummy(ImVec2(0, -2)); ImGui::SameLine();
				ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));
				ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImVec4(30 / 255.f, 30 / 255.f, 39 / 255.f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0 / 255.f, 0 / 255.f, 0 / 255.f, 0.1f));
				ImGui::BeginChild("knife", ImVec2(279, 268), true); {
					ImGui::Checkbox("enable", &config_system.item.skinchanger_enable);
					ImGui::Spacing();
					ImGui::Spacing();
					ImGui::PushStyleColor(ImGuiCol_Text, config_system.item.skinchanger_enable ? ImVec4(1.f, 1.f, 1.f, 1) : ImVec4(.6f, .6f, .6f, 1));
					ImGui::Combo("typ", &config_system.item.knife_model, "default\0bayonet\0m9\0karambit\0bowie\0butterfly\0falchion\0flip\0gut\0huntsman\0shaddow-daggers\0navaja\0stiletto\0talon\0ursus");
					ImGui::Combo(("skin"), &config_system.item.paint_kit_vector_index_knife, [](void* data, int idx, const char** out_text) {
						*out_text = parser_skins[idx].name.c_str();
						return true;
						}, nullptr, parser_skins.size(), 10);
					config_system.item.paint_kit_index_knife = parser_skins[config_system.item.paint_kit_vector_index_knife].id;
					ImGui::Spacing();
					ImGui::Combo("condition", &config_system.item.knife_wear, "factory-new\0minimal-wear\0field-tested\0well-worn\0battle-scarred");
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 18);
					ImGui::Spacing();
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 18);
					if (ImGui::Button("force update", ImVec2(84, 18))) {
						utilities::force_update();
					}
					ImGui::PopStyleColor();

				}
				ImGui::EndChild(true);

				ImGui::PopStyleVar();
				ImGui::Dummy(ImVec2(0, -2)); ImGui::SameLine();
				ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));

				ImGui::BeginChild("gloves", ImVec2(279, 267), true); {
					ImGui::Checkbox("enable", &config_system.item.glovechanger_enable);
					ImGui::Spacing();
					ImGui::Spacing();
					ImGui::PushStyleColor(ImGuiCol_Text, config_system.item.glovechanger_enable ? ImVec4(1.f, 1.f, 1.f, 1) : ImVec4(.6f, .6f, .6f, 1));
					ImGui::Combo("typ", &config_system.item.glove_model, "default\0blood\0sport\0slick\0leather\0moto\0speci\0hydra");
					ImGui::Combo(("skin"), &config_system.item.paint_kit_vector_index_glove, [](void* data, int idx, const char** out_text) {
						*out_text = parser_gloves[idx].name.c_str();
						return true;
						}, nullptr, parser_gloves.size(), 10);
					config_system.item.paint_kit_index_glove = parser_gloves[config_system.item.paint_kit_vector_index_glove].id;
					ImGui::Spacing();
					ImGui::Combo("condition", &config_system.item.glove_wear, "factory-new\0minimal-wear\0field-tested\0well-worn\0battle-scarred");
					ImGui::Spacing();
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 18);
					if (ImGui::Button("force update", ImVec2(84, 18))) {
						utilities::force_update();
					}
					ImGui::PopStyleColor();
				}
				ImGui::EndChild(true);

				ImGui::NextColumn();

				ImGui::BeginChild("weapons", ImVec2(279, 543), true); {
					static int weapons_page = 0;
					if (ImGui::ButtonT("pistol", ImVec2(50, 30), weapons_page, 0, false, ImColor(0, 0, 0))) weapons_page = 0; ImGui::SameLine(0, 0);
					if (ImGui::ButtonT("rifle", ImVec2(50, 30), weapons_page, 1, false, false)) weapons_page = 1; ImGui::SameLine(0, 0);
					if (ImGui::ButtonT("sniper", ImVec2(50, 30), weapons_page, 2, false, false)) weapons_page = 2; ImGui::SameLine(0, 0);
					if (ImGui::ButtonT("smg", ImVec2(50, 30), weapons_page, 3, false, false)) weapons_page = 3; ImGui::SameLine(0, 0);
					if (ImGui::ButtonT("heavy", ImVec2(50, 30), weapons_page, 4, false, false)) weapons_page = 4;

					ImGui::PushStyleColor(ImGuiCol_Text, config_system.item.skinchanger_enable ? ImVec4(1.f, 1.f, 1.f, 1) : ImVec4(.6f, .6f, .6f, 1));
					switch (weapons_page) {
					case 0:
						ImGui::Combo(("p2000"), &config_system.item.paint_kit_vector_index_p2000, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
							}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_p2000 = parser_skins[config_system.item.paint_kit_vector_index_p2000].id;


						ImGui::Combo(("usp-s"), &config_system.item.paint_kit_vector_index_usp, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
							}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_usp = parser_skins[config_system.item.paint_kit_vector_index_usp].id;

						ImGui::Combo(("glock"), &config_system.item.paint_kit_vector_index_glock, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
							}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_glock = parser_skins[config_system.item.paint_kit_vector_index_glock].id;

						ImGui::Combo(("p250"), &config_system.item.paint_kit_vector_index_p250, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
							}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_p250 = parser_skins[config_system.item.paint_kit_vector_index_p250].id;

						ImGui::Combo(("five-seven"), &config_system.item.paint_kit_vector_index_fiveseven, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
							}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_fiveseven = parser_skins[config_system.item.paint_kit_vector_index_fiveseven].id;

						ImGui::Combo(("tec9"), &config_system.item.paint_kit_vector_index_tec, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
							}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_tec = parser_skins[config_system.item.paint_kit_vector_index_tec].id;

						ImGui::Combo(("cz75a"), &config_system.item.paint_kit_vector_index_cz, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
							}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_cz = parser_skins[config_system.item.paint_kit_vector_index_cz].id;

						ImGui::Combo(("duals"), &config_system.item.paint_kit_vector_index_duals, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
							}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_duals = parser_skins[config_system.item.paint_kit_vector_index_duals].id;

						ImGui::Combo(("deagle"), &config_system.item.paint_kit_vector_index_deagle, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
							}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_deagle = parser_skins[config_system.item.paint_kit_vector_index_deagle].id;

						ImGui::Combo(("revolver"), &config_system.item.paint_kit_vector_index_revolver, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
							}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_revolver = parser_skins[config_system.item.paint_kit_vector_index_revolver].id;

						break;
					case 1:
						ImGui::Combo(("famas"), &config_system.item.paint_kit_vector_index_famas, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
							}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_famas = parser_skins[config_system.item.paint_kit_vector_index_famas].id;

						ImGui::Combo(("galil"), &config_system.item.paint_kit_vector_index_galil, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
							}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_galil = parser_skins[config_system.item.paint_kit_vector_index_galil].id;

						ImGui::Combo(("m4a4"), &config_system.item.paint_kit_vector_index_m4a4, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
							}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_m4a4 = parser_skins[config_system.item.paint_kit_vector_index_m4a4].id;

						ImGui::Combo(("m4a1"), &config_system.item.paint_kit_vector_index_m4a1, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
							}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_m4a1 = parser_skins[config_system.item.paint_kit_vector_index_m4a1].id;

						ImGui::Combo(("ak47"), &config_system.item.paint_kit_vector_index_ak47, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
							}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_ak47 = parser_skins[config_system.item.paint_kit_vector_index_ak47].id;

						ImGui::Combo(("sg 553"), &config_system.item.paint_kit_vector_index_sg553, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
							}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_sg553 = parser_skins[config_system.item.paint_kit_vector_index_sg553].id;

						ImGui::Combo(("aug"), &config_system.item.paint_kit_vector_index_aug, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
							}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_aug = parser_skins[config_system.item.paint_kit_vector_index_aug].id;

						break;
					case 2:
						ImGui::Combo(("ssg08"), &config_system.item.paint_kit_vector_index_ssg08, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
							}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_ssg08 = parser_skins[config_system.item.paint_kit_vector_index_ssg08].id;

						ImGui::Combo(("awp"), &config_system.item.paint_kit_vector_index_awp, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
							}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_awp = parser_skins[config_system.item.paint_kit_vector_index_awp].id;

						ImGui::Combo(("scar20"), &config_system.item.paint_kit_vector_index_scar, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
							}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_scar = parser_skins[config_system.item.paint_kit_vector_index_scar].id;

						ImGui::Combo(("g3sg1"), &config_system.item.paint_kit_vector_index_g3sg1, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
							}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_g3sg1 = parser_skins[config_system.item.paint_kit_vector_index_g3sg1].id;

						break;
					case 3:
						ImGui::Combo(("bizon"), &config_system.item.paint_kit_vector_index_ppbizon, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
							}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_ppbizon = parser_skins[config_system.item.paint_kit_vector_index_ppbizon].id;

						ImGui::Combo(("mp5"), &config_system.item.paint_kit_vector_index_mp5sd, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
							}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_mp5sd = parser_skins[config_system.item.paint_kit_vector_index_mp5sd].id;

						ImGui::Combo(("mp7"), &config_system.item.paint_kit_vector_index_mp7, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
							}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_mp7 = parser_skins[config_system.item.paint_kit_vector_index_mp7].id;

						ImGui::Combo(("mp9"), &config_system.item.paint_kit_vector_index_mp9, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
							}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_mp9 = parser_skins[config_system.item.paint_kit_vector_index_mp9].id;

						ImGui::Combo(("mac10"), &config_system.item.paint_kit_vector_index_mac10, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
							}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_mac10 = parser_skins[config_system.item.paint_kit_vector_index_mac10].id;

						ImGui::Combo(("p90"), &config_system.item.paint_kit_vector_index_p90, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
							}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_p90 = parser_skins[config_system.item.paint_kit_vector_index_p90].id;

						ImGui::Combo(("ump45"), &config_system.item.paint_kit_vector_index_ump45, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
							}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_ump45 = parser_skins[config_system.item.paint_kit_vector_index_ump45].id;


						break;
					case 4:
						ImGui::Combo(("sawoff"), &config_system.item.paint_kit_vector_index_sawedoff, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
							}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_sawedoff = parser_skins[config_system.item.paint_kit_vector_index_sawedoff].id;

						ImGui::Combo(("m249"), &config_system.item.paint_kit_vector_index_m249, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
							}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_m249 = parser_skins[config_system.item.paint_kit_vector_index_m249].id;

						ImGui::Combo(("negev"), &config_system.item.paint_kit_vector_index_negev, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
							}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_negev = parser_skins[config_system.item.paint_kit_vector_index_negev].id;

						ImGui::Combo(("mag7"), &config_system.item.paint_kit_vector_index_mag7, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
							}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_mag7 = parser_skins[config_system.item.paint_kit_vector_index_mag7].id;

						ImGui::Combo(("xm1014"), &config_system.item.paint_kit_vector_index_xm1014, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
							}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_xm1014 = parser_skins[config_system.item.paint_kit_vector_index_xm1014].id;

						ImGui::Combo(("nova"), &config_system.item.paint_kit_vector_index_nova, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
							}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_nova = parser_skins[config_system.item.paint_kit_vector_index_nova].id;

						break;

					}
					ImGui::PopStyleColor();
				}
				ImGui::EndChild(true);
				ImGui::PopStyleVar();
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
				ImGui::Columns();
				break;
				case 3:
			
				
			
				ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImVec4(30 / 255.f, 30 / 255.f, 39 / 255.f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0 / 255.f, 0 / 255.f, 0 / 255.f, 0.1f));
				

				
			
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));


				
				ImGui::Dummy(ImVec2(-215, 0)); ImGui::SameLine();
				ImGui::BeginChild("info", ImVec2(279, 543), true); {
					char buffer[UNLEN + 1];
					DWORD size;
					size = sizeof(buffer);
					GetUserName(buffer, &size);
					char title[UNLEN];
					char ch1[64] = "                          welcome, ";
					char* ch = strcat(ch1, buffer);

					ImGui::Text(ch);
					ImGui::Text("                    build: " __DATE__ " / " __TIME__);
					if (std::strstr(GetCommandLineA(), "-insecure")) {
						ImGui::Text("                            insecure mode!");
					}
				}
				ImGui::EndChild(true);
				ImGui::PopStyleVar();
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
				
				break;
				case 4:
					break;
				case 0:
					ImGui::Columns(2, NULL, false);
					ImGui::Dummy(ImVec2(0, -2)); ImGui::SameLine();
					ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
					ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));

					//push window color for child
					ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImVec4(30 / 255.f, 30 / 255.f, 39 / 255.f, 1.0f));
					//push border color for child
					ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0 / 255.f, 0 / 255.f, 0 / 255.f, 0.1f));
					ImGui::BeginChild("aimbot", ImVec2(279, 268), true); {
						ImGui::Checkbox("active", &config_system.item.aim_enabled);
						ImGui::PushStyleColor(ImGuiCol_Text, config_system.item.aim_enabled ? ImVec4(1.f, 1.f, 1.f, 1) : ImVec4(.6f, .6f, .6f, 1));
						ImGui::Combo("mode", &config_system.item.aim_mode, "hitbox\0nearest hitbox"); //todo add custom bone selection - designer
						ImGui::PopStyleColor();
						ImGui::Checkbox("dynamic fov", &config_system.item.aim_distance_based_fov);
						ImGui::Checkbox("silent aim", &config_system.item.aim_silent);
						ImGui::Checkbox("scope aim", &config_system.item.scope_aim);
						ImGui::Checkbox("smoke aim", &config_system.item.smoke_check);
						ImGui::Checkbox("friendly fire", &config_system.item.aim_team_check);
						ImGui::Checkbox("auto pistol", &config_system.item.aimbot_auto_pistol);
						ImGui::PushStyleColor(ImGuiCol_Text, config_system.item.aim_enabled ? ImVec4(1.f, 1.f, 1.f, 1) : ImVec4(.6f, .6f, .6f, 1));
						ImGui::SliderInt("kill delay", &config_system.item.aimbot_delay_after_kill, 0, 350);
						ImGui::PopStyleColor();

					} ImGui::EndChild(true);

					ImGui::PopStyleVar();
					ImGui::Dummy(ImVec2(0, -2)); ImGui::SameLine();
					ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
					ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));

					ImGui::BeginChild("additional", ImVec2(279, 267), true); {
					
						
						ImGui::Checkbox("- standalone rcs -", &config_system.item.rcs_standalone);
						if (config_system.item.rcs_standalone) {
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
							ImGui::SliderFloat(" rcs amount x", &config_system.item.rcs_standalone_x, 0.0f, 1.0f, "%.2f");
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
							ImGui::SliderFloat(" rcs amount y", &config_system.item.rcs_standalone_y, 0.0f, 1.0f, "%.2f");
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
						}
						ImGui::Checkbox("triggerbot", &config_system.item.trigger_enable);
						if (config_system.item.trigger_enable) {
							ImGui::Hotkey("##triger key", &config_system.item.trigger_key, ImVec2(100, 20));
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
							if (ImGui::BeginCombo("hitbox", "...", ImVec2(0, 105))) {
								ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8);
								ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
								ImGui::Selectable(("head"), &config_system.item.trigger_hitbox_head, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
								ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
								ImGui::Selectable(("body"), &config_system.item.trigger_hitbox_body, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
								ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
								ImGui::Selectable(("arms"), &config_system.item.trigger_hitbox_arms, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
								ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
								ImGui::Selectable(("legs"), &config_system.item.trigger_hitbox_legs, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
								ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
								ImGui::EndCombo();
								ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
							}
							ImGui::SliderInt(" trigger delay", &config_system.item.trigger_delay, 1, 50);
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
							;
						}
						
					}
					ImGui::EndChild(true);

					ImGui::NextColumn();

					ImGui::BeginChild("settings", ImVec2(279, 543), true); {
						if (ImGui::ButtonT("pistol", ImVec2(50, 30), test, 0, false, ImColor(0, 0, 0))) test = 0; ImGui::SameLine(0, 0);
						if (ImGui::ButtonT("rifle", ImVec2(50, 30), test, 1, false, false)) test = 1; ImGui::SameLine(0, 0);
						if (ImGui::ButtonT("sniper", ImVec2(50, 30), test, 2, false, false)) test = 2; ImGui::SameLine(0, 0);
						if (ImGui::ButtonT("smg", ImVec2(50, 30), test, 3, false, false)) test = 3; ImGui::SameLine(0, 0);
						if (ImGui::ButtonT("heavy", ImVec2(50, 30), test, 4, false, false)) test = 4;

						ImGui::PushFont(font_menu);

						ImGui::PushStyleColor(ImGuiCol_Text, config_system.item.aim_enabled ? ImVec4(1.f, 1.f, 1.f, 1) : ImVec4(.6f, .6f, .6f, 1));
						switch (test) {
						case 0:
							if (config_system.item.aim_mode == 0) {
								ImGui::Combo("pistol hitbox", &config_system.item.aim_bone_pistol, "head\0neck\0chest\0stomach\0pelvis");
							}
							ImGui::SliderFloat("pistol fov", &config_system.item.aim_fov_pistol, 0.0f, 180.0f, "%.2f");
							ImGui::SliderFloat("pistol smooth", &config_system.item.aim_smooth_pistol, 1.f, 10.f, "%.2f");
							ImGui::SliderFloat("pistol rcs x", &config_system.item.rcs_x_pistol, 0.0f, 1.0f, "%.2f");
							ImGui::SliderFloat("pistol rcs y", &config_system.item.rcs_y_pistol, 0.0f, 1.0f, "%.2f");
							break;
						case 1:
							if (config_system.item.aim_mode == 0) {
								ImGui::Combo("rifle hitbox", &config_system.item.aim_bone_rifle, "head\0neck\0chest\0stomach\0pelvis");
							}
							ImGui::SliderFloat("rifle fov", &config_system.item.aim_fov_rifle, 0.0f, 180.0f, "%.2f");
							ImGui::SliderFloat("rifle smooth", &config_system.item.aim_smooth_rifle, 1.f, 10.f, "%.2f");
							ImGui::SliderFloat("rifle rcs x", &config_system.item.rcs_x_rifle, 0.0f, 1.0f, "%.2f");
							ImGui::SliderFloat("rifle rcs y", &config_system.item.rcs_y_rifle, 0.0f, 1.0f, "%.2f");
							break;
						case 2:
							if (config_system.item.aim_mode == 0) {
								ImGui::Combo("sniper hitbox", &config_system.item.aim_bone_sniper, "head\0neck\0chest\0stomach\0pelvis");
							}
							ImGui::SliderFloat("sniper fov", &config_system.item.aim_fov_sniper, 0.0f, 180.0f, "%.2f");
							ImGui::SliderFloat("sniper smooth", &config_system.item.aim_smooth_sniper, 1.f, 10.f, "%.2f");
							ImGui::SliderFloat("sniper rcs x", &config_system.item.rcs_x_sniper, 0.0f, 1.0f, "%.2f");
							ImGui::SliderFloat("sniper rcs y", &config_system.item.rcs_y_sniper, 0.0f, 1.0f, "%.2f");
							break;
						case 3:
							if (config_system.item.aim_mode == 0) {
								ImGui::Combo("smg hitbox", &config_system.item.aim_bone_smg, "head\0neck\0chest\0stomach\0pelvis");
							}

							ImGui::SliderFloat("smg fov", &config_system.item.aim_fov_smg, 0.0f, 180.0f, "%.2f");
							ImGui::SliderFloat("smg smooth", &config_system.item.aim_smooth_smg, 1.f, 10.f, "%.2f");
							ImGui::SliderFloat("smg rcs x", &config_system.item.rcs_x_smg, 0.0f, 1.0f, "%.2f");
							ImGui::SliderFloat("smg rcs y", &config_system.item.rcs_y_smg, 0.0f, 1.0f, "%.2f");
							break;
						case 4:
							if (config_system.item.aim_mode == 0) {
								ImGui::Combo("heavy hitbox", &config_system.item.aim_bone_heavy, "head\0neck\0chest\0stomach\0pelvis");
							}
							ImGui::SliderFloat("heavy fov", &config_system.item.aim_fov_heavy, 0.0f, 180.0f, "%.2f");
							ImGui::SliderFloat("heavy smooth", &config_system.item.aim_smooth_heavy, 1.f, 10.f, "%.2f");
							ImGui::SliderFloat("heavy rcs x", &config_system.item.rcs_x_heavy, 0.0f, 1.0f, "%.2f");
							ImGui::SliderFloat("heavy rcs y", &config_system.item.rcs_y_heavy, 0.0f, 1.0f, "%.2f");
							break;
						}
						ImGui::PopStyleColor();

						ImGui::PopFont();
					} ImGui::EndChild(true);
					ImGui::PopStyleColor();
					ImGui::PopStyleColor();
					ImGui::PopStyleVar();
					ImGui::Columns();

					break;
			}
		}

		ImGui::PopFont();

		ImGui::End();
	}
}

void c_menu::run_popup() {
	ImGui::PushFont(font_menu);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));
	ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImVec4(30 / 255.f, 30 / 255.f, 39 / 255.f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0 / 255.f, 0 / 255.f, 0 / 255.f, 0.1f));


		bool done = false;
	



	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar();
	ImGui::PopFont();
}


