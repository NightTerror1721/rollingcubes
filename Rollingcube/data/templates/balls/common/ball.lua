include "ball_utils"


function OnInit()
    error("ERROR: NEED DEFINE 'OnInit' FUNCTION IN BALL TEMPLATE!!")
end


function OnDestroy()
    BallUtils.destroy()
end


---@param ball Ball
function OnConstruct(ball)
    BallUtils.construct(ball)
end


---@param ball Ball
---@param cam Camera
function OnRender(ball, cam)
    BallUtils.render(ball, cam)
end


---@param ball Ball
---@param elapsedTime number
function OnUpdate(ball, elapsedTime)
    BallUtils.update(ball, elapsedTime)
end


---@param ball Ball
---@param elapsedTime number
function OnLevelPostUpdate(ball, elapsedTime)
    BallUtils.levelPostUpdate(ball, elapsedTime)
end


---@param ball Ball
---@param event BallCollideEvent
function OnCollide(ball, event)
    BallUtils.collide(ball, event)
end
