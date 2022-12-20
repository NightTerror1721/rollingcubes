openlib "models"


MainModel = nil
MainMaterial = nil

---@type string
BallMainTexture = ""


function OnInit()
    MainModel = Theme.getBallModel()

    MainMaterial = Material()
    MainMaterial.diffuseColor = vec3(1, 1, 1)
    MainMaterial.ambientColor = vec3(0.1, 0.1, 0.1)
    MainMaterial.specularColor = vec3(0.7, 0.7, 0.7)
    MainMaterial.shininess = 32

    if BallMainTexture ~= "" then
        MainMaterial.diffuseTexture = Theme.getTexture(BallMainTexture)
    end
end


function OnDestroy()
    MainModel = nil
    MainMaterial = nil
    BallMainTexture = ""
end



---@param modelobj ModelObject
---@param renderData ModelObjectRenderData
function OnRender(modelobj, renderData)
    renderData.material = MainMaterial
    modelobj:renderModel(MainModel, renderData)
end


---@param mesh Mesh
---@param renderData ModelObjectRenderData
function OnRenderMesh(mesh, renderData)
    mesh:render(renderData)
end
