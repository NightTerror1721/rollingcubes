openlib "blocks"
openlib "tiles"

---@type Tile
MainTile = nil


function OnInit()
    print("Initiating test block model")

    MainTile = Theme.getTile("normal1")

end


function OnDestroy()
    print("Destroying test block model")
end


---@param block Block
function OnBlockConstruct(block)
end


---@param side BlockSide
function OnBlockSideConstruct(side)
    print("building side: ", side.sideId)
end


---@param block Block
---@param cam Camera
function OnRender(block, cam)
end


---@param side BlockSide
---@param cam Camera
function OnRenderSide(side, cam)
    side:renderTile(cam, MainTile)
end


---@param block Block
---@param elapsedTime number
function OnUpdate(block, elapsedTime)
end


---@param side BlockSide
---@param elapsedTime number
function OnUpdateSide(side, elapsedTime)
end

