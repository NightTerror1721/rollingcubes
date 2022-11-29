openlib "blocks"
openlib "tiles"


mainTile = nil


function OnInit()
    print("Initiating test block model")

    mainTile = Theme.getTile("normal1")

end


function OnDestroy()
    print("Destroying test block model")
end


function OnBlockConstruct(block)
end


function OnBlockSideConstruct(side)
    print("building side: ", side.sideId)
end


function OnRender(block, cam)
end


function OnRenderSide(side, cam)
    side:renderTile(cam, mainTile)
end


function OnUpdate(block, elapsedTime)
end


function OnUpdateSide(side, elapsedTime)
end



