print("start 2")

--print("press enter ...")
--local s = io.read()

function zyre:put_contact(me, cont)
  return(me)
end


function zyre:event1(me, address)
  print("[2]-event1:",me, address)
  return "[1]-"..me, "[1]-"..address
end

zyre:get_wallets(function(id, name, sec)
  print("[2] - wallet:", id, name)
end)

--local db = database("test-db")
--db["x"] = "TEST string data"
--local text = db:get("x")
--print(text)

--os.execute("sleep "..tonumber("2"))
--local tm = os.time() + 5

--while tm > os.time() do 
-- zyre:run(1000)
--end

zyre:run()

print("stop 2")
