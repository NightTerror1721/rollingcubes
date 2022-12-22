include "skybox_utils"


function OnInit()
    error("ERROR: NEED DEFINE 'OnInit' FUNCTION IN SKYBOX TEMPLATE!!")
end


function OnDestroy()
    SkyboxUtils.destroy()
end


---@param skybox Skybox
function OnConstruct(skybox)
    SkyboxUtils.construct(skybox)
end


---@param skybox Skybox
---@param elapsedTime number
function OnUpdate(skybox, elapsedTime)
    SkyboxUtils.update(skybox, elapsedTime)
end


---@param skybox Skybox
---@param cam Camera
function OnRender(skybox, cam)
    SkyboxUtils.render(skybox, cam)
end
