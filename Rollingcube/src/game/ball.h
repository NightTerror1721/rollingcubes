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

private:
	BallTemplate::Ref _template = nullptr;

	bool _selectable = false;
	bool _resetView = false;
	bool _inHighJump = false;
	bool _startPositionSet = false;

	Basis _oldBasis = {};
	Basis _newBasis = {};

	glm::vec3 _oldPosition = { 0, 0, 0 };
	glm::vec3 _newPosition = { 0, 0, 0 };

	Move _lastMove = Move::None;
	Move _currentMove = Move::None;

	Jump _jump = Jump::None;

	Orientation _startOrientation = Orientation::Default;

	float _fallTime = 0;

	float _jumpHeightOverGround = 0;
	float _jumpDist = 0;
	float _jumpDestDist = 0;
	float _jumpFlyTime = 0;

	float _moveSpeed = balls::baseMoveSpeed;
	float _groundDist = balls::baseGroundDist;
	float _groundDistOffset = 0;
	float _upJumpSpeed = balls::baseUpJumpSpeed;
	float _farJumpSpeed = balls::baseFarJumpSpeed;
	float _highJumpSpeed = balls::baseHighJumpSpeed;
	float _gravity = cubes::gravity;
	float _maxFallSpeed = cubes::baseMaxFallSpeed;
	float _rotationSpeed = balls::baseRotationSpeed(_moveSpeed);
	float _delayForwardJump = balls::baseDelayForwardJump;
	float _forwardPressTime = balls::baseForwardPressTime;
	float _forwardJumpPressTime = balls::baseForwardJumpPressTime;
	float _rotationTimeStop = balls::baseRotationTimeStop;

	float _normalJumpDistance = 2;
	float _farJumpDistance = 3;
	float _highJumpDistance = 1;

	float _camMoveInterpolation = 0;
	float _camZRotation = 0;

	float _timeMultiplicator = 1;

	float _tempMoveSpeedMultiply = 1;

	CameraHints _camHints = {};

	std::shared_ptr<Block> _onBlock = nullptr;
	std::shared_ptr<Block> _blockUnderMe = nullptr;

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

public:
	constexpr BallTemplate::Ref getTemplate() const { return _template; }

	constexpr void setRadius(float radius) { _groundDist = radius; }
	constexpr float getRadius() const { return _groundDist; }

	constexpr void setGroundDistanceOffset(float offset) { _groundDistOffset = offset; }
	constexpr float getGroundDistanceOffset() const { return _groundDistOffset; }

private:
	glm::vec3 getAtBlockPosition(const std::shared_ptr<Block>& block, Block::Side::SideId sideId);
};
