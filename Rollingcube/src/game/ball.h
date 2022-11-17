#pragma once

#include "engine/entities.h"

#include "ball_constants.h"
#include "modelobj.h"
#include "luadefs.h"

#include "block.h"


namespace lua::lib { void registerBallsLibToLua(); }


class Mob;

class Ball;
struct BallCollideEvent;

class BallTemplate : public LuaTemplate
{
public:
	using Ref = Reference<BallTemplate>;
	using ConstRef = ConstReference<BallTemplate>;

public:
	static constexpr std::string_view FunctionOnConstruct = "OnConstruct";
	static constexpr std::string_view FunctionOnRender = "OnRender";
	static constexpr std::string_view FunctionOnUpdate = "OnUpdate";
	static constexpr std::string_view FunctionOnLevelPostUpdate = "OnLevelPostUpdate";
	static constexpr std::string_view FunctionOnCollide = "OnCollide";
//	static constexpr std::string_view FunctionGetIsLandingOnSide = "GetIsLandingOnSide";

public:
	BallTemplate() = default;
	BallTemplate(const BallTemplate&) = delete;
	BallTemplate(BallTemplate&&) noexcept = default;
	~BallTemplate() = default;

	BallTemplate& operator= (const BallTemplate&) = delete;
	BallTemplate& operator= (BallTemplate&&) noexcept = default;

	inline Type getType() const override { return Type::Ball; }

public:
	inline void onConstruct(Ball& ball) { vcall(FunctionOnConstruct, std::addressof(ball)); }
	inline void onRender(Ball& ball, const Camera& cam) { vcall(FunctionOnRender, std::addressof(ball), std::addressof(const_cast<Camera&>(cam))); }
	inline void onUpdate(Ball& ball, Time elapsedTime) { vcall(FunctionOnUpdate, std::addressof(ball), double(elapsedTime.toSeconds())); }
	inline void onLevelPostUpdate(Ball& ball, Time elapsedTime) { vcall(FunctionOnLevelPostUpdate, std::addressof(ball), double(elapsedTime.toSeconds())); }

	void onCollide(Ball& ball, const BallCollideEvent& event);
};




class BallTemplateManager : public LuaTemplateManager<BallTemplate>
{
private:
	static BallTemplateManager Instance;

public:
	static inline BallTemplateManager& instance() { return Instance; }

private:
	inline BallTemplateManager() : LuaTemplateManager() {}
};




enum class BallCollideEventEntityType : int
{
	BlockSide = 0,
	Mob = 1,
	Ball = 2
};

enum class BallCollideEventBlockSideKind : int
{
	Front = 0,
	Up = 1,
	Ground = 2
};

struct BallCollideEvent
{
public:
	using EntityType = BallCollideEventEntityType;
	using BlockSideKind = BallCollideEventBlockSideKind;

public:
	EntityType entityType;
	union
	{
		BlockSideKind blockSideKind;
		Reference<Mob> mob;
		Reference<Ball> ball;
	};

public:
	constexpr bool isBlockSideCollision() const { return entityType == EntityType::BlockSide; }
	constexpr bool isMobCollision() const { return entityType == EntityType::Mob; }
	constexpr bool isBallCollision() const { return entityType == EntityType::Ball; }

	constexpr bool isHitBlockSideOnFront() const { return isBlockSideCollision() && blockSideKind == BlockSideKind::Front; }
	constexpr bool isHitBlockSideOnUp() const { return isBlockSideCollision() && blockSideKind == BlockSideKind::Up; }
	constexpr bool isHitBlockSideOnGround() const { return isBlockSideCollision() && blockSideKind == BlockSideKind::Ground; }
};




enum class BallMove : int
{
	None,
	Ahead,
	Up,
	Down,
	RotateLeft,
	RotateRight,
	JumpUp,
	JumpAhead,
	JumpFar,
	Falling,
	SlideDown
};

enum class BallJump : int
{
	None,
	Normal,
	Far,
	High
};

enum class LookDirection : int
{
	Front = 0,
	Up = 1,
	Down = -1
};




class Ball : public ModelableEntity, public LuaLocalVariablesContainer
{
public:
	using Move = BallMove;
	using Jump = BallJump;

public:
	struct CameraHints
	{
		glm::vec3 normal = { 3, 3.0 / 2.0, 3.0 / 4.0 };
		glm::vec3 lookUp = { 0.01, 3.355, 3.0 / 4.0 };
		glm::vec3 lookDown = { 0.2, 1.0 / 5.0, 2.0 };
	};

	struct DefaultValues
	{
		float moveSpeed = balls::baseMoveSpeed;
		float groundDist = balls::baseGroundDist;
		float groundDistOffset = 0;
		float jumpUpSpeed = balls::baseUpJumpSpeed;
		float jumpFarSpeed = balls::baseFarJumpSpeed;
		float jumpHighSpeed = balls::baseHighJumpSpeed;
		float gravity = cubes::gravity;
		float maxFallSpeed = cubes::baseMaxFallSpeed;
		float rotationSpeed = balls::baseRotationSpeed(moveSpeed);
		float lateForwardJumpTime = balls::baseLateForwardJumpTime;
		float forwardPressTime = balls::baseForwardPressTime;
		float forwardJumpPressTime = balls::baseForwardJumpPressTime;
		float rotationStopTime = balls::baseRotationStopTime;
	};

private:
	BallTemplate::Ref _template = nullptr;

	bool _selectable = true;
	bool _resetView = false;
	bool _inHighJump = false;
	bool _startPositionSet = false;
	bool _inAir = false;
	bool _movementCheck = true;

	Basis _oldBasis = {};
	Basis _newBasis = {};

	glm::vec3 _oldPosition = { 0, 0, 0 };
	glm::vec3 _newPosition = { 0, 0, 0 };

	Move _lastMove = Move::None;
	Move _currentMove = Move::None;

	Jump _jump = Jump::None;

	Orientation _startOrientation = Orientation::Default;

	DefaultValues _defaults = {};

	float _fallTime = 0;

	float _jumpHeightOverGround = 0;
	float _jumpDist = 0;
	float _jumpDestDist = 0;
	float _jumpFlyTime = 0;

	float _ySpeed = 0;
	float _dSpeed = 0;

	float _forwardPressTime = _defaults.forwardPressTime;
	float _lateForwardJumpTime = _defaults.lateForwardJumpTime;
	float _rotationStopTime = _defaults.rotationStopTime;

	float _camMoveInterpolation = 0;
	float _camZRotation = 0;

	float _timeMultiplicator = 1;

	float _tempMoveSpeedMultiply = 1;

	unsigned int _normalJumpDistance = 2;
	unsigned int _farJumpDistance = 3;
	unsigned int _highJumpDistance = 1;

	CameraHints _camHints = {};

	LookDirection _lookDir = LookDirection::Front;
	float _lookPos = 0;

	std::shared_ptr<Block> _onBlock = nullptr;
	std::shared_ptr<Block> _blockUnderMe = nullptr;
	Reference<Block::Side> _prevSide = nullptr;
	Reference<Block::Side> _lastStaticSide = nullptr;

	Block::Side::SideId _onSide = Block::Side::SideId::Top;

public:
	Ball() = default;
	Ball(const Ball&) = delete;
	Ball(Ball&&) noexcept = default;
	~Ball() = default;

	Ball& operator= (const Ball&) = delete;
	Ball& operator= (Ball&&) noexcept = default;

public:
	void init(BallTemplate::Ref templ);

	void render(const Camera& cam) override;

	void update(Time elapsedTime) override;

	void setCameraPosition(Camera& cam);

public:
	constexpr BallTemplate::Ref getTemplate() const { return _template; }

	constexpr void setRadius(float radius) { _defaults.groundDist = radius; }
	constexpr float getRadius() const { return _defaults.groundDist; }

	constexpr void setGroundDistanceOffset(float offset) { _defaults.groundDistOffset = offset; }
	constexpr float getGroundDistanceOffset() const { return _defaults.groundDistOffset; }

	constexpr void setSelectable(bool selectable) { _selectable = selectable; }
	constexpr bool isSelectable() const { return _selectable; }

	constexpr void setRotationStopTime(float time) { _rotationStopTime = time; }
	constexpr void setRotationStopTime(float time, float defaultTime) { _defaults.rotationStopTime = _rotationStopTime = time; }

	constexpr void setLookPosition(float position) { _lookPos = position; }
	constexpr float getLookPosition() const { return _lookPos; }

	constexpr void setTimeMultiplicator(float multiplicator) { _timeMultiplicator = multiplicator; }
	constexpr float getTimeMultiplicator() const { return _timeMultiplicator; }

	constexpr void setMoveSpeed(float speed) { _defaults.moveSpeed = speed; }
	constexpr float getMoveSpeed() const { return _defaults.moveSpeed; }

	constexpr void setRotationSpeed(float speed) { _defaults.rotationSpeed = speed; }
	constexpr float getRotationSpeed() const { return _defaults.rotationSpeed; }

	constexpr void setJumpUpSpeed(float speed) { _defaults.jumpUpSpeed = speed; }
	constexpr float getJumpUpSpeed() const { return _defaults.jumpUpSpeed; }

	constexpr void setJumpFarSpeed(float speed) { _defaults.jumpFarSpeed = speed; }
	constexpr float getJumpFarSpeed() const { return _defaults.jumpFarSpeed; }

	constexpr void setJumpHighSpeed(float speed) { _defaults.jumpHighSpeed = speed; }
	constexpr float getJumpHighSpeed() const { return _defaults.jumpHighSpeed; }

	constexpr void setJumpSpeeds(float up, float far, float high)
	{
		setJumpUpSpeed(up);
		setJumpFarSpeed(far);
		setJumpHighSpeed(high);
	}

	constexpr void setMaxFallSpeed(float speed) { _defaults.maxFallSpeed = speed; }
	constexpr float getMaxFallSpeed() const { return _defaults.maxFallSpeed; }

	constexpr void setGravity(float gravity) { _defaults.gravity = gravity; }
	constexpr float getGravity() const { return _defaults.gravity; }

	constexpr void setNormalJumpDistance(unsigned int amount) { _normalJumpDistance = amount; }
	constexpr unsigned int getNormalJumpDistance() const { return _normalJumpDistance; }

	constexpr void setFarJumpDistance(unsigned int amount) { _farJumpDistance = amount; }
	constexpr unsigned int getFarJumpDistance() const { return _farJumpDistance; }

	constexpr void setHighJumpDistance(unsigned int amount) { _highJumpDistance = amount; }
	constexpr unsigned int getHighJumpDistance() const { return _highJumpDistance; }

	constexpr void setJumpDistances(unsigned int normal, unsigned int far, unsigned int high)
	{
		setNormalJumpDistance(normal);
		setFarJumpDistance(far);
		setHighJumpDistance(high);
	}

	constexpr void setCurrentForwardPressTime(float time) { _forwardPressTime = time; }
	constexpr float getCurrentForwardPressTime() const { return _forwardPressTime; }

	constexpr void setForwardPressTime(float time) { _defaults.forwardPressTime = time; }
	constexpr float getForwardPressTime() const { return _defaults.forwardPressTime; }

	constexpr void setForwardJumpPressTime(float time) { _defaults.forwardJumpPressTime = time; }
	constexpr float getForwardJumpPressTime() const { return _defaults.forwardJumpPressTime; }

	constexpr void setCurrentLateForwardJumpTime(float time) { _lateForwardJumpTime = time; }
	constexpr float getCurrentLateForwardJumpTime() const { return _lateForwardJumpTime; }

	constexpr void setLateForwardJumpTime(float time) { _defaults.lateForwardJumpTime = time; }
	constexpr float getLateForwardJumpTime() const { return _defaults.lateForwardJumpTime; }

	constexpr void setCameraNormalHint(const glm::vec3& hint) { _camHints.normal = hint; }
	constexpr const glm::vec3& getCameraNormalHint() const { return _camHints.normal; }

	constexpr void setCameraLookUpHint(const glm::vec3& hint) { _camHints.lookUp = hint; }
	constexpr const glm::vec3& getCameraLookUpHint() const { return _camHints.lookUp; }

	constexpr void setCameraLookDownHint(const glm::vec3& hint) { _camHints.lookDown = hint; }
	constexpr const glm::vec3& getCameraLookDownHint() const { return _camHints.lookDown; }

	constexpr void setCameraHints(const glm::vec3& normal, const glm::vec3& lookUp, const glm::vec3& lookDown)
	{
		setCameraNormalHint(normal);
		setCameraLookUpHint(lookUp);
		setCameraLookDownHint(lookDown);
	}

	constexpr void setCameraZRotation(float rotation) { _camZRotation = rotation; }
	constexpr float getCameraZRotation() const { return _camZRotation; }

private:
	glm::vec3 getAtBlockPosition(const std::shared_ptr<Block>& block, Block::Side::SideId sideId);
};
