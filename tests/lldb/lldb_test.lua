require("lldb")

local db = database("test-db")
db["x"] = "TEST string data"
local text = db:get("x")
print(text)
