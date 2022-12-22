openlib "shader"
openlib "models"
openlib "camera"
openlib "debug"


local shader <const> = ShaderProgram.defaults.lightning

---@param fieldPrefix string
---@param fieldPostfix string
---@return string[]
local function DefineShaderUniformConstantArray8(fieldPrefix, fieldPostfix)
    local fieldArray = {}

    for i = 0, LightningShader.maxStaticLightsCount, 1 do
        fieldArray[i] = fieldPrefix.."["..i.."]."..fieldPostfix
    end

    return fieldArray
end


LightningShader = {}

LightningShader.maxStaticLightsCount = 8

LightningShader.fields = {
    flags = {
        useNormalMapping = "useNormalMapping"
    },

    material = {
        ambientColor = "material.color.ambient",
        diffuseColor = "material.color.diffuse",
        specularColor = "material.color.specular",
        diffuseTexture = "material.diffuse",
        specularTexture = "material.specular",
        normalsTexture = "material.normals",
        shininess = "material.shininess",
        opacity = "material.opacity",
    },

    staticLights = {
        count = "pointLightsCount",
        position = DefineShaderUniformConstantArray8("pointLights", "position"),
        ambientColor = DefineShaderUniformConstantArray8("pointLights", "color.ambient"),
        diffuseColor = DefineShaderUniformConstantArray8("pointLights", "color.diffuse"),
        specularColor = DefineShaderUniformConstantArray8("pointLights", "color.specular"),
        constant = DefineShaderUniformConstantArray8("pointLights", "constant"),
        linear = DefineShaderUniformConstantArray8("pointLights", "linear"),
        quadratic = DefineShaderUniformConstantArray8("pointLights", "quadratic"),
        intensity = DefineShaderUniformConstantArray8("pointLights", "intensity")
    },

    mainStaticLight = {
        useMainPointLight = "useMainPointLight",
        position = "mainPointLight.position",
        ambientColor = "mainPointLight.color.ambient",
        diffuseColor = "mainPointLight.color.diffuse",
        specularColor = "mainPointLight.color.specular",
        constant = "mainPointLight.constant",
        linear = "mainPointLight.linear",
        quadratic = "mainPointLight.quadratic",
        intensity = "mainPointLight.intensity"
    },

    directionalLight = {
        direction = "dirLight.direction",
        ambientColor = "dirLight.color.ambient",
        diffuseColor = "dirLight.color.diffuse",
        specularColor = "dirLight.color.specular",
        intensity = "dirLight.intensity"
    },

    model = "model",
    modelNormal = "modelNormal",

    viewProjection = "viewProjection",
    viewPos = "viewPos"
}


---@param material Material
function LightningShader.setMaterial(material)
    local fields <const> = LightningShader.fields.material

    shader:setVec3(fields.ambientColor, material.ambientColor)
    shader:setVec3(fields.diffuseColor, material.diffuseColor)
    shader:setVec3(fields.specularColor, material.specularColor)

    if material.diffuseTexture ~= nil then
        shader:setInt(fields.diffuseTexture, 0)
    end

    if material.specularTexture ~= nil then
        shader:setInt(fields.specularTexture, 1)
    end

    if material.normalsTexture ~= nil then
        shader:setBoolean(LightningShader.fields.flags.useNormalMapping, true)
        shader:setInt(fields.normalsTexture, 2)
    else
        shader:setBoolean(LightningShader.fields.flags.useNormalMapping, false)
    end

    shader:setFloat(fields.shininess, material.shininess)
    shader:setFloat(fields.opacity, material.opacity)
end


---@param count integer
function LightningShader.setStaticLightsCount(count)
    count = math.max(0, math.min(8, count))
    shader:setInt(LightningShader.fields.staticLights.count, count)
end


---@param light Light
---@param index integer
function LightningShader.setStaticLight(light, index)
    local fields <const> = LightningShader.fields.staticLights

    shader:setVec3(fields.position[index], light.position)
    shader:setVec3(fields.ambientColor[index], light.ambientColor)
    shader:setVec3(fields.diffuseColor[index], light.diffuseColor)
    shader:setVec3(fields.specularColor[index], light.specularColor)
    shader:setFloat(fields.constant[index], light.constantAttenuation)
    shader:setFloat(fields.linear[index], light.linearAttenuation)
    shader:setFloat(fields.quadratic[index], light.quadraticAttenuation)
    shader:setFloat(fields.intensity[index], light.intensity)
end


---@param lightsContainer StaticLightContainer
function LightningShader.setStaticLights(lightsContainer)
    local len <const> = #lightsContainer
    LightningShader.setStaticLightsCount(len)

    for i = 0, LightningShader.maxStaticLightsCount, 1 do
        LightningShader.setStaticLight(lightsContainer[i], i)
    end
end


---@param light Light
function LightningShader.setMainStaticLight(light)
    local fields <const> = LightningShader.fields.mainStaticLight

    shader:setBoolean(fields.useMainPointLight, true)
    shader:setVec3(fields.position, light.position)
    shader:setVec3(fields.ambientColor, light.ambientColor)
    shader:setVec3(fields.diffuseColor, light.diffuseColor)
    shader:setVec3(fields.specularColor, light.specularColor)
    shader:setFloat(fields.constant, light.constantAttenuation)
    shader:setFloat(fields.linear, light.linearAttenuation)
    shader:setFloat(fields.quadratic, light.quadraticAttenuation)
    shader:setFloat(fields.intensity, light.intensity)
end


function LightningShader.setDisabledMainStaticLight()
    shader:setBoolean(LightningShader.fields.mainStaticLight.useMainPointLight, false)
end


---@param light DirectionalLight
function LightningShader.setDirectionalLight(light)
    local fields <const> = LightningShader.fields.directionalLight

    shader:setVec3(fields.direction, light.direction)
    shader:setVec3(fields.ambientColor, light.ambientColor)
    shader:setVec3(fields.diffuseColor, light.diffuseColor)
    shader:setVec3(fields.specularColor, light.specularColor)
    shader:setFloat(fields.intensity, light.intensity)
end


---@param material Material
function LightningShader.activateMaterialTextures(material)
    if material.diffuseTexture ~= nil then
        material.diffuseTexture:activate(0)
    end

    if material.specularTexture ~= nil then
        material.specularTexture:activate(1)
    elseif material.diffuseTexture ~= nil then
        material.diffuseTexture:activate(1)
    end

    if material.normalsTexture ~= nil then
        material.normalsTexture:activate(2)
    end
end


---@param material Material
function LightningShader.deactivateMaterialTextures(material)
    if material.diffuseTexture ~= nil then
        material.diffuseTexture:unbind()
    end

    if material.specularTexture ~= nil then
        material.specularTexture:unbind()
    end

    if material.normalsTexture ~= nil then
        material.normalsTexture:unbind()
    end
end


---@param data ModelObjectRenderData|TileRenderData
function LightningShader.bindRenderData(data)
    local fields <const> = LightningShader.fields

    shader:use()

--  testCam(data.camera)
    local v = vec2()
    local v2 = v.x

    local x = Camera()
    getClassName(x.eye)

    print("fase 1", data.camera)
    shader:setMat4(fields.viewProjection, data.camera.viewProjectionMatrix)
    shader:setVec3(fields.viewPos, data.camera.position)

    print("fase 2")
    if data.material ~= nil then
        LightningShader.activateMaterialTextures(data.material)
        LightningShader.setMaterial(data.material)
    end

    print("fase 3")
    shader:setMat4(fields.model, data.transform.modelMatrix)
    shader:setMat4(fields.modelNormal, data.transform.normalMatrix)

    print("fase 4")
    if data.staticLights ~= nil then
        LightningShader.setStaticLights(data.staticLights)
    end
end
