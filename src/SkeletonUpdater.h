#pragma once

#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/classes/skeleton_modifier3d.hpp"
#include "godot_cpp/classes/skeleton3d.hpp"
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/variant/typed_dictionary.hpp"
#include "BoneData.h"

using namespace godot;

class SkeletonUpdater : public SkeletonModifier3D {
	GDCLASS(SkeletonUpdater, SkeletonModifier3D)

protected:
	static void _bind_methods() {
		godot::ClassDB::bind_method(
			D_METHOD("_on_skeleton_data_updated"), &SkeletonUpdater::_on_skeleton_data_updated);
	}

	void _on_skeleton_data_updated(godot::TypedDictionary<godot::String, BoneData> skeletonData) {
		godot::print_line("skelly signal fired");
		auto skeletonDataKeys = skeletonData.keys();
		for (auto i = 0; i < skeletonDataKeys.size(); i++) {
			String boneName = skeletonDataKeys[i];
			if (this->get_skeleton()->find_bone(boneName) == -1) {
				continue;
			} else {
				auto boneData = (BoneData*) ( skeletonData[boneName].get_validated_object());
				if (boneData == nullptr) {
					godot::print_line(godot::String("boneData == null"));
					continue;
				}

				Transform3D poseTransform = Transform3D(boneData->getHeadRotation());
			}
		}
	}

public:
	SkeletonUpdater() = default;
	~SkeletonUpdater() override = default;
};

