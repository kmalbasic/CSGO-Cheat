#include "events.hpp"
#include "../misc/logs.hpp"
#include "../features.hpp"




#pragma comment(lib, "Winmm.lib")

c_hooked_events events;

void c_hooked_events::fire_game_event(i_game_event* event) noexcept {
	auto event_name = event->get_name();

	if (!strcmp(event_name, "player_hurt")) {
		misc::movement::event(event);
		event_logs.event_player_hurt(event);
		
	}

	else if (strstr(event->get_name(), "item_purchase")) {
		event_logs.event_item_purchase(event);
	}


	
}

int c_hooked_events::get_event_debug_id(void) noexcept {
	return EVENT_DEBUG_ID_INIT;
}

void c_hooked_events::setup() noexcept {
	m_i_debug_id = EVENT_DEBUG_ID_INIT;
	interfaces::event_manager->add_listener(this, "player_hurt", false);
	interfaces::event_manager->add_listener(this, "item_purchase", false);


	printf("Events initialized!\n");
}

void c_hooked_events::release() noexcept {
	interfaces::event_manager->remove_listener(this);
}
