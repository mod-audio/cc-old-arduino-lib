def encode_cc(buf):
    return buf.replace('\x1B', '\x1B\x1B').replace('\xAA', '\x1B\x55')

def decode_cc(buf):
    return buf.replace('\x1B\x55', '\xAA').replace('\x1B\x1B', '\x1B')

# def send(msg,verbose=True):
#   if verbose:
#     to_send = 'chain "\xAA' + encode_hmi(encode_cc(msg[1:])) + '"\0'
#     print repr(to_send)
#     s.write(to_send)

def checksum(buffer):
    check = 0
    for c in buffer:
        check += ord(c)

    return (check&0xFF)


# def run():
#     print "Serial started!"
#     s = serial.Serial("/dev/ttyUSB0", 500000, timeout=0.05)


#     # s.write('ping\0'); s.readall()
#     s.write('\xAA\x80\x00\x01\x23\x00\x1Fhttp://portalmod.com/devices/button\x01\x01\x00\xEF')
#     time.sleep(1); print repr(decode_cc(decode_hmi(s.readall())))

#     s.write('\xAA\x80\x00\x02\x00\x00\x2C')
#     time.sleep(1); print repr(decode_cc(decode_hmi(s.readall())))

#     s.write('\xAA\x80\x00\x03\x20\x00\x01\x00\x00\x01\x00\x04Gain\x00\x00\x80\x3F\x00\x00\x00\x00\x00\x00\x80\x3F\x00\x00\x00\x00\x21\x00\x02dB\x00\x19')
#     time.sleep(1); print repr(decode_cc(decode_hmi(s.readall())))

#     # s.write('\xAA\x80\x00\x03\x20\x00\x02\x20\x20\x02\x20\x04Foot\x00\x00\x80\x3F\x00\x00\x00\x00\x00\x00\x80\x3F\x00\x00\x00\x00\x02\x00\x02dB\x00\x75')
#     # time.sleep(1); print repr(decode_cc(decode_hmi(s.readall())))
#     print ('-'*48)

    #r = request()
    #return r

# def request():
#     seq = 0
#     check = 0x2F
#     for i in range(128):
#         s.write('\xAA\x80\x00\x04\x01\x00%c%c' % (seq, check))
#         time.sleep(0.2)
#         #print repr(decode_cc(decode_hmi(s.readall())))
#         seq += 1
#         seq &= 0xFF
#         check += 1
#         check &= 0xFF

#     received = s.readall()
#     return received

import serial
import time
import sys

s = serial.Serial("/dev/pts/3", baudrate=500000, timeout=0.01)
# s = serial.Serial("/dev/ttyUSB0", baudrate=500000, timeout=0.01)

# States:
# 0 - waiting device connection message
# 1 - connected, sending device descriptor request.
# 2 - descriptor received, sending assignment.

host_id = 0;
device_id = 0x80;
state = 0; # Waiting device's connection message.

if s:
    print "Serial port is open."
else:
    print "Couldn't stabilish a connection. Try again."

def parse(buf):
    if len(buf) > 7:
        if buf[3] == 1:
            url_size = buf[6]
            url = buf[7:7+url_size]
            to_send = bytearray("\xAA")
            to_send.append(device_id)
            to_send.append(host_id)
            to_send.append(1)
            to_send.append(buf[4])
            to_send.append(buf[5])
            to_send.append(url_size)
            to_send.extend(url)
            to_send.extend(buf[7+url_size:10+url_size])
            to_send.append(checksum(str(to_send)))
            to_send[1:] = encode_cc(to_send[1:])
            s.write(to_send)


def init():
    while state == 0:
        if s.inWaiting():
            data = bytearray(s.readall())
            if data.find('\xaa') >= 0:
                data = data[data.find('\xaa'):]
                parse(data)




init()