--PBAX 15.09.19

CutScene = {}                         -- CutScene class
CutScene_mt = { __index = CutScene }    -- CutScene metatable
setmetatable(CutScene, CutScene_mt)

CutScene_instance = nil

function CutScene:init()
	CutScene_instance = Engine.CutScene.new()
	CutScene_instance:AddPoint(vec3.new(0.0, 0.0, 0.0), vec3.new(0.0, 0.0, -1.0),  5.0) --1
	CutScene_instance:AddPoint(vec3.new(0.0, 0.0, 5.0), vec3.new(1.0, 0.0, 0.0),  5.0) --2
	--CutScene_instance:AddPoint(vec3.new(1.0, 16.0,2.0), vec3.new(2.0, 0.0, -4.0),  5.6) --3
	--CutScene_instance:AddPoint(vec3.new(8.0, 0.0, 4.0), vec3.new(1.0, 0.0, 4.0),  5.4) --4
	--
	--CutScene_instance:AddPoint(vec3.new(6.0, 1.0, 5.0), vec3.new(54.0, -1.0, -5.0),  0.7) --5
	--CutScene_instance:AddPoint(vec3.new(32.0, 1.0, 5.0), vec3.new(12.0, -1.0, 5.0),  0.3) --6
	--CutScene_instance:AddPoint(vec3.new(3.0, 1.0, 5.0), vec3.new(32.0, -1.0, -5.0),  0.5) --7
	--CutScene_instance:AddPoint(vec3.new(67.0, 1.0, 5.0), vec3.new(63.0, -1.0, 5.0),  0.9) --8

	CutScene_instance:Start()
end

function CutScene:main()
	self:UpdateCutScene()
end

-- Want to know more keys? See *submodules*\DXTK\Inc\Keyboard.h (start with 35 line)
-- Or See The Ansi Codes
function CutScene:UpdateCutScene()
	--if (Engine.Keyboard.IsKeyDown(66)) then
	--	CutScene_instance:DeletePoints()
	--end
	CutScene_instance:Update()
end
