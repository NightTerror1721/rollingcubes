openlib "base"
openlib "Rollingcube.tiles"


material = nil


function OnInit()
    material = Material()
    material.diffuseColor = vec3(1, 1, 1)
    material.ambientColor = vec3(0.1, 0.1, 0.1)
    material.specularColor = vec3(0.3, 0.3, 0.3)
    material.shininess = 50

    material.diffuseTexture = Theme.getTexture("tile1")
    material.specularTexture = Theme.getTexture("tile1_sm")
    material.normalsTexture = Theme.getTexture("tile1_nm")
end


function OnDestroy()
    material = nil
end


function OnRender(tile, sideId, renderData)
    renderData.material = material
    tile:renderQuad(sideId, renderData)
end
