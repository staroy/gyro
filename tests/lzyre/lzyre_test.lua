print("start 1")

--print("press enter ...")
--local s = io.read()

function zyre:put_contact(me, cont)
  return(me)
end

zyre:get_wallets(function(id, name, sec)
  print("wallet [1]:", id, name)
end)

local db = database("test-db")
db["x"] = "TEST string data"
local text = db:get("x")
print(text)

--os.execute("sleep "..tonumber("2"))
--local tm = os.time() + 5

--while tm > os.time() do 
-- zyre:run(1000)
--end

local t2 = zyre:at("@lzyre_test-2.lua")
t2:event1("lzyre_test-1","MyAddress", function(me, address)
  print(me, address)
end)

zyre:run()

print("stop 1")
