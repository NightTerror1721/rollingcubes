openlib "balls"
openlib "models"


-- CONSTANTS --
local BALL_MODEL_PREFIX <const> = "ball_"

local COLLISION_TYPE_SIDE <const> = 0
local COLLISION_TYPE_MOB <const>  = 1
local COLLISION_TYPE_BALL <const> = 2

local BLOCK_SIDE_KIND_FRONT <const>  = 0
local BLOCK_SIDE_KIND_UP <const>     = 1
local BLOCK_SIDE_KIND_GROUND <const> = 2


-- PROPERTIES --

ballModel = nil
ballScale = 1
transparentBall = false

shadowTexture = nil
shadowScale = 1


-- PRIVATE FUNCTIONS --



-- PUBLIC FUNCTIONS --

BallUtils = {}

---@param template string
function BallUtils.load(template)
    ballModel = Theme.getModelObject(BALL_MODEL_PREFIX..template)
end

function BallUtils.destroy()
    ballModel = nil
    shadowTexture = nil
end

---@param ball Ball
function BallUtils.construct(ball)
end

---@param ball Ball
---@param cam Camera
function BallUtils.render(ball, cam)
    local renderData = ModelObjectRenderData.fromEntity(ball, cam)
    ballModel:render(renderData)
end

---@param ball Ball
---@param elapsedTime number
function BallUtils.update(ball, elapsedTime)
end

---@param ball Ball
---@param elapsedTime number
function BallUtils.levelPostUpdate(ball, elapsedTime)
end

---@param ball Ball
---@param event BallCollideEvent
function BallUtils.collide(ball, event)
end
