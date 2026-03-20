#pragma once

#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/classes/skeleton3d.hpp"
#include "godot_cpp/variant/typed_array.hpp"
#include <regex>
#include "godot_cpp/classes/file_access.hpp"
#include "godot_cpp/classes/gltf_state.hpp"
#include <filesystem>
#include "GodotUtilities.h"

using namespace godot;


class AvatarImporter : public RefCounted {
	GDCLASS(AvatarImporter, RefCounted)

protected:
	static void _bind_methods() {
		//godot::ClassDB::bind_method(D_METHOD("renameBones"), &AvatarImporter::renameBones);
		godot::ClassDB::bind_method(D_METHOD("verifyAvatarFile"), &AvatarImporter::verifyAvatarFile);
		godot::ClassDB::bind_method(D_METHOD("importAvatar"), &AvatarImporter::importAvatar);
		//godot::ClassDB::bind_method(D_METHOD("loadAvatar"), &AvatarImporter::loadAvatar);
		godot::ClassDB::bind_method(D_METHOD("avatarRuntimeError"), &AvatarImporter::avatarRuntimeError);
	}

	// Regex objects
	const std::regex rootRegex = std::regex("^.*?root.*?$", std::regex::icase);
	const std::regex hipRegex = std::regex("^.*?hip.*?$", std::regex::icase);
	const std::regex waistRegex = std::regex(
		"^.*?spine|waist|((lower|lwr)(chest|chst)).*?$", std::regex::icase);
	const std::regex chestRegex = std::regex(
		"^.*?(chest|chst)|((upper|uppr|upr)(chest|chst)).*?$", std::regex::icase);
	const std::regex neckRegex = std::regex("^.*?neck.*?$", std::regex::icase);
	const std::regex headRegex = std::regex("^.*?head|skull.*?$", std::regex::icase);
	const std::regex leftShoulderRegex = std::regex(
		"^.*?(shoulder|shldr|sldr)(left|_l_).*?$", std::regex::icase);
	const std::regex rightShoulderRegex = std::regex(
		"^.*?(shoulder|shldr|sldr)(right|_r_).*?$", std::regex::icase);
	const std::regex leftUpperArmRegex = std::regex(
		"^.*?(((upper|uppr|upr)(arm))|bicep)(left|_l_).*?$", std::regex::icase);
	const std::regex rightUpperArmRegex = std::regex(
		"^.*?(((upper|uppr|upr)(arm))|bicep)(right|_r_).*?$", std::regex::icase);
	const std::regex leftLowerArmRegex = std::regex(
		"^.*?(((lower|lowr|lwr)(arm))|flexor)(left|_l_).*?$", std::regex::icase);
	const std::regex rightLowerArmRegex = std::regex(
		"^.*?(((lower|lowr|lwr)(arm))|flexor)(right|_r_).*?$", std::regex::icase);
	const std::regex leftHandRegex = std::regex("^.*?(hand)(left|_l_).*?$", std::regex::icase);
	const std::regex rightHandRegex = std::regex("^.*?(hand)(right|_r_).*?$", std::regex::icase);
	const std::regex leftUpperLegRegex = std::regex(
		"^.*?(((upper|uppr|upr)(leg))|quad|femur)(left|_l_).*?$", std::regex::icase);
	const std::regex rightUpperLegRegex = std::regex(
		"^.*?(((upper|uppr|upr)(leg))|quad|femur)(right|_r_).*?$", std::regex::icase);
	const std::regex leftLowerLegRegex = std::regex(
		"^.*?(((lower|lowr|lwr)(leg))|calf)(left|_l_).*?$", std::regex::icase);
	const std::regex rightLowerLegRegex = std::regex(
		"^.*?(((lower|lowr|lwr)(leg))|calf)(right|_r_).*?$", std::regex::icase);
	const std::regex leftFootRegex = std::regex("^.*?(foot)(left|_l_).*?$", std::regex::icase);
	const std::regex rightFootRegex = std::regex("^.*?(foot)(right|_r_).*?$", std::regex::icase);

	// Map of nescesary bones and the regex objects to search for them
	const std::map<String, std::regex> _requiredBones = {
		{ "ROOT",                      rootRegex },
		{ "HIP",                        hipRegex },
		{ "WAIST",                    waistRegex },
		{ "CHEST",                    chestRegex },
		{ "NECK",                      neckRegex },
		{ "HEAD",                      headRegex },
		{ "LEFT_SHOULDER",     leftShoulderRegex },
		{ "RIGHT_SHOULDER",   rightShoulderRegex },
		{ "LEFT_UPPER_ARM",    leftUpperArmRegex },
		{ "RIGHT_UPPER_ARM",  rightUpperArmRegex },
		{ "LEFT_LOWER_ARM",    leftLowerArmRegex },
		{ "RIGHT_LOWER_ARM",  rightLowerArmRegex },
		{ "LEFT_HAND",             leftHandRegex },
		{ "RIGHT_HAND",           rightHandRegex },
		{ "LEFT_UPPER_LEG",    leftUpperLegRegex },
		{ "RIGHT_UPPER_LEG",  rightUpperLegRegex },
		{ "LEFT_LOWER_LEG",    leftLowerLegRegex },
		{ "RIGHT_LOWER_LEG",  rightLowerLegRegex },
		{ "LEFT_FOOT",             leftFootRegex },
		{ "RIGHT_FOOT",           rightFootRegex }
	};

	const enum fileType {
		vrm,
		glb,
		gltf
	};

	const std::map<std::string, fileType> fileTypeMap{
		{ ".vrm",   vrm },
		{ ".glb",   glb },
		{ ".gltf", gltf }
	};

	FileAccess Files;

public:
	AvatarImporter() = default;
	~AvatarImporter() override = default;

	// Load avatar file into a Node. Only call directly on existing avatar files.
	static void loadAvatar(String path, Node *node) {
		Ref<GLTFState> gltfState = memnew(GLTFState);
		gltfState.instantiate();
		Ref<GLTFDocument> gltfDocument = memnew(GLTFDocument);
		gltfDocument.instantiate();
		gltfDocument->append_from_file(path, gltfState);
		node = gltfDocument->generate_scene(gltfState);
	}

	// Import a new avatar file. Use this first on new avatars.
	void importAvatar(String avatarPath, Node *avatarNode) {
		// Try to load the avatar file
		try {
			// Path will change if source file needs to be converted
			String path = verifyAvatarFile(avatarPath);

			loadAvatar(path, avatarNode);

			// Verify skeleton structure

			auto skels = avatarNode->find_children("", "Skeleton3D");
			Skeleton3D* skeleton;
			switch (skels.size()) {
				case 0:
					avatarRuntimeError(avatarSkeletonNotFound);
				case 1:
					skeleton = (Skeleton3D*) skels[0].get_validated_object();
					break;
				default:
					avatarRuntimeError(avatarSkeletonOverload);
			}
			renameBones(*skeleton);

			// attach SkeletonUpdater node

			// save the processed avatar node to a new file in a seperate directory

		} catch (const std::exception& ex) {
			godot::print_line("Error when importing avatar file: " + *ex.what());
			return;
		}
	}

private:
	// Check the avatar file extension
	String verifyAvatarFile(String sourcePath) {
		String outPath;
		std::filesystem::path path = sourcePath.utf8().get_data();
		std::string fileExtensionString = path.extension().string();
		fileType fileExtension = fileTypeMap.at(fileExtensionString);
		switch (fileExtension) {
			case vrm:
				// Change the file extension to glb
				path.replace_extension(".glb");
				outPath = path.c_str();
				break;
			case glb:
			case gltf:
				outPath = sourcePath;
				break;
			default:
				avatarRuntimeError(avatarFileTypeError + String(fileExtensionString.c_str()));
		}

		return outPath;
	}

	// Change bone names in avatar to match SolarXR protocol names
	void renameBones(Skeleton3D skeleton) {
		const int boneCount = skeleton.get_bone_count();
		if (boneCount == 0) {
			avatarRuntimeError(avatarSkeletonEmpty);
		}
		std::vector<int> untestedBones(boneCount);
		for (int i = 0; i < boneCount; i++) {
			untestedBones[i] = i;
		}

		// Look for remaining bones
		for (const auto &pair : _requiredBones) {
			for (auto i = 0; i < untestedBones.size(); i++) {
				auto boneIndex = untestedBones[i];
				std::string boneName;
				GodotUtilities::GodotToCString(skeleton.get_bone_name(boneIndex), boneName);
				if (std::regex_search(boneName, pair.second)) {
					skeleton.set_bone_name(boneIndex, pair.first);
					untestedBones.erase(untestedBones.begin() + i);
					break;
				} else if (i == (untestedBones.size() - 1)) {
					avatarRuntimeError(avatarSkeletonBoneNotFound + pair.first);
				}
			}
		}
	}

	// Take a godot string and throw a runtime error
	void avatarRuntimeError(String message) {
		std::string convertedMessage;
		GodotUtilities::GodotToCString(message, convertedMessage);
		throw std::runtime_error(convertedMessage);
	}

	// Error message templates
	String avatarFileTypeError = "Avatar file type is invalid. Expected: vrm, glb, or gltf. Found: ";
	String avatarSkeletonNotFound = "Avatar skeleton not found.";
	String avatarSkeletonOverload = "Avatar has too many skeletons.";
	String avatarSkeletonEmpty = "Avatar skeleton has no bones.";
	String avatarSkeletonBoneNotFound = "Could not find avatar skeleton bone: ";
};
