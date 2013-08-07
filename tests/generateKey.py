from pyndn import NDN,Name,Interest,ContentObject,Key

k = Key()
k.generateRSA(1024)
del k
