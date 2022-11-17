include "ball_utils"


function OnInit()
    error("ERROR: NEED DEFINE 'OnInit' FUNCTION IN BALL TEMPLATE!!")
end


function OnDestroy()
    BallUtils.destroy()
end


function OnConstruct(ball)
    BallUtils.construct(ball)
end


function OnRender(ball, cam)
    BallUtils.render(ball, cam)
end


function OnUpdate(ball, elapsedTime)
    BallUtils.update(ball, elapsedTime)
end


function OnLevelPostUpdate(ball, elapsedTime)
    BallUtils.levelPostUpdate(ball, elapsedTime)
end


function OnCollide(ball, event)
    BallUtils.collide(ball, event)
end
