#include "SlimeVRInterface.h"
#include <godot_cpp/variant/typed_dictionary.hpp>
#include <flatbuffers/flatbuffers.h>
#include <ixwebsocket/IXUserAgent.h>
#include <ixwebsocket/IXWebSocket.h>
#include <solarxr_protocol/generated/all_generated.h>
#include <any>
#include "BoneData.h"

namespace tracker = solarxr_protocol::data_feed::tracker;
namespace device = solarxr_protocol::data_feed::device_data;
namespace data_feed = solarxr_protocol::data_feed;
namespace math = solarxr_protocol::datatypes::math;

static constexpr uint16_t updateRate = 200;

void SlimeVRInterface::_bind_methods() {
	godot::ClassDB::bind_method(D_METHOD("start"), &SlimeVRInterface::start);
	ADD_SIGNAL(MethodInfo("skeleton_data_updated"));
}

void SlimeVRInterface::start() {
	std::string url("ws://localhost:21110");
	webSocket.setUrl(url);

	log("Connecting to " + url + "...");

	webSocket.setOnMessageCallback([this](const ix::WebSocketMessagePtr &msg) {
		if (msg->type == ix::WebSocketMessageType::Message) {
			//proccess the recieved message
			log("[Interface] Received message: " + std::to_string(msg->str.size()));
			//only proccess the message if it is binary
			if (msg->binary) {
				processMessage(msg->str);
			}
		} else if (msg->type == ix::WebSocketMessageType::Open) {
			//request data when connection is successful
			log("[Interface] Connection established");
			subscribeToDataFeed();
		} else if (msg->type == ix::WebSocketMessageType::Error) {
			// Maybe SSL is not configured properly
			log("Connection error: " + msg->errorInfo.reason);
		} else {
			//indicate an unexpected message type
			log("[Interface] Unexpected message Type: " + std::to_string((int)msg->type));
		}
	});

	webSocket.start();

	//fire a signal when websocket is ready?
}

void SlimeVRInterface::log(std::string str) {
	//godot::print_line(godot::String(str.c_str()));
	std::cout << "[SlimeVR] " << str << std::endl;
}

void SlimeVRInterface::subscribeToDataFeed() {
	flatbuffers::FlatBufferBuilder builder(1024 * 1024);

	//construct data feed subscription message
	auto physicalTrackerMask = tracker::CreateTrackerDataMask(builder, true, true, true, true);
	auto deviceDataMask = device::CreateDeviceDataMask(builder, physicalTrackerMask, true);
	auto virtualTrackerMask = tracker::CreateTrackerDataMask(builder, true, true, true, true);
	auto dataFeedConfig = data_feed::CreateDataFeedConfig(builder,
		1000 / updateRate, deviceDataMask, virtualTrackerMask, true);
	std::vector<flatbuffers::Offset<data_feed::DataFeedConfig>> dataFeedConfigs = { dataFeedConfig };
	auto dataFeedsVector = builder.CreateVector(dataFeedConfigs);
	auto startDataFeed = data_feed::CreateStartDataFeed(builder, dataFeedsVector);
	auto dataFeedMessageHeader = data_feed::CreateDataFeedMessageHeader(builder,
		data_feed::DataFeedMessage::StartDataFeed, startDataFeed.Union());
	std::vector<flatbuffers::Offset<data_feed::DataFeedMessageHeader>> dataFeedMessages = { dataFeedMessageHeader };
	auto dataFeedMessagesVector = builder.CreateVector(dataFeedMessages);
	auto messageBundle = solarxr_protocol::CreateMessageBundle(builder, dataFeedMessagesVector);
	builder.Finish(messageBundle);

	log("Sending data feed subscription message...");

	uint8_t *buf = builder.GetBufferPointer();

	int size = builder.GetSize();

	ix::IXWebSocketSendData data((char *)buf, size);

	webSocket.sendBinary(data);
}

//verify integretiy of recieved message and extract information
void SlimeVRInterface::processMessage(std::string raw) {
	uint8_t *buffer_pointer = (uint8_t *)raw.c_str();

	auto root = flatbuffers::GetRoot<solarxr_protocol::MessageBundle>(buffer_pointer);

	auto dataFeedMsgs = root->data_feed_msgs();
	if (dataFeedMsgs != nullptr) {
		log("[Interface] Message Size " + std::to_string(dataFeedMsgs->size()));
		for (auto i = 0; i < dataFeedMsgs->size(); i++) {
			auto dataFeedMsg = dataFeedMsgs->Get(i);
			if (dataFeedMsg == nullptr) {
				continue;
			}

			if (dataFeedMsg->message_type() != data_feed::DataFeedMessage::DataFeedConfig) {
				continue;
			}

			auto update = dataFeedMsg->message_as_DataFeedUpdate();

			//look for bone information in the message
			auto bones = update->bones();
			if (bones != nullptr) {
				godot::TypedDictionary<godot::String, BoneData> skeletonData;
				for (auto b = 0; b < bones->size(); ++b) {
					auto bone = bones->Get(b);
					if (bone == nullptr) {
						continue;
					}

					if (bone->rotation_g() == nullptr) {
						continue;
					}

					//place collected data into a variable
					auto boneName = std::string(EnumNameBodyPart(bone->body_part()));
					auto headPosition = *bone->head_position_g();
					auto headRotation = *bone->rotation_g();
					auto boneLength = bone->bone_length();
					BoneData boneData{ headPosition, headRotation, boneLength };
					skeletonData.set(godot::String(boneName.c_str()), &boneData);
				}

				//transmit the new skeletonData
				emit_signal("skeleton_data_updated", skeletonData);
			} else {
				log("[Interface] Bone data not found");
			}
		}
	}
}
