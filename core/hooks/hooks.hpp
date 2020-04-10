#pragma once

namespace hooks {
	bool initialize();
	void release();

	LRESULT __stdcall wndproc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) noexcept;

	extern WNDPROC wndproc_original;
	extern HWND window;
	namespace create_move {
		using fn = bool(__stdcall*)(float, c_usercmd*);
		bool __fastcall hook(void* ecx, void* edx, int input_sample_frametime, c_usercmd* cmd);
	}
	namespace frame_stage_notify {
		using fn = void(__thiscall*)(i_base_client_dll*, int);
		void __stdcall hook(int frame_stage);

	}
	namespace scene_end {
		using fn = void(__thiscall*)(void*);
		void __stdcall hook();

	}
	namespace do_post_screen_fx {
		using fn = int(__thiscall*)(void*, int);
		int __stdcall hook(int value);

	}
	namespace override_view {
		using fn = void* (__fastcall*)(i_client_mode*, void* _this, c_viewsetup * setup);
		void __fastcall hook(void* _this, void* _edx, c_viewsetup* setup);
	}
	namespace present {
		using fn = long(__stdcall*)(IDirect3DDevice9*, const RECT*, const RECT*, HWND, const RGNDATA*);
		HRESULT  __stdcall hook(IDirect3DDevice9* pDevice, const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion);
	}
	namespace reset {
		using fn = long(__stdcall*)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
		HRESULT  __stdcall hook(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);;
	}
	namespace draw_model_execute {
		using fn = void(__thiscall*)(iv_model_render*, IMatRenderContext*, const draw_model_state_t&, const model_render_info_t&, matrix_t*);
		void __stdcall hook(IMatRenderContext* ctx, const draw_model_state_t& state, const model_render_info_t& info, matrix_t* bone_to_world);
	}

	namespace paint_traverse {
		using fn = void(__thiscall*)(i_panel*, unsigned int, bool, bool);
		void __stdcall hook(unsigned int panel, bool force_repaint, bool allow_force);
	}

	namespace lock_cursor {
		using fn = void(__thiscall*)(void*);
		void __stdcall hook();
	}

	/*namespace in_key_event {
		using fn = int(__stdcall*)(int, int, const char*);
		int __fastcall hook(void* ecx, int edx, int event_code, int key_num, const char* current_binding);
	}*/

}