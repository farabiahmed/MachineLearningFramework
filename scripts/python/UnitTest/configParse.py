from compiler.pycodegen import EXCEPT

def strToValue(str):
    if len(str.split(";")) > 1 :
        try:
            return [int(i) for i in str.split(";")]
        except ValueError:
            return [float(i) for i in str.split(";")]
    else:
        try:
            return int(str)
        except ValueError:
            return float(str)

configstr = "NUMBER_OF_AGENTS=2,NUMBER_OF_ROWS=5,HIDDEN_LAYERS=12;6,ETA_LEARNING_RATE=0.1"

configParams = configstr.split(",")

print(configParams[0])
print(configParams[1:])


dict = {}

for param in configParams:
    dict[param.split("=")[0]] = param.split("=")[1]
    
print(dict)
print(dict["HIDDEN_LAYERS"])
print(type(dict["HIDDEN_LAYERS"]))

print(strToValue("12;41"))
print(type(strToValue("12;41")))

print(strToValue("12"))      
print(type(strToValue("12")))

print(strToValue("3.14"))      
print(type(strToValue("3.14")))