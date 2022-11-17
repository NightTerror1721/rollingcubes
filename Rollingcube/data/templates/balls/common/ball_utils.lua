openlib "base"
openlib "Rollingcube.balls"
openlib "Rollingcube.models"


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

function BallUtils.load(template)
    ballModel = Theme.getModelObject(BALL_MODEL_PREFIX..template)
end

function BallUtils.destroy()
    ballModel = nil
    shadowTexture = nil
end

function BallUtils.construct(ball)
end

function BallUtils.render(ball, cam)
    local renderData = ModelObjectRenderData.fromEntity(ball, cam)
    ballModel:render(renderData)
end

function BallUtils.update(ball, elapsedTime)
end

function BallUtils.levelPostUpdate(ball, elapsedTime)
end

function BallUtils.collide(ball, event)
end
