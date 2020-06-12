
from PCM2Wav import * 
import struct

def _sample_2_bin(sample):
    #return struct.pack(self.__formats[self.sample_width], self._chr(int(sample)))
    temp=0x0E456967
    print (temp)
    print(struct.pack('I',temp))

    print (sample)
    return struct.pack('I',sample)


#_sample_2_bin(0x0E456967)
output = PCM2Wav(PCM2Wav.saleae.I2S, "untitled.csv", "example.wav")