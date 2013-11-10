import pickle

class PartialReconfigData(object):   
    def load(self, fname):
        self.configData = pickle.load(open(fname, 'rb'))        
        klist = list(self.configData['values'].keys())
        self.limits = (min(klist), max(klist))

    def getParamSpec(self, name):
        spec = {}        
        spec['name'] = name
        spec['type'] = 'int'
        spec['limits'] = self.limits
        spec['reconfigData'] = self.configData
        
    def getPartialBitstream(self, indx):
        diffs = self.configData['values'][indx]        
        data = list(self.configData['base'])
        
        #Update base
        for diff in diffs:
            data[diff[0]] = diff[1]
        
        return data

class PartialReconfigConnection(object):
    reconfig = 52        
    CODE_READ       = 0x80
    CODE_WRITE      = 0xC0        
    
    def __init__(self):
        self.oa = None
    
    def con(self, oadc):
        self.oa = oadc
    
    def dis(self):
        self.oa = None
    
    def program(self, cfgdata):
       
        if self.oa is None:            
            return
       
        dataarray = [0x00]
       
        for data in cfgdata:
            #data = int(t, 2)
            msb = data >> 8;
            lsb = data & 0xff;            
            #print "%x %x"%(msb, lsb)            
            dataarray.append(msb)
            dataarray.append(lsb)


        self.oa.sendMessage(self.CODE_WRITE, self.reconfig, dataarray, Validate=False)

        #stat = sc.sendMessage(self.CODE_READ, self.reconfig, Validate=False, maxResp=1)
        #print "%02x"%stat[0]

        self.oa.sendMessage(self.CODE_WRITE, self.reconfig, [0x1A], Validate=False)
        
        #stat = sc.sendMessage(self.CODE_READ, self.reconfig, Validate=False, maxResp=1)
        #print "%02x"%stat[0]
