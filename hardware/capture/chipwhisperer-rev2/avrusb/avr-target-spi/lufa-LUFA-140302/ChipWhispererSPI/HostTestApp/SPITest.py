try:
    import hid
except ImportError:
    hid = None


class HIDSPI(object):
    CMDSPI = 0x01
    CMDBOOT = 0xFE
    
    
    def findCWSPI(self, VID=0x03EB, PID=0xBAED):
        print "Detecting HID device..."
        self.hdev = hid.device(VID, PID)        
        print "Manufacturer: %s" % self.hdev.get_manufacturer_string()
        print "Product: %s" % self.hdev.get_product_string()

    def sendHID(self, cmd, data=[]):        
        #Report is 64 bytes. -2 for our bytes
        padding = 64 - 2 - len(data)

        #Generate the message to pass to the stack
        hidmsg = []
        
        #Report ID is first byte (only 1 report here)
        hidmsg.append(0x00)

        #Command
        hidmsg.append(cmd)

        #Length of Payload (if applicable)        
        hidmsg.append(len(data))

        #Payload padded to correct size
        hidmsg = hidmsg + data
        hidmsg = hidmsg + [0]*padding

        #Pass onward
        if self.hdev == None:
            return
        elif self.hdev != 2:
        #else:
            #print "HID Write: ",
            #for i in hidmsg:
            #    print "0x%02x"%i,
            #print ""                
            if self.hdev.write(hidmsg) < 0:
                raise IOError("Error calling hdev.write()")

    def sendSPI(self, data):
        self.sendHID(self.CMDSPI, data)       
        data = self.hdev.read(65)
        if data[0] & 0x80:
            plen = data[0] & 0x7F
            results = data[1:(1+plen)]
        else:
            raise IOError("Invalid input HID Report")            
        
        #print results
        return results
        

    def jumpBootloader(self):
        self.sendHID(self.CMDBOOT)

def main():

    spi = HIDSPI()

    spi.findCWSPI()

    #spi.sendSPI([0xAB, 0xCE])

    #rv = spi.sendSPI([0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    #             0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0])


    rv = spi.sendSPI([0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
                      0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
                      0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
                      0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c])
    
    

    for i,r in enumerate(rv):
        print "%02x "%r,
        if i == 15:
            print ""
            
    print ""
    

if __name__ == '__main__':
    main()
