#include "json.hpp"

#include "Animation/Animation.h"
#include "Font/Font.h"

using json = nlohmann::json;

void to_json(json &j, const AnimationFrame &f)
{
	j = json{ { "frame", f.frame }, { "duration", f.duration }, { "horizontalOffset", f.horizontalOffset }, { "verticalOffset", f.verticalOffset }, { "sound", f.sound } };
}

void from_json(const json &j, AnimationFrame &f)
{
	j.at("frame").get_to(f.frame);
	j.at("duration").get_to(f.duration);

	if (j.contains("horizontalOffset"))
		j.at("horizontalOffset").get_to(f.horizontalOffset);
	else
		f.horizontalOffset = 0;

	if (j.contains("verticalOffset"))
		j.at("verticalOffset").get_to(f.verticalOffset);
	else
		f.verticalOffset = 0;

	if (j.contains("sound"))
		j.at("sound").get_to(f.sound);
}

void to_json(json &j, const AnimationState &s)
{
	j = json{ { "frames", s.frames }, { "next", s.next }, { "sound", s.sound }, { "loops", s.loops } };
}

void from_json(const json &j, AnimationState &s)
{
	j.at("frames").get_to(s.frames);
	j.at("next").get_to(s.next);

	if (j.contains("sound"))
		j.at("sound").get_to(s.sound);

	if (j.contains("loops"))
		j.at("loops").get_to(s.loops);
	else
		s.loops = 1;
}

void to_json(json &j, const Animation &a)
{
	j = json{ { "initialState", a.initialState }, { "states", a.states } };
}

void from_json(const json &j, Animation &a)
{
	j.at("initialState").get_to(a.initialState);
	j.at("states").get_to(a.states);
}

void to_json(json &j, const Font &f)
{
	j = json{ { "texture", f.texture }, { "cellWidth", f.cellWidth }, { "cellHeight", f.cellHeight }, { "baseCharacter", std::string(1, f.baseCharacter) }, { "characterWidth", f.characterWidth } };
}

void from_json(const json &j, Font &f)
{
	j.at("texture").get_to(f.texture);
	j.at("cellWidth").get_to(f.cellWidth);
	j.at("cellHeight").get_to(f.cellHeight);
	std::string temp;
	j.at("baseCharacter").get_to(temp);
	f.baseCharacter = temp[0];
	j.at("characterWidth").get_to(f.characterWidth);
}