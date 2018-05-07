configstr = "NUMBER_OF_AGENTS=2,NUMBER_OF_ROWS=5,HIDDEN_LAYERS=12;6,ETA_LEARNING_RATE=0.1"

configParams = configstr.split(",")

print(configParams)


dict = {}

for param in configParams:
    dict[param.split("=")[0]] = param.split("=")[1]
    
print(dict)
print(dict["HIDDEN_LAYERS"])