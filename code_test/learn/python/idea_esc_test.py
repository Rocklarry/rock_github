import serial
import time
import thread

class EscPacket:
	def __init__(self):
		self.packet = []
		self.onepacket = []
		self.fd = open("command.txt","w")
	
	def save(self,text):
		self.fd.write(text)
		self.fd.write("\n")
		self.fd.flush()
		
	def crc(self,input_array):
		crc = 0xA001
		result = 0x0FFFF
		for onebyte in input_array:
			result = result ^ ( onebyte )
			for i in range(8):
				if (result & 0x1) <> 0:
					result = ( (result >> 1) & 0x7FFF ) ^ crc
				else:
					result = (result >> 1) & 0x7FFF

		return result
		
	def dump(self,packet):
		for onebyte in packet:
			print "%02x "%onebyte,
		print " "

	def pack(self,input_array):
		packet = [0xAF]
		packet.append(len(input_array) + 4)
		packet.extend(input_array)
		crc = self.crc(packet[1:])
		packet.append(crc & 0x0FF)
		packet.append( (crc >> 8) & 0x0FF )
		return packet
	
	def pack_close_loop(self,count,p,bgr):
		if (count<20): 
			for i in range(4):
				p[i] = 0;

		for i in range(4):
			p[i] = int(p[i]/2)*2;

		p[count % 4] = p[count % 4] + 1

		packet = [2]
		for p0 in p:
			packet.append(p0 & 0x0FF)
			packet.append( (p0 >> 8) & 0xFF	)
			
		packet.append( (bgr[0] & 7) | ((bgr[1] & 7) << 3) | ((bgr[2] & 3) << 6) )
		packet.append( ((bgr[2] & 4) >> 2) | ((bgr[3] & 7) << 1) )
		return self.pack(packet)


	def parse_close_loop(self,packet):
		if len(packet) < 11 or packet[0] <> 0x2 :
			print "Close loop: format wrong ",
			self.dump(packet)
			return ([-1,-1,-1,-1],[-1,-1,-1,-1])
			
		index = 1
		p = []
		for i in range(4):
			p.append( packet[index] | (packet[index+1] << 8) )
			index = index + 2
			
		bgr = [packet[index] & 7]
		bgr.append( (packet[index] >> 3) & 7)
		bgr.append( ((packet[index] >> 6) & 3) | ((packet[index+1] & 1) << 2 ) )
		bgr.append( (packet[index+1] >> 1) & 7 )

		output = "Close loop: rpm %r, bgr %r"%(p,bgr)
		self.save(output)
		print output
		return (p,bgr)

	def parse_feedback(self,packet):
		if len(packet) < 7 or packet[0] <> 0x80:
			print "Feedback: format wrong ",
			self.dump(packet)
			return (-1,-1,-1,-1,-1)
		index = 1
		
		id = packet[index] & 0x0F
		state = (packet[index] >> 4) & 0x0F
		index = index + 1

		rpm = packet[index] | ( packet[index+1] << 8 )
		index = index + 2
		
		count = packet[index]
		index = index + 1

		#packet[index] = 0x10
		index = index + 1

		voltage = packet[index] / 34 + 9.0
		index = index + 1
		
		statemapping = ["stopped","NA","NA","NA","spinning up","spinning forward","spinning in reverse","NA","NA","NA","NA","NA","NA","NA","NA","NA"]
		output =  "Feedback: id %d, state %d [%s], rpm %d, count %d, voltage %r"%(id, state, statemapping[state], rpm, count, voltage)
		self.save(output)
		print output
		return (id, state, rpm, count, voltage)
		
	def bin2pack(self,uart_packet):
		packet = []
		for i in range(len(uart_packet)):
			#packet.append(ord(uart_packet[i]))
			packet.append(ord(uart_packet[i]))
		return packet		
		
	def pack2bin(self,packet):
		uart_packet = bytearray(len(packet))
		for i in range(len(packet)):
			uart_packet[i] = packet[i] & 0x0FF
		return uart_packet
		
	def put_to_ring_buffer(self,packet):
		self.packet.extend(packet)	

	def get_packet_from_ring_buffer(self):
		for i in range(len(self.packet)):
			if self.packet[i] == 0x0AF:
				if len(self.packet) <= i + 1:
					return False
				#print "i = %d, packet_len = %d, total = %d"%(i,self.packet[i+1],len(self.packet))
				if len(self.packet) < i + self.packet[i+1]:
					return False
				self.onepacket = self.packet[i:i + self.packet[i+1]]
				self.packet = self.packet[i + self.packet[i+1]:]
				#print self.onepacket
				#print self.packet
				return True
		return False		
		
class Esc:
	def list_ports(self):
		import serial.tools.list_ports
		allport = serial.tools.list_ports.comports()
		for oneport in allport:
			print oneport

	def open_uart(self,port_number):
		self.uart = serial.Serial("COM%d"%port_number,250000,timeout=0)

	def read_packet(self):
		packet = self.uart.read(256)
		return packet

	def send_packet(self,packet):
		self.uart.write(packet)

	def close_uart(self):
		self.uart.close()

	def dump(self,packet):
		for onebyte in packet:
			print "%02x "%onebyte,
		print " "

	def parse_packet(self,packet):
		if len(packet) < 1:
			return
			
		if packet[0] == 0:
			self.pack.parse_version_request(packet)
		elif packet[0] == 1:
			self.pack.parse_open_loop(packet)
		elif packet[0] == 2:
			self.pack.parse_close_loop(packet)
		elif packet[0] == 3:
			self.pack.parse_tone_packet(packet)
		elif packet[0] == 5:
			self.pack.parse_led_control(packet)
		elif packet[0] == 10:
			self.pack.parse_reset_request(packet)
		elif packet[0] == 0x6D:
			self.pack.parse_version_response(packet)
		elif packet[0] == 0x80:
			self.pack.parse_feedback(packet)
		
	def peek_packet(self):
		uartpacket = self.read_packet()
		if len(uartpacket) > 0:
			packet = self.pack.bin2pack(uartpacket)
			#print "raw: ",
			#self.dump(packet)
		
			self.pack.put_to_ring_buffer(packet)
			while self.pack.get_packet_from_ring_buffer():
				#self.dump(self.pack.onepacket)
				#self.dump(self.pack.onepacket[2:-2])
				self.parse_packet(self.pack.onepacket[2:-2])

	
	def test_esc(self,port):
		self.pack = EscPacket()
		self.open_uart(port)
		test_num = int(sys.argv[1])
		count = 0
		# for onespeed in [5000,9000,11000,15000,17000]:
		for onespeed in [test_num]:
		
			print "RPM: %d"%(onespeed)
			for loop in range(2500):
				speed = [onespeed,onespeed,onespeed,onespeed]
				packet = self.pack.pack_close_loop(count,speed,[6,6,6,6])
				count = count + 1
				uartpacket = self.pack.pack2bin(packet)
				self.send_packet(uartpacket)
				time.sleep(0.002)
				self.peek_packet()
		
		self.close_uart()
	
esc = Esc()
esc.test_esc(12) 


