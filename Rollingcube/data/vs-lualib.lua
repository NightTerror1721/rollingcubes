---@meta

---@param libname RollingcubeLibname
---@return boolean
function openlib(libname) end

---@param path string
---@param fromRoot? boolean
---@return boolean
function include(path, fromRoot) end


---@alias RollingcubeLibname
---| '"geometry"'
---| '"gl"'
---| '"debug"'
---| '"camera"'
---| '"shader"'
---| '"entities"'
---| '"themes"'
---| '"blocks"'
---| '"tiles"'
---| '"models"'
---| '"balls"'
---| '"skyboxes"'


--- class vec2 ---

---@class vec2
---@field x number
---@field y number
---@field r number
---@field g number
---@field s number
---@field t number
---@operator add(vec2):vec2
---@operator sub(vec2):vec2
---@operator mul(vec2|number):vec2
---@operator div(vec2|number):vec2
---@operator unm:vec2
---@operator len:number
---@overload fun(x?:number|vec2|vec3|vec4, y?:number): vec2
vec2 = {
    ---method
    ---@param self vec2
    ---@return vec2
    normalize = function(self) end,

    ---method
    ---@param self vec2
    ---@return number
    length = function(self) end,

    ---method
    ---@param self vec2
    ---@param vector vec2
    ---@return number
    dot = function(self, vector) end
}


--- class vec3 ---

---@class vec3
---@field x number
---@field y number
---@field z number
---@field r number
---@field g number
---@field b number
---@field s number
---@field t number
---@field p number
---@operator add(vec3):vec3
---@operator sub(vec3):vec3
---@operator mul(vec3|number):vec3
---@operator div(vec3|number):vec3
---@operator unm:vec3
---@operator len:number
---@overload fun(x?:number|vec3|vec4, y?:number, z?:number): vec3
vec3 = {
    ---method
    ---@param self vec3
    ---@return vec3
    normalize = function(self) end,

    ---method
    ---@param self vec3
    ---@return number
    length = function(self) end,

    ---method
    ---@param self vec3
    ---@param vector vec3
    ---@return number
    dot = function(self, vector) end,

    ---method
    ---@param self vec3
    ---@param vector vec3
    ---@return vec3
    cross = function(self, vector) end
}


--- class vec4 ---

---@class vec4
---@field x number
---@field y number
---@field z number
---@field w number
---@field r number
---@field g number
---@field b number
---@field a number
---@field s number
---@field t number
---@field p number
---@field q number
---@operator add(vec4):vec4
---@operator sub(vec4):vec4
---@operator mul(vec4|number):vec4
---@operator div(vec4|number):vec4
---@operator unm:vec4
---@operator len:number
---@overload fun(x?:number|vec4, y?:number, z?:number, w?:number): vec4
vec4 = {
    ---method
    ---@param self vec4
    ---@return vec4
    normalize = function(self) end,

    ---method
    ---@param self vec4
    ---@return number
    length = function(self) end,

    ---method
    ---@param self vec4
    ---@param vector vec4
    ---@return number
    dot = function(self, vector) end
}


--- class mar4 ---

---@class mat4
---@field [number] number
---@operator add(mat4):vec4
---@operator sub(mat4):vec4
---@operator mul(mat4|number):vec4
---@operator div(mat4|number):vec4
---@operator unm:mat4
---@operator len:number
---@overload fun(value?: number): mat4
mat4 = {
    ---method
    ---@param self mat4
    ---@return mat4
    transpose = function(self) end,

    ---method
    ---@param self mat4
    ---@return mat4
    inverse = function(self) end,

    ---method
    ---@param self mat4
    ---@param position vec3
    ---@return mat4
    translate = function(self, position) end,

    ---method
    ---@param self mat4
    ---@param scaleFactor vec3
    ---@return mat4
    scale = function(self, scaleFactor) end,

    ---method
    ---@param self mat4
    ---@param angle number
    ---@param axis vec3
    ---@return mat4
    rotate = function(self, angle, axis) end,

    ---static method
    ---@return mat4
    identity = function() end,

    ---static method
    ---@param eye vec3
    ---@param center vec3
    ---@param up vec3
    ---@return mat4
    lookAt = function(eye, center, up) end,

    ---static method
    ---@param left number
    ---@param right number
    ---@param bottom number
    ---@param top number
    ---@param near number
    ---@param far number
    ---@return mat4
    ortho = function(left, right, bottom, top, near, far) end,

    ---static method
    ---@param fov number
    ---@param aspect number
    ---@param near number
    ---@param far number
    ---@return mat4
    perspective = function(fov, aspect, near, far) end
}


--- Math functions ---

---@param degrees number
---@return number
function radians(degrees) end

---@param radians number
---@return number
function degrees(radians) end


--- namespace GL ---

GL = {
    ---@enum GL.DepthFunction
    DepthFunction = {
        Never = 512,
        Less = 513,
        Equal = 514,
        LessOrEqual = 515,
        Greater = 516,
        NotEqual = 517,
        GreaterOrEqual = 518,
        Always = 519,
        Default = 513
    },

    ---@enum GL.CullFace
    CullFace = {
        Front = 1028,
        Back = 1029,
        FrontAndBack = 1032,
        Default = 1029
    },

    ---@enum GL.FrontFace
    FrontFace = {
        Clockwise = 2304,
        CounterClockwise = 2305,
        Default = 2305
    },


    ---@param depthFunction GL.DepthFunction
    setDepthFunction = function(depthFunction) end,

    enableDepthTest = function() end,

    disableDepthTest = function() end,

    ---@param face GL.CullFace
    setCullFace = function(face) end,

    ---@param face GL.FrontFace
    setFrontFace = function(face) end,

    enableCullFace = function() end,

    disableCullFace = function() end,
}


--- class Camera ---

---@class Camera
---@field eye vec3
---@field center vec3
---@field up vec3
---@field fov number readonly
---@field aspect number readonly
---@field nearPlane number readonly
---@field farPlane number readonly
---@field viewMatrix mat4 readonly
---@field projectionMatrix mat4 readonly
---@field viewProjectionMatrix mat4 readonly
---@field centeredViewMatrix mat4 readonly
---@field position vec3 readonly alias for 'Camera.eye' field
---@field front vec3 readonly
---@field right vec3 readonly
---@field eulerAngles vec3 readonly
---@operator call:Camera
Camera = {
    ---method
    ---@param self Camera
    ---@param delta vec3
    move = function(self, delta) end,

    ---method
    ---@param self Camera
    ---@param angle number
    ---@param axis vec3
    rotate = function(self, angle, axis) end,

    ---method
    ---@param self Camera
    ---@param angle number
    ---@param axis vec3
    rotateFree = function(self, angle, axis) end,

    ---method
    ---@param self Camera
    ---@param flag boolean
    setLockedUp = function(self, flag) end,

    ---method
    ---@param self Camera
    ---@return boolean
    isLockedUp = function(self) end,

    ---method
    ---@param self Camera
    ---@param vector vec3
    ---@return vec3
    getLocalVector = function(self, vector) end,

    ---method
    ---@param self Camera
    ---@param eye vec3
    ---@param center vec3
    ---@param up vec3
    lookAt = function(self, eye, center, up) end,

    ---method
    ---@param self Camera
    updateViewMatrix = function(self) end,

    ---method
    ---@param self Camera
    updateProjectionMatrix = function(self) end,

    ---method
    ---@param self Camera
    ---@param position vec3
    ---@return number
    getDistanceTo = function(self, position) end,

    ---method
    ---@param self Camera
    ---@param position vec3
    ---@return number
    getZDistanceTo = function(self, position) end,

    ---method
    ---@param self Camera
    ---@param model mat4
    ---@return mat4
    mvp = function(self, model) end
}


--- class ShaderProgram ---

---@class ShaderProgram
---@field id integer readonly
ShaderProgram = {
    ---method
    ---@param self ShaderProgram
    ---@return boolean
    isCreated = function(self) end,

    ---method
    ---@param self ShaderProgram
    ---@return boolean
    isValid = function(self) end,

    ---method
    ---@param self ShaderProgram
    use = function(self) end,

    ---method
    ---@param self ShaderProgram
    ---@param name string
    ---@param value number
    setFloat = function(self, name, value) end,

    ---method
    ---@param self ShaderProgram
    ---@param name string
    ---@param value number[]
    setFloatArray = function(self, name, value) end,

    ---method
    ---@param self ShaderProgram
    ---@param name string
    ---@param value integer
    setInt = function(self, name, value) end,

    ---method
    ---@param self ShaderProgram
    ---@param name string
    ---@param value integer[]
    setIntArray = function(self, name, value) end,

    ---method
    ---@param self ShaderProgram
    ---@param name string
    ---@param value integer
    setUnsignedInt = function(self, name, value) end,

    ---method
    ---@param self ShaderProgram
    ---@param name string
    ---@param value integer[]
    setUnsignedIntArray = function(self, name, value) end,

    ---method
    ---@param self ShaderProgram
    ---@param name string
    ---@param value boolean
    setBoolean = function(self, name, value) end,

    ---method
    ---@param self ShaderProgram
    ---@param name string
    ---@param value vec2
    setVec2 = function(self, name, value) end,

    ---method
    ---@param self ShaderProgram
    ---@param name string
    ---@param value vec2[]
    setVec2Array = function(self, name, value) end,

    ---method
    ---@param self ShaderProgram
    ---@param name string
    ---@param value vec3
    setVec3 = function(self, name, value) end,

    ---method
    ---@param self ShaderProgram
    ---@param name string
    ---@param value vec3[]
    setVec3Array = function(self, name, value) end,

    ---method
    ---@param self ShaderProgram
    ---@param name string
    ---@param value vec4
    setVec4 = function(self, name, value) end,

    ---method
    ---@param self ShaderProgram
    ---@param name string
    ---@param value vec4[]
    setVec4Array = function(self, name, value) end,

    ---method
    ---@param self ShaderProgram
    ---@param name string
    ---@param value mat4
    setMat4 = function(self, name, value) end,

    ---method
    ---@param self ShaderProgram
    ---@param name string
    ---@param value mat4[]
    setMat4Array = function(self, name, value) end,

    ---static method
    ---@param name string
    ---@param vertexShaderPath string
    ---@param fragmentShaderPath string
    ---@param geometryShaderPath? string
    ---@return boolean
    load = function(name, vertexShaderPath, fragmentShaderPath, geometryShaderPath) end,

    ---static method
    ---@param name string
    ---@return ShaderProgram | nil
    get = function(name) end,

    ---static method
    ---@param name string
    ---@return boolean
    exists = function(name) end,

    ---@class ShaderProgram.DefaultShadersPool
    ---@field lightning ShaderProgram readonly
    ---@field freetypeFont ShaderProgram readonly
    ---@field sky ShaderProgram readonly
    ---@field lines ShaderProgram readonly
    defaults = {}
}


--- class Material ---

---@class Material
---@field ambientColor vec3
---@field diffuseColor vec3
---@field specularColor vec3
---@field diffuseTexture Texture
---@field specularTexture Texture
---@field normalsTexture Texture
---@field shininess number
---@field opacity number
---@overload fun(): Material
Material = {
    ---method
    ---@param self Material
    ---@param color vec3
    setColor = function(self, color) end,

    ---method
    ---@param self Material
    ---@return boolean
    hasTransparency = function(self) end,

    ---method
    ---@param self Material
    bindTexturesToLightningShader = function(self) end,

    ---method
    ---@param self Material
    unbindTextures = function(self) end
}


--- enum TextureMagnificationFilter ---

---@enum TextureMagnificationFilter
TextureMagnificationFilter = {
    Nearest = 9728,
    Bilinear = 9729
}


--- enum TextureMinificationFilter ---

---@enum TextureMinificationFilter
TextureMinificationFilter = {
    Nearest = 9728,
    Bilinear = 9729,
    NearestMipmap = 9984,
    BilinearMipmap = 9987,
    Trilinear = 9987
}


--- class Texture ---

---@class Texture
---@field id integer readonly
---@field width integer readonly
---@field height integer readonly
---@field filePath string readonly
---@overload fun(): Texture
Texture = {
    ---method
    ---@param self Texture
    ---@return boolean
    isCreated = function(self) end,

    ---method
    ---@param self Texture
    ---@return boolean
    hasFile = function(self) end,

    ---method
    ---@param self Texture
    bind = function(self) end,

    ---method
    ---@param self Texture
    unbind = function(self) end,

    ---method
    ---@param self Texture
    ---@param textureUnit integer
    activate = function(self, textureUnit) end,

    ---method
    ---@param self Texture
    ---@param filter TextureMagnificationFilter
    setMagnificationFilter = function(self, filter) end,

    ---method
    ---@param self Texture
    ---@param filter TextureMinificationFilter
    setMinificationFilter = function(self, filter) end,

    ---method
    ---@param self Texture
    ---@param flag boolean
    setRepeat = function(self, flag) end,
}


--- class CubeMapTexture ---

---@class CubeMapTexture
---@field id integer readonly
---@field width integer readonly
---@field height integer readonly
---@overload fun(): CubeMapTexture
CubeMapTexture = {
    ---method
    ---@param self CubeMapTexture
    ---@param faceIndex integer
    ---@return string
    getFilePath = function(self, faceIndex) end,

    ---method
    ---@param self CubeMapTexture
    ---@return boolean
    isCreated = function(self) end,

    ---method
    ---@param self CubeMapTexture
    ---@param faceIndex integer
    ---@return boolean
    hasFile = function(self, faceIndex) end,

    ---method
    ---@param self CubeMapTexture
    bind = function(self) end,

    ---method
    ---@param self CubeMapTexture
    unbind = function(self) end,

    ---method
    ---@param self CubeMapTexture
    ---@param textureUnit integer
    activate = function(self, textureUnit) end,

    ---method
    ---@param self CubeMapTexture
    ---@param filter TextureMagnificationFilter
    setMagnificationFilter = function(self, filter) end,

    ---method
    ---@param self CubeMapTexture
    ---@param filter TextureMinificationFilter
    setMinificationFilter = function(self, filter) end,

    ---method
    ---@param self CubeMapTexture
    ---@param flag boolean
    setRepeat = function(self, flag) end,
}


--- class Light ---

---@class Light
---@field ambientColor vec3
---@field diffuseColor vec3
---@field specularColor vec3
---@field position vec3
---@field constantAttenuation number
---@field linearAttenuation number
---@field quadraticAttenuation number
---@field intensity number
---@overload fun(): Light
Light = {
    ---method
    ---@param self Light
    ---@param color vec3
    setColor = function(self, color) end,
}


--- class DirectionalLight ---

---@class DirectionalLight
---@field ambientColor vec3
---@field diffuseColor vec3
---@field specularColor vec3
---@field direction vec3
---@field intensity number
---@overload fun(): DirectionalLight
DirectionalLight = {
    ---method
    ---@param self DirectionalLight
    ---@param color vec3
    setColor = function(self, color) end,

    ---method
    ---@param self DirectionalLight
    ---@param pitch number
    ---@param yaw number
    setDirectionFromAngles = function(self, pitch, yaw) end
}


--- class StaticLightContainer ---

---@class StaticLightContainer
---@field position vec3 readonly
---@field [number] Light readonly
---@operator len:number
StaticLightContainer = {}


--- class Transformable ---

---@class Transformable
---@field position vec3
---@field rotationAngles vec3
---@field scalation vec3
---@field basisRight vec3 readonly
---@field basisForward vec3 readonly
---@field basisFront vec3 readonly
---@field basisBackward vec3 readonly
---@field globalPosition vec3 readonly
---@field globalScale vec3 readonly
---@field modelMatrix mat4 readonly
---@field normalMatrix mat4 readonly
---@field invertedModelMatrix mat4 readonly
Transformable = {
    ---method
    ---@param self Transformable
    ---@param delta vec3
    move = function(self, delta) end,

    ---method
    ---@param self Transformable
    ---@param delta vec3
    rotate = function(self, delta) end,

    ---method
    ---@param self Transformable
    ---@param delta vec3
    scale = function(self, delta) end,
}


--- class Entity ---

---@class Entity: Transformable
Entity = {
    ---method
    ---@param self Entity
    ---@param camera Camera
    render = function(self, camera) end,

    ---method
    ---@param self Entity
    ---@param camera Camera
    renderWithLightningShader = function(self, camera) end,

    ---method
    ---@param self Entity
    ---@param camera Camera
    renderBoundingVolume = function(self, camera) end,

    ---method
    ---@param self Entity
    ---@param elapsedTime number
    update = function(self, elapsedTime) end,

    ---method
    ---@param self Entity
    ---@return boolean
    hasTransparency = function(self) end,

    ---method
    ---@param self Entity
    ---@param flag boolean
    setForceTransparency = function(self, flag) end,

    ---method
    ---@param self Entity
    ---@return boolean
    isForceTransparencyEnabled = function(self) end,

    ---method
    ---@param self Entity
    ---@param camera Camera
    ---@return boolean
    isVisibleInCamera = function(self, camera) end,
}


--- class Theme ---

---@class Theme
---@field name string static readonly
Theme = {
    ---static method
    ---@param name string
    ---@return boolean
    change = function(name) end,

    ---static method
    ---@param name string
    ---@return Texture
    getTexture = function(name) end,

    ---static method
    ---@param name string
    ---@return CubeMapTexture
    getCubeMapTexture = function(name) end,

    ---static method
    ---@param name string
    ---@return Model
    getModel = function(name) end,

    ---static method
    ---@return Model
    getBallModel = function() end,

    ---static method
    ---@return Model
    getSkyboxModel = function() end,

    ---static method
    ---@param name string
    ---@return Tile
    getTile = function(name) end,

    ---static method
    ---@param name string
    ---@return ModelObject
    getModelObject = function(name) end
}


--- class TileRenderData ---

---@class TileRenderData
---@field camera Camera readonly
---@field transform Transformable readonly
---@field material Material
TileRenderData = {}


--- alias BlockSideId ---

---@alias BlockSideId
---| 0 Front
---| 1 Back
---| 2 Left
---| 3 Right
---| 4 Top
---| 5 Bottom


--- class Tile ---

---@class Tile
Tile = {
    ---method
    ---@param self Tile
    ---@param sideId BlockSideId
    ---@param renderData TileRenderData
    renderQuad = function(self, sideId, renderData) end
}


--- class BlockSide ---

---@class BlockSide
---@field sideId BlockSideId readonly
---@field id integer readonly
---@field parent Block readonly
---@field rotationAngles vec3 readonly
---@field scalation vec3 readonly
---@field normal vec3 readonly
---@field modelMatrix mat4 readonly
BlockSide = {
    ---method
    ---@param self BlockSide
    ---@param camera Camera
    ---@param tile Tile
    renderTile = function(self, camera, tile) end
}


--- class Block ---

---@class Block: Entity
---@field blockId integer readonly
Block = {
    ---method
    ---@param self Block
    ---@param sideId BlockSideId
    ---@return BlockSide
    getSide = function(self, sideId) end,

    ---method
    ---@param self Block
    ---@param camera Camera
    render = function(self, camera) end,

    ---method
    ---@generic T
    ---@param self Block
    ---@param name string
    ---@return T
    getLocalValue = function(self, name) end,

    ---method
    ---@generic T
    ---@param self Block
    ---@param name string
    ---@param value T
    setLocalValue = function(self, name, value) end,

    ---method
    ---@param self Block
    ---@param name string
    deleteLocalValue = function(self, name) end,
}


--- class ModelObjectRenderData ---

---@class ModelObjectRenderData
---@field camera Camera
---@field transform Transformable
---@field material Material
---@field staticLights StaticLightContainer
ModelObjectRenderData = {
    ---static method
    ---@param entity Entity
    ---@param camera Camera
    fromEntity = function(entity, camera) end
}


--- class Model ---

---@class Model
Model = {}


--- class ModelObject ---

---@class ModelObject
ModelObject = {
    ---method
    ---@param self ModelObject
    ---@param model Model
    ---@param renderData ModelObjectRenderData
    renderModel = function(self, model, renderData) end,

    ---method
    ---@param self ModelObject
    ---@param renderData ModelObjectRenderData
    render = function(self, renderData) end
}


--- class Mesh ---

---@class Mesh
---@field name string readonly
Mesh = {
    ---method
    ---@param self Mesh
    ---@param renderData ModelObjectRenderData
    render = function(self, renderData) end
}


--- alias BallCollideEventEntityType ---

---@alias BallCollideEventEntityType
---| 0 BlockSide
---| 1 Mob
---| 2 Ball


--- alias BallCollideEventEntityType ---

---@alias BallCollideEventBlockSideKind
---| 0 Front
---| 1 Up
---| 2 Ground


--- class BallCollideEvent ---

---@class BallCollideEvent
---@field type BallCollideEventEntityType readonly
---@field blockSideKind BallCollideEventBlockSideKind readonly
---@field ball Ball
BallCollideEvent = {}


--- class Ball ---

---@class Ball: Entity
Ball = {
    ---method
    ---@generic T
    ---@param self Ball
    ---@param name string
    ---@return T
    getLocalValue = function(self, name) end,

    ---method
    ---@generic T
    ---@param self Ball
    ---@param name string
    ---@param value T
    setLocalValue = function(self, name, value) end,

    ---method
    ---@param self Ball
    ---@param name string
    deleteLocalValue = function(self, name) end,
}


--- class Skybox ---

---@class Skybox: Entity
Skybox = {
    ---method
    ---@generic T
    ---@param self Skybox
    ---@param name string
    ---@return T
    getLocalValue = function(self, name) end,

    ---method
    ---@generic T
    ---@param self Skybox
    ---@param name string
    ---@param value T
    setLocalValue = function(self, name, value) end,

    ---method
    ---@param self Skybox
    ---@param name string
    deleteLocalValue = function(self, name) end,
}
