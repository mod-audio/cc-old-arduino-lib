import serial
import struct
import random
import time
import sys

class Mode:
    def __init__(self):
        self.relevant_properties = 0
        self.properties_values = 0

class Actuator:
    def __init__(self):
        self.label = ""
        self.id = 0
        self.num_modes = 0
        self.num_assignments = 0
        self.num_steps = 0


class Device:
    def __init__(self):
        self.uri = ""
        self.label = ""
        self.channel = 0
        self.num_actuators = 0


class Parameter:
    def __init__(self, label, unit, minimum, maximum, num_scalepoints):
        self.label = label
        self.unit = unit
        self.minimum = minimum
        self.maximum = maximum
        self.default = (random.random() * (maximum - minimum) + minimum)
        self.value = (random.random() * (maximum - minimum) + minimum)
        self.steps = 65
        self.num_scalepoints = num_scalepoints
        self.sp_dict = dict()
        for x in range(0, num_scalepoints):
            param_label = "param" + str(x)
            self.sp_dict[param_label] = (random.random() * (maximum - minimum) + minimum)

    def describe(self):
        ret = bytearray()
        ret.append(len(self.label))
        ret.extend(self.label)
        ret.extend(struct.pack('%sf' % 1, self.value))
        ret.extend(struct.pack('%sf' % 1, self.minimum))
        ret.extend(struct.pack('%sf' % 1, self.maximum))
        ret.extend(struct.pack('%sf' % 1, self.default))
        ret.extend(struct.pack('%sH' % 1, self.steps))
        ret.append(len(self.unit))
        ret.extend(self.unit)
        ret.extend(struct.pack('%sB' % 1, self.num_scalepoints))
        for x in range(0, self.num_scalepoints):
            param_label = "param" + str(x)
            ret.append(len(param_label))
            ret.extend(param_label)
            # ret.extend(str(self.sp_dict[param_label]))
            ret.extend(struct.pack('%sf' % 1,self.sp_dict[param_label]))
        return ret


class Host:
    def __init__(self, serial):
        self.serial = serial
        self.id = '\x00'
        self.state = 1;
        self.header = bytearray()
        self.header.append('\x80')
        self.header.append(self.id)
        self.param = Parameter("gain", "db", -3, 16, 3)

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
                self.connection_response(buf)
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

    def connection_response(self, buf):
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

    def descriptor_request(self):
        descriptor_msg = bytearray(self.header)
        descriptor_msg.append(2) #device descriptor request function
        descriptor_msg.append(0) #data size 1
        descriptor_msg.append(0) #data size 2
        self.send(descriptor_msg)

    def control_assignment(self, parameter):
        assign_msg = bytearray(self.header)
        assign_msg.append(3) #device descriptor request function
        msg = bytearray()
        msg.append(1) #act id
        msg.append(0) #param mode mask 1
        msg.append(0) #param mode mask 2
        msg.append(1) #assign id
        msg.append(1) #port mask
        msg.extend(parameter.describe())
        data_size = struct.pack('%sH' % 1, len(msg))
        assign_msg.extend(data_size)
        assign_msg.extend(msg)
        self.send(assign_msg)

    def data_request(self):
        descriptor_msg = bytearray(self.header)
        descriptor_msg.append(4) #data request function
        descriptor_msg.append(1) #data size 1
        descriptor_msg.append(0) #data size 2
        descriptor_msg.append(0) #seq
        self.send(descriptor_msg)

    def control_unassignment(self, parameter_id):
        unassignment_msg = bytearray(self.header)
        unassignment_msg.append(5) #data request function
        unassignment_msg.append(1) #data size 1
        unassignment_msg.append(0) #data size 2
        unassignment_msg.append(parameter_id) #seq
        self.send(unassignment_msg)


def print_help():
    print "Choose between the following actions:"
    print "\t0-Run Full Test."
    print "\t1-Receive Connection Request."
    print "\t2-Device Descriptor Request."
    print "\t3-Control Assignment."
    print "\t4-Data Request.(20)"
    print "\t5-Control Unassignment."
    print "\tq-Quit script."

ser = serial.Serial("/dev/pts/4", baudrate=500000, timeout=0.01)
# s = serial.Serial("/dev/ttyUSB0", baudrate=500000, timeout=0.01)

if ser:
    print "Serial port is open."
else:
    print "Couldn't stabilish a connection. Try again."


mod = Host(ser)

# print param.describe()

print_help()

running = True

while running:
    var = raw_input("Next Action:")
    try:
        if int(var) == 0:
            mod.run()
        elif int(var) == 1:
            mod.run()
        elif int(var) == 2:
            mod.descriptor_request()
        elif int(var) == 3:
            param = raw_input("Enter the assigned parameter attributes (using space between):\nLabel Unit Minimum Maximum number_of_scalepoints\n")
            param = param.split(" ")
            param = Parameter(param[0], param[1], int(param[2]), int(param[3]), int(param[4]))
            mod.control_assignment(param)
        elif int(var) == 4:
            for count in range(0,20):
                mod.data_request()
                time.sleep(0.2)
        # elif int(var) == 5:
            # mod.control_unassignment()
        else:
            print "%d is not a valid function.\n" % int(var)
            print_help()
    except ValueError:
        if var == 'q':
            running = False
            print "FaLoWsSs :)"
        else:
            print " '%s' is not a valid function, try again." % var
            print_help()
