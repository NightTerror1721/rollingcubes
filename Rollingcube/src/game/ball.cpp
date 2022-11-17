#include "ball.h"

#include "engine/lua/lua.h"
#include "engine/lua/constants.h"
#include "utils/lualib_constants.h"


BallTemplateManager BallTemplateManager::Instance;






void Ball::init(BallTemplate::Ref templ)
{
	if (templ == nullptr)
	{
		logger::error("Cannot set null template on Ball");
		return;
	}

	_template = templ;
	clearLuaVariables();

	_template->onConstruct(*this);

	setScale(balls::getScale());
}

void Ball::render(const Camera& cam)
{
	if (_template != nullptr)
	{
		bindLightnigShaderRenderData(cam);
		_template->onRender(*this, cam);
	}
}

void Ball::update(Time elapsedTime)
{
	if (_template != nullptr)
		_template->onUpdate(*this, elapsedTime);
	ModelableEntity::update(elapsedTime);
}

glm::vec3 Ball::getAtBlockPosition(const std::shared_ptr<Block>& block, Block::Side::SideId sideId)
{
	/*const auto& side = block->getSide(sideId);
	const auto& sideModelMatrix = side.getModelMatrix();

	auto pos = side.getPosition();
	return pos + ()*/
	return {};
}

void Ball::setCameraPosition(Camera& cam)
{
	const float lookUp = _lookPos > 0 ? _lookPos : 0;
	const float lookDown = _lookPos < 0 ? -_lookPos : 0;
	const float dif = 1.f - lookUp - lookDown;

	const glm::vec3 hints = (_camHints.normal * dif) + ((_camHints.lookDown * lookDown) + (_camHints.lookUp * lookUp));

	const float dist = cubes::side::size * hints.x;
	const float height = cubes::side::size * hints.y;
	const float lookHeight = cubes::side::size * hints.z;

	const glm::vec3 eye = getPosition() - (getFront() * dist) + (getUp() * height);
	const glm::vec3 center = getPosition() + (getUp() * lookHeight);

	cam.lookAt(eye, center, { 0, 1, 0 });
	cam.rotateLocal(_camZRotation, { 0, 0, 1 }, true);
}

















namespace lua::lib
{
	namespace LUA_balls { static defineLuaLibraryConstructor(registerToLua, root, state); }

	void registerBallsLibToLua()
	{
		LuaLibraryManager::instance().registerLibrary(
			::lua::lib::names::balls,
			&LUA_balls::registerToLua,
			{ ::lua::lib::names::themes, ::lua::lib::names::blocks, ::lua::lib::names::models }
		);
	}
}

namespace lua::lib::LUA_balls
{

	static int collideEventGetType(const BallCollideEvent* self) { return int(self->entityType); }

	static int collideEventGetBlockSideKind(const BallCollideEvent* self) { return int(self->blockSideKind); }

	static Ball* collideEventGetBall(const BallCollideEvent* self) { return &self->ball; }

//	static Mob* collideEventGetBall(const BallCollideEvent* self) { return &self->mob; }



	static defineLuaLibraryConstructor(registerToLua, root, state)
	{
		namespace meta = lua::metamethod;


		root = root.beginClass<BallCollideEvent>("BallCollideEvent")
				.addProperty("type", &collideEventGetType)
				.addProperty("blockSideKind", &collideEventGetBlockSideKind)
				.addProperty("ball", &collideEventGetBall)
				.addProperty("mob", &collideEventGetBall)
			.endClass();


		auto clss = root.deriveClass<Ball, ModelableEntity>("Ball");
		clss
			// Fields //
			// Methods //
			;

		lua::lib::utils::addLuaLocalVariablesToClass(clss);

		root = clss.endClass();
		return true;
	}
}
