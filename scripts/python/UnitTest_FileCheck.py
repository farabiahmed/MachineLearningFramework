import os
import os.path

logfolder="20190928_121201"

if os.path.isfile("log/" + logfolder + "/model_0.h5"):
    print("log/" + logfolder + "/model_0.h5")
    print("###############################")
    print("Existing model loaded.......")
    print("###############################")