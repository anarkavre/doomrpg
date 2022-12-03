#ifndef __JSON_H__
#define __JSON_H__

#include <string>

#include "json.hpp"

using json = nlohmann::json;

struct AnimationFrame;
struct AnimationState;
struct Animation;
struct Font;

void to_json(json &j, const AnimationFrame &f);
void from_json(const json &j, AnimationFrame &f);
void to_json(json &j, const AnimationState &s);
void from_json(const json &j, AnimationState &s);
void to_json(json &j, const Animation &a);
void from_json(const json &j, Animation &a);
void to_json(json &j, const Font &f);
void from_json(const json &j, Font &f);

namespace JSON
{
	template <class T>
	void Parse(const std::string &text, T &object)
	{
		json j = json::parse(text);
		j.get_to(object);
	}

	template <class T>
	void Stringify(const T &object, std::string &text)
	{
		json j = object;
		text = j.dump();
	}
};

#endif