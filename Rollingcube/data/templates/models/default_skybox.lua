openlib "models"


MainModel = nil
MainTexture = nil


function OnInit()
    MainModel = Theme.getSkyboxModel()

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