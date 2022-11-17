openlib "base"
openlib "Rollingcube.models"


mainModel = nil
mainMaterial = nil

texture = nil


function OnInit()
    mainModel = Theme.getBallModel()

    mainMaterial = Material()
    mainMaterial.diffuseColor = vec3(1, 1, 1)
    mainMaterial.ambientColor = vec3(0.1, 0.1, 0.1)
    mainMaterial.specularColor = vec3(0.7, 0.7, 0.7)
    mainMaterial.shininess = 32

    if texture ~= nil then
        mainMaterial.diffuseTexture = Theme.getTexture(texture)
    end
end


function OnDestroy()
    mainModel = nil
    mainMaterial = nil
    texture = nil
end


function OnRender(modelobj, renderData)
    renderData.material = mainMaterial
    modelobj:renderModel(mainModel, renderData)
end


function OnRenderMesh(mesh, renderData)
    mesh:render(renderData)
end
