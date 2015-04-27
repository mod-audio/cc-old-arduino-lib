import serial
import time
import sys

class Host:
    def __init__(self, serial):
        self.serial = serial
        self.id = '\x00'
        self.state = 1;
        self.header = bytearray()
        self.header.append('\x80')
        self.header.append(self.id)

        # States:
        # 1 - waiting device connection message
        # 2 - connected, sending device descriptor request.
        # 3 - descriptor received, sending assignment.


    def encode_cc(self, buf):
        return buf.replace('\x1B', '\x1B\x1B').replace('\xAA', '\x1B\x55')

    def decode_cc(self, buf):
        return buf.replace('\x1B\x55', '\xAA').replace('\x1B\x1B', '\x1B')

    def checksum(self, buffer):
        check = 0
        for c in buffer:
            check += ord(c)

        return (check&0xFF)

    def send(self, msg,verbose=True):
        to_send = msg
        to_send.insert(0, '\xAA') #Adding sync byte
        to_send.append(self.checksum(str(to_send))) #Calculating checksum
        to_send[1:] = self.encode_cc(to_send[1:]) #encoding for CC
        self.serial.write(to_send) #sending message
        if verbose:
            print repr(to_send)

    def parse(self, buf):
        if len(buf) > 7:
            if buf[3] == 1:
                url_size = buf[6]
                url = buf[7:7+url_size]

                msg = bytearray(self.header)

                msg.append(1) #function
                msg.append(buf[4]) #data size 1
                msg.append(buf[5]) #data size 2
                msg.append(url_size)
                msg.extend(url)
                msg.extend(buf[7+url_size:10+url_size])

                self.send(msg)

                self.state = 2

            if buf[3] == 2:
                self.state = 3

            if buf[3] == 3:
                self.state = 4

    def run(self):
        if self.serial.inWaiting():
            data = bytearray(self.serial.readall())
            if data.find('\xaa') >= 0:
                data = data[data.find('\xaa'):]
                self.parse(self.decode_cc(data))

        if self.state == 2:
            time.sleep(1)
            descriptor_msg = bytearray(self.header)
            descriptor_msg.append(2) #device descriptor request function
            descriptor_msg.append(0) #data size 1
            descriptor_msg.append(0) #data size 2

            self.send(descriptor_msg)
            self.state = 3

        if self.state == 3:
            msg = host_header
            msg.append(3) #device descriptor request function
            msg.append(0) #data size 1
            msg.append(0) #data size 2

ser = serial.Serial("/dev/pts/3", baudrate=500000, timeout=0.01)
# s = serial.Serial("/dev/ttyUSB0", baudrate=500000, timeout=0.01)

if ser:
    print "Serial port is open."
else:
    print "Couldn't stabilish a connection. Try again."


mod = Host(ser)

while 1:
    mod.run()