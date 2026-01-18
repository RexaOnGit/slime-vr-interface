#pragma once

#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/variant.hpp"
#include <ixwebsocket/IXWebSocket.h>

using namespace godot;

class SlimeVRInterface : public RefCounted {
	GDCLASS(SlimeVRInterface, RefCounted)

protected:
	static void _bind_methods();

public:
	SlimeVRInterface() = default;
	~SlimeVRInterface() override = default;

	void start();

private:
	ix::WebSocket webSocket;

	void log(std::string);

	void subscribeToDataFeed();

	void processMessage(std::string);
};
