from PrioritizedBoundedDictionary import PrioritizedBoundedDictionary


memory = PrioritizedBoundedDictionary(5)

memory.add(("test1", 1))
memory.add(("test2", 2))
memory.add(("test3", 3))
memory.add(("test4", 4))
memory.add(("test5", 5))

print(memory.length())

if memory.keyexist("test1"):
    ret = memory.get("test1")
    print(ret)
else:
    print("Doesnt Exist")