#pragma once

#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/variant.hpp"
#include <ixwebsocket/IXWebSocket.h>
#include "godot_cpp/classes/gltf_document.hpp"
#include "godot_cpp/classes/node.hpp"


using namespace godot;

class SlimeVRInterface : public RefCounted {
	GDCLASS(SlimeVRInterface, RefCounted)

protected:
	static void _bind_methods();

	String _avatarResourcePath;
	Node *_avatarNode;
	godot::Ref<GLTFDocument> _avatarGLTF;

public:
	SlimeVRInterface() = default;
	~SlimeVRInterface() override = default;

	void start();

	void avatarInit(Node*);

	String getAvatarResourcePath() const {
		return _avatarResourcePath;
	}

	void setAvatarResourcePath(String path) {
		godot::print_line(path);
		_avatarResourcePath = path;
	}

private:
	ix::WebSocket webSocket;

	void log(std::string);

	void subscribeToDataFeed();

	void processMessage(std::string);
};
