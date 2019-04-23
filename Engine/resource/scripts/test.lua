--PBAX 13.04.19
function TestConsole(Cmd)
	Engine.Console.AddCmd(Cmd);
end

function TestLogError(Text)
	Engine.Console.LogError(Text)
end

function TestLogInfo(Text)
	Engine.Console.LogInfo(Text)
end

function TestLogNormal(Text)
	Engine.Console.LogNormal(Text)
end
