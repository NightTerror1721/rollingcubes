openlib "skyboxes"
openlib "shader"
openlib "gl"


local Properties = {}


SkyboxUtils = {}

---@param name string
function SkyboxUtils.load(name)
    Properties.texture = Theme.getCubeMapTexture(name.."json")
    Properties.model = Theme.getSkyboxModel()
    Properties.shader = ShaderProgram.defaults.sky
end

function SkyboxUtils.destroy()
end

---@param skybox Skybox
function SkyboxUtils.construct(skybox)
end

---@param skybox Skybox
---@param elapsedTime number
function SkyboxUtils.update(skybox, elapsedTime)
end

---@param skybox Skybox
---@param cam Camera
function SkyboxUtils.render(skybox, cam)
    local model = Properties.model
    local shader = Properties.shader
    local texture = Properties.texture

    GL.setDepthFunction(GL.DepthFunction.LessOrEqual)
    SkyboxUtils.prepareSkyShader(skybox, cam, shader, texture)
    
    ModelObject.

end



---@param skybox Skybox
---@param cam Camera
---@param shader ShaderProgram
---@param texture Texture|CubeMapTexture
function SkyboxUtils.prepareSkyShader(skybox, cam, shader, texture)
    shader:use()
    texture:activate(0)
    shader:setInt("skybox", 0)
    shader:setMat4("view", cam.centeredViewMatrix)
    shader:setMat4("projection", cam.projectionMatrix)
end
