--PBAX 15.09.19

CutScene = {}                         -- CutScene class
CutScene_mt = { __index = CutScene }    -- CutScene metatable
setmetatable(CutScene, CutScene_mt)

Obj = nil

Points = {}
function CutScene:init()
	Obj = Engine.CutScene.new()
	Obj:AddPoint(vec3.new(0.0, 0.0, 0.0), vec3.new(0.0, 0.0, 0.0),  0.4) --1
	Obj:AddPoint(vec3.new(8.0, 8.0, 8.0), vec3.new(0.0, -1.0, 0.0),  0.5) --2
	Obj:AddPoint(vec3.new(16.0, 16.0,16.0), vec3.new(2.0, 0.0, 0.0),  0.6) --3
	Obj:AddPoint(vec3.new(32.0, 32.0, 32.0), vec3.new(0.0, 0.0, 56.0),  0.4) --4
	--
	--Obj:AddPoint(vec3.new(6.0, 1.0, 5.0), vec3.new(54.0, -1.0, -5.0),  0.7) --5
	--Obj:AddPoint(vec3.new(32.0, 1.0, 5.0), vec3.new(12.0, -1.0, 5.0),  0.3) --6
	--Obj:AddPoint(vec3.new(3.0, 1.0, 5.0), vec3.new(32.0, -1.0, -5.0),  0.5) --7
	--Obj:AddPoint(vec3.new(67.0, 1.0, 5.0), vec3.new(63.0, -1.0, 5.0),  0.9) --8

	Obj:Start()
end

function CutScene:main()
	self:UpdateCutScene()
end

function CutScene:UpdateCutScene()
	Obj:Update()
end
