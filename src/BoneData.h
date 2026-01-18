#pragma once

#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/variant.hpp"
#include <solarxr_protocol/generated/all_generated.h>

namespace math = solarxr_protocol::datatypes::math;
using namespace godot;

class BoneData : public RefCounted {
	GDCLASS(BoneData, RefCounted)

protected:
	static void _bind_methods() {
		godot::ClassDB::bind_method(D_METHOD("getHeadPosition"), &BoneData::getHeadPosition);
		godot::ClassDB::bind_method(D_METHOD("getHeadRotation"), &BoneData::getHeadRotation);
		godot::ClassDB::bind_method(D_METHOD("getBoneLength"), &BoneData::getBoneLength);
	};

	math::Vec3f _headPosition;
	math::Quat _headRotation;
	float _boneLength;

public:
	BoneData() :
			_headPosition(math::Vec3f()), _headRotation(math::Quat()), _boneLength(0.0f)
	{}
	BoneData(math::Vec3f headPosition, math::Quat headRotation, float boneLength) :
			_headPosition(headPosition), _headRotation(headRotation), _boneLength(boneLength)
	{}
	~BoneData() override = default;

	Vector3 getHeadPosition() const {
		return Vector3(_headPosition.x(), _headPosition.y(), _headPosition.z());
	}
	Quaternion getHeadRotation() const {
		return Quaternion(_headRotation.x(), _headRotation.y(), _headRotation.z(), _headRotation.w());
	}
	float getBoneLength() const { return _boneLength; }
};
