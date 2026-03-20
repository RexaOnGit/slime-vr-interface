#pragma once

#include "godot_cpp/classes/ref_counted.hpp"

using namespace godot;

class GodotUtilities : public RefCounted {
	GDCLASS(GodotUtilities, RefCounted)
		
protected:
	static void _bind_methods() {
		//godot::ClassDB::bind_method(D_METHOD("GodotToCString"), &GodotUtilities::GodotToCString);
	}

public:
	GodotUtilities() = default;
	~GodotUtilities() override = default;

	// Convert a Godot String to an std::string
	static void GodotToCString(String gString, std::string& cstring) {
		cstring = gString.utf8().get_data();
	}
};
