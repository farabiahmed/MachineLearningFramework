import heapq

class PrioritizedBoundedDictionary():   # 

    def __init__(self, capacity):
        self.heap = []
        self.dict = {}
        self.capacity = capacity

    def add(self, sample):
        key, value = sample 
        self.dict[key] = (value, 0)
        heapq.heappush(self.heap, (0, key))           
        
    def keyexist(self, key):
        if key in self.dict:
            return True
        else:
            return False
    
    def get(self, key):
        (value, counter) = self.dict[key]
        
        return value
    
    def length(self):
        print("Dict Len: ",len(self.dict))
        print("Heap Len: ",len(self.heap))
        return len(self.dict)
    
    def clean(self):
        self.heap = []
        self.dict = {}
