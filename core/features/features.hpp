#pragma once
#include "../../dependencies/common_includes.hpp"

namespace misc {
	namespace movement {
		void bunny_hop(c_usercmd* cmd);
		void rank_reveal() noexcept;
		void fduck(c_usercmd* cmd);
		void viewmodel() noexcept;
		void third_person() noexcept;
		void offscreen_esp() noexcept;
		void relay_cluster();
		int getfps();
		void additional_info_data(c_usercmd* cmd);
		void additional_info();
		void run() noexcept;
		void event(i_game_event* event) noexcept;
	    void draw() noexcept;
		void clantag_spammer();
		void force_crosshair()  noexcept;
		void spectators();
		void ragdoll();
		void watermark();
	};
}