openlib "tiles"


MainMaterial = nil


function OnInit()
    MainMaterial = Material()
    MainMaterial.diffuseColor = vec3(1, 1, 1)
    MainMaterial.ambientColor = vec3(0.45, 0.45, 0.45)
    MainMaterial.specularColor = vec3(0.3, 0.3, 0.3)
    MainMaterial.shininess = 50

    MainMaterial.diffuseTexture = Theme.getTexture("tile1")
    MainMaterial.specularTexture = Theme.getTexture("tile1_sm")
    MainMaterial.normalsTexture = Theme.getTexture("tile1_nm")
end


function OnDestroy()
    MainMaterial = nil
end


---@param tile Tile
---@param sideId integer
---@param renderData TileRenderData
function OnRender(tile, sideId, renderData)
    renderData.material = MainMaterial
    tile:renderQuad(sideId, renderData)
end
