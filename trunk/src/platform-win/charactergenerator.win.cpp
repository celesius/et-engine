#include <Windows.h>
#include <et/gui/charactergenerator.h>

using namespace et;
using namespace et::gui;

class et::gui::CharacterGeneratorPrivate
{
	public:
};

CharacterGenerator::CharacterGenerator(RenderContext* _rc, const std::string& face, size_t size) : 
	_private(new CharacterGeneratorPrivate()), _face(face), _size(size)
{
	
}

CharacterGenerator::~CharacterGenerator()
{
	delete _private;
}

CharDescriptor CharacterGenerator::generateCharacter(int value)
{
	return CharDescriptor();
}

CharDescriptor CharacterGenerator::generateBoldCharacter(int value)
{
	return CharDescriptor();
}
