#pragma once
#include "../../dependencies/common_includes.hpp"
#include "../../dependencies/utilities/DrawManager.h"
#include "../features/features.hpp"
#include "../features/misc/engine_prediction.hpp"
#include "../features/skinchanger/animfix.hpp"
#include "../menu/menu.hpp"
#include "../features/skinchanger/skinchanger.hpp"
#include "../features/visuals/visuals.hpp"
#include "../features/skinchanger/parser.hpp"
#include "../features/aimbot/trigger_simple.hpp"
#include "../features/skinchanger/glovechanger.hpp"
#include "../features/misc/fake_lag.hpp"
#include "../features/misc/logs.hpp"
#include "../features/misc/globals.hpp"
#include "../features/misc/events.hpp"
#include "../features/aimbot/aimbot.hpp"
#include "../menu/backdrop.hpp"
#include "../features/backtrack/backtrack.hpp"


/*how to get entity:

	for (int i = 1; i <= interfaces::globals->max_clients; i++) {
		auto entity = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(i));
	}

*/
bool bInitializedDrawManager = false;
hooks::create_move::fn create_move_original = nullptr;
hooks::paint_traverse::fn paint_traverse_original = nullptr;
hooks::frame_stage_notify::fn frame_stage_notify_original = nullptr;
hooks::present::fn present_original = nullptr;
hooks::reset::fn reset_original = nullptr;
hooks::lock_cursor::fn lock_cursor_original = nullptr;
hooks::override_view::fn override_view_original = nullptr;
HWND hooks::window;
WNDPROC hooks::wndproc_original = NULL;
//hooks::in_key_event::fn in_key_event_original = nullptr;
hooks::draw_model_execute::fn draw_model_execute_original = nullptr;
hooks::do_post_screen_fx::fn do_post_screen_fx_original = nullptr;
hooks::scene_end::fn scene_end_original = nullptr;




unsigned int get_virtual(void* class_, unsigned int index) { return (unsigned int)(*(int**)class_)[index]; }

bool hooks::initialize() {

	auto create_move_target = reinterpret_cast<void*>(get_virtual(interfaces::clientmode, 24));
	auto present_target = reinterpret_cast<void*>(get_virtual(interfaces::directx, 17));
	auto reset_target = reinterpret_cast<void*>(get_virtual(interfaces::directx, 16));
	auto paint_traverse_target = reinterpret_cast<void*>(get_virtual(interfaces::panel, 41));
	auto lock_cursor_target = reinterpret_cast<void*>(get_virtual(interfaces::surface, 67));
	auto override_view_target = reinterpret_cast<void*>(get_virtual(interfaces::clientmode, 18));
	auto in_key_event_target = reinterpret_cast<void*>(get_virtual(interfaces::client, 21));
	auto scene_end_target = reinterpret_cast<void*>(get_virtual(interfaces::render_view, 9));
	auto draw_model_execute_target = reinterpret_cast<void*>(get_virtual(interfaces::model_render, 21));
	auto frame_stage_notify_target = reinterpret_cast<void*>(get_virtual(interfaces::client, 37));
	auto do_post_screen_fx_target = reinterpret_cast<void*>(get_virtual(interfaces::clientmode, 44));

	
	events.setup();
	kit_parser.setup();

	window = FindWindowW(L"Valve001", NULL);
	wndproc_original = reinterpret_cast<WNDPROC>(SetWindowLongW(window, GWL_WNDPROC, reinterpret_cast<LONG>(wndproc)));

    
	
	if (MH_Initialize() != MH_OK) {
		throw std::runtime_error("failed to initialize MH_Initialize.");
		return false;
	}
	
	if (MH_CreateHook(draw_model_execute_target, &draw_model_execute::hook, reinterpret_cast<void**>(&draw_model_execute_original)) != MH_OK) {
		throw std::runtime_error("failed to initialize DME.");
		return false;
	}
	if (MH_CreateHook(present_target, &present::hook, reinterpret_cast<void**>(&present_original)) != MH_OK) {
		throw std::runtime_error("failed to initialize present.");
		return false;
	}

	if (MH_CreateHook(reset_target, &reset::hook, reinterpret_cast<void**>(&reset_original)) != MH_OK) {
		throw std::runtime_error("failed to initialize reset.");
	return false;
	}	

	if (MH_CreateHook(frame_stage_notify_target, &frame_stage_notify::hook, reinterpret_cast<void**>(&frame_stage_notify_original)) != MH_OK) {
		throw std::runtime_error("failed to initialize FSN.");
		return false;
	}

	if (MH_CreateHook(create_move_target, &create_move::hook, reinterpret_cast<void**>(&create_move_original)) != MH_OK) {
		throw std::runtime_error("failed to initialize create_move. (outdated index?)");
		return false;
	}
	if (MH_CreateHook(override_view_target, &override_view::hook, reinterpret_cast<void**>(&override_view_original)) != MH_OK) {
		throw std::runtime_error("failed to initialize override_view. (outdated index?)");
		return false;
	}

	if (MH_CreateHook(paint_traverse_target, &paint_traverse::hook, reinterpret_cast<void**>(&paint_traverse_original)) != MH_OK) {
		throw std::runtime_error("failed to initialize paint_traverse. (outdated index?)");
		return false;
	}

	if (MH_CreateHook(do_post_screen_fx_target, &do_post_screen_fx::hook, reinterpret_cast<void**>(&do_post_screen_fx_original)) != MH_OK) {
		throw std::runtime_error("failed to initialize dopostscrFX. (outdated index?)");
		return false;
	}

	if (MH_CreateHook(lock_cursor_target, &lock_cursor::hook, reinterpret_cast<void**>(&lock_cursor_original)) != MH_OK) {
		throw std::runtime_error("failed to initialize lock_cursor. (outdated index?)");
		return false;
	}

	if (MH_CreateHook(scene_end_target, &scene_end::hook, reinterpret_cast<void**>(&scene_end_original)) != MH_OK) {
		throw std::runtime_error("failed to initialize scene_end. (outdated index?)");
		return false;
	}

	/*if (MH_CreateHook(in_key_event_target, &in_key_event::hook, reinterpret_cast<void**>(&in_key_event_original)) != MH_OK) {
		throw std::runtime_error("failed to initialize in_key_event. (outdated index?)");
		return false;
	}*/

	if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK) {
		throw std::runtime_error("failed to enable hooks.");
		return false;
	}
	
	/*
	interfaces::console->get_convar("viewmodel_fov")->callbacks.set_size(false);
	interfaces::console->get_convar("viewmodel_offset_x")->callbacks.set_size(false);
	interfaces::console->get_convar("viewmodel_offset_y")->callbacks.set_size(false);
	interfaces::console->get_convar("viewmodel_offset_z")->callbacks.set_size(false);
	interfaces::console->get_convar("viewmodel_fov")->set_value(120); */

#ifdef debug_build
	console::log("[setup] hooks initialized!\n");
#endif
	return true;
}
static bool initialized = false;
void hooks::release() {
	MH_Uninitialize();
	events.release();
	MH_DisableHook(MH_ALL_HOOKS);
}
void __stdcall hooks::frame_stage_notify::hook(int frame_stage) {
	// static auto original_fn = reinterpret_cast<frame_stage_notify_fn>(client_hook->get_original(37));
	

	if (frame_stage == FRAME_RENDER_START) {

	}

	else if (frame_stage == FRAME_NET_UPDATE_POSTDATAUPDATE_START) {
		skin_changer.run();
		glove_changer.run();		
	}

	else if (frame_stage == FRAME_NET_UPDATE_START && interfaces::engine->is_in_game()) {
		
	}

	else if (frame_stage == FRAME_NET_UPDATE_END && interfaces::engine->is_in_game()) {
		
	}

	frame_stage_notify_original(interfaces::client, frame_stage);
}
void __stdcall hooks::scene_end::hook()  {
	
	visuals.chams();
	
	scene_end_original(interfaces::render_view);
}
int __stdcall hooks::do_post_screen_fx::hook(int value) {
	

	//visuals.glow();
	

	return do_post_screen_fx_original(interfaces::clientmode, value);
}

extern LRESULT ImGui_ImplDX9_WndProcHandler(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

LRESULT __stdcall hooks::wndproc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) noexcept {
	static bool pressed = false;

	if (!pressed && GetAsyncKeyState(VK_INSERT)) {
		pressed = true;
	}
	else if (pressed && !GetAsyncKeyState(VK_INSERT)) {
		pressed = false;

		menu.opened = !menu.opened;
	}

	if (menu.opened) {
		interfaces::inputsystem->enable_input(false);

	}
	else if (!menu.opened) {
		interfaces::inputsystem->enable_input(true);
	}

	if (menu.opened && ImGui_ImplDX9_WndProcHandler(hwnd, message, wparam, lparam))
		return true;

	return CallWindowProcW(wndproc_original, hwnd, message, wparam, lparam);
}

bool __fastcall hooks::create_move::hook(void* ecx, void* edx, int input_sample_frametime, c_usercmd* cmd) {
	create_move_original(input_sample_frametime, cmd);

	if (!cmd || !cmd->command_number)
		return create_move_original(input_sample_frametime, cmd);

	csgo::local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));
	player_t* local = (player_t*)interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player());
	auto weapon = local->active_weapon();


	const auto ebp = reinterpret_cast<uintptr_t*>(uintptr_t(_AddressOfReturnAddress()) - sizeof(void*));
	auto& send_packet = *reinterpret_cast<bool*>(*ebp - 0x1C);

	bool f9 = false;
	bool f10 = false;



	auto old_viewangles = cmd->viewangles;
	auto old_forwardmove = cmd->forwardmove;
	auto old_sidemove = cmd->sidemove;


	
	
	desync::handle(cmd,send_packet);
	fakelag->do_fakelag(cmd, send_packet);
	misc::movement::relay_cluster();
	misc::movement::bunny_hop(cmd);
	misc::movement::clantag_spammer();
	misc::movement::viewmodel();
	misc::movement::additional_info_data(cmd);
	misc::movement::ragdoll();
	
	

	prediction::start(cmd); 
	//aimbot.run(cmd);
    lagcomp::get().handle(local, cmd);
    trigger.trigger(cmd);
	prediction::end();



	math.correct_movement(old_viewangles, cmd, old_forwardmove, old_sidemove);

	//misc::movement::memewalk(cmd);
	misc::movement::fduck(cmd);

	cmd->forwardmove = std::clamp(cmd->forwardmove, -450.0f, 450.0f);
	cmd->sidemove = std::clamp(cmd->sidemove, -450.0f, 450.0f);
	cmd->upmove = std::clamp(cmd->upmove, -450.0f, 450.0f);

	cmd->viewangles.x = std::clamp(cmd->viewangles.x, -89.0f, 89.0f);
	cmd->viewangles.y = std::clamp(cmd->viewangles.y, -180.0f, 180.0f);
	cmd->viewangles.z = 0.0f;

	if (!g::send_packet)
		g::real_angles = cmd->viewangles;

	

	return false;
}

HRESULT __stdcall hooks::reset::hook(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	
	if (!initialized)
		reset_original (pDevice, pPresentationParameters);

	menu.invalidate_objects();
	long hr = reset_original(pDevice, pPresentationParameters);
	menu.create_objects(pDevice);

	return reset_original(pDevice, pPresentationParameters);
}

HRESULT __stdcall hooks::present::hook (IDirect3DDevice9* pDevice, const RECT* pSourceRect, const RECT* pDestRect,
	HWND hDestWindowOverride, const RGNDATA* pDirtyRegion)
{
	if (!initialized) {
		menu.apply_fonts();
		menu.setup_resent(pDevice);

		initialized = true;
	}
	if (initialized) {
		menu.pre_render(pDevice);
		menu.post_render();

		menu.run_popup();
		menu.run(pDevice);
		menu.end_present(pDevice);
	};

	static auto oPresent = present_original;
	return oPresent(pDevice, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}

void __stdcall hooks::lock_cursor::hook() {

	if (menu.opened) {
		interfaces::surface->unlock_cursor();
		return;
	}
	return lock_cursor_original(interfaces::surface);
}
void __fastcall hooks::override_view::hook(void* _this, void* _edx, c_viewsetup* setup) {
	
	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));

	if (GetAsyncKeyState(config_system.item.third_person_key) & 1)
		in_thirdperson = !in_thirdperson;

	
	if (interfaces::engine->is_connected() && interfaces::engine->is_in_game())
	{
		misc::movement::third_person();
	}



	
	override_view_original(interfaces::clientmode, _this, setup);
}

void __stdcall hooks::paint_traverse::hook(unsigned int panel, bool force_repaint, bool allow_force) {
	auto panel_to_draw = fnv::hash(interfaces::panel->get_panel_name(panel));

	switch (panel_to_draw) {
	case fnv::hash("MatSystemTopPanel"):

		if (menu.opened)
		{
			Drop::DrawBackDrop();
		};
		
		visuals.run();
		event_logs.run();
		misc::movement::run();
		misc::movement::watermark();
		misc::movement::additional_info();
		misc::movement::force_crosshair();
		misc::movement::offscreen_esp();

		break;
	}

	paint_traverse_original(interfaces::panel, panel, force_repaint, allow_force);
}


void __stdcall hooks::draw_model_execute::hook(IMatRenderContext* ctx, const draw_model_state_t& state, const model_render_info_t& info, matrix_t* bone_to_world)  {
	
	visuals.chams_misc(info);

   return draw_model_execute_original(interfaces::model_render, ctx, state, info, bone_to_world);
}






