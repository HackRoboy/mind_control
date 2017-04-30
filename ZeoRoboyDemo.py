#!/usr/bin/env python
# System Libraries
from math import sqrt, copysign, tanh
import time
from time import localtime,strftime, gmtime
import urllib
import socket
import threading

import rospy
from roboy_comm.srv import ShowEmotion

# Zeo Libraries
from ZeoRawData.BaseLink import BaseLink
from ZeoRawData.Utility import *

# User customizable variables
port = '/dev/ttyUSB0'
username = 'Roboy83'
password = 'hello'
ros_host = '10.183.113.58'

DEBUG = False        # Controls whether debug outputs are printed
#Filter Variables
noiseTop = 25        # A noise reading above this is regarded as too noisy
noiseBottom = 5      # A noise reading below this is regarded as no significant data
eyeMax = 0.1         # The positive peak of an eye movement must be above this threshold to register
eyeMin = -0.1        # The negative peak of an eye movement must be below this threshold to register
eyeDistMin = 25      # The two peaks must be at least this far apart to count
eyeDistMax = 128      # The two peaks must be less than this far apart to count
eyeDiff = 0.4       # The max and min must be this different or more



#service call ros
def send_emotion_left(left):
    rospy.wait_for_service('roboy_face/show_emotion');
    try:
	service = rospy.ServiceProxy('roboy_face/show_emotion', ShowEmotion);
        if(left):
            service('lookleft');
        else:
	    service('lookright');
    except rospy.ServiceException, e:
        print "Service call failed%s"%e







signalSeconds = 2.5  # The number of seconds within which both right and left eye movents must occur before signal is logged    

# Global variables
urlBuffer = []
startRightClock = 0
startLeftClock = 0

def simplifyArray(A):
    for i in range(len(A)):
        A[i] = int(A[i])
    return str(A).replace(' ','').replace('[','').replace(']','')

def sig(x):
    #Simulated Sigmoid function
    stp = 30 #steepness, how rapidly it goes from min to max
    shft = 2.3 #cutoff point
    return (tanh(stp*(x)-shft)+1)/2
    
def filter2hz(A):
    """
    Filters out all above about 2hz noise from a signal.
    """
    # Filter designed in matlab
    # Convolution math from http://www.phys.uu.nl/~haque/computing/WPark_recipes_in_python.html
    filter=[0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000,
    0.0000, 0.0000, -0.0002, -0.0004, -0.0009, -0.0018,
    -0.0030, -0.0044, -0.0055, -0.0056, -0.0034, 0.0022,
    0.0125, 0.0277, 0.0474, 0.0699, 0.0925, 0.1119, 0.1251,
    0.1298, 0.1251, 0.1119, 0.0925, 0.0699, 0.0474, 0.0277,
    0.0125, 0.0022, -0.0034, -0.0056, -0.0055, -0.0044,
    -0.0030, -0.0018, -0.0009, -0.0004, -0.0002, 0.0000,
    0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000]
    #convolution
    P = len(A)
    Q = len(filter)
    N = P + Q - 1
    c = []
    for k in range(N):
        t = 0
        lower = max(0, k-(Q-1))
        upper = min(P-1, k)
        for i in range(lower, upper+1):
            t = t + A[i] * filter[k-i]
        c.append(t)
    return c

class sleepStreamConnection(threading.Thread):
    """Constantly tries to upload the waveform data"""
    def run (self):
        global urlBuffer
        
        cleared = False
        while not cleared:
            try:
                urllib.urlopen('http://www.sleepstreamonline.com/scripts/clear.php',urllib.urlencode({'user' : username, 'password' : password}))
                cleared = True
            except:
                pass#probably a timeout, continue trying
        
        while True:
            if len(urlBuffer)==0:
                # No data, No rush, take a break
                time.sleep(0.25)
            try:
                while len(urlBuffer)>0 :
                    #urllib.urlopen('http://www.sleepstreamonline.com/scripts/update.php', urlBuffer[0])
                    urllib.urlopen('http://www.schlafhacking.de/sleepstream/update.php', urlBuffer[0])
                    urlBuffer = urlBuffer[1:]
                    
            except:
                pass#probably a timeout, continue trying

class onlineParser:
    """
    Interprets the incoming Zeo data, encapsulates it, and sends it to sleepstream.
    """        
    
    def clearSlice(self):
          self.Slice = {'user'     : username,
                        'password'   : password,
                        'time'       : strftime("%Y-%m-%d %H:%M:%S",localtime()),
                        'waveform'   : "",
                        'frequency'  : "",
                        'sleepstage' : self.Slice['sleepstage'],
                        'eye'        : 0,
                        'event'      : "",
                        'log'        : "" 
                        }
             
    def __init__(self):
        """Creates a new parser object."""
        # Create and clear object fields
        self.Slice = {'user'     : username,
                      'password'   : password,
                      'time'       : strftime("%Y-%m-%d %H:%M:%S",localtime()),
                      'waveform'   : "",
                      'frequency'  : "",
                      'sleepstage' : -1,
                      'eye'        : 0,
                      'event'      : "",
                      'log'        : "" 
                      }
        self.WaveBuffer = [0]*128
        self.EyeBuffer = [0]*192
        self.uploader = sleepStreamConnection().start()
        self.noise = False

    def update(self, timestamp, timestamp_subsec, version, data):
        """
        Update the current Slice with new data from Zeo.
        This function is setup to be easily added to the 
        BaseLink's callbacks.
        """
        global username
        global password
        global urlBuffer
        global startRightClock
        global startLeftClock
        
        if version != 3:
            print 'Unsupport raw data output version: %i' % version
            return
            
        datatype = dataTypes[ord(data[0])]
        
        if datatype == 'Event':
            e = getUInt32(data[1:5])
            if eventTypes.has_key(e):
                self.Slice['event'] = eventTypes[e]
            
        elif datatype == 'SliceEnd':
            urlBuffer.append(urllib.urlencode(self.Slice))
            self.clearSlice()
            
        elif datatype == 'Waveform':
            wave = []
            #rwave = [] 
            for i in range(1,256,2):
                value = getInt16(data[i:i+2])# Raw value
                #rwave.append(value)
                value = float(value*315)/0x8000    # convert to uV value FIX
                wave.append(value)
            
            #print wave
            filtered60 = filter60hz(self.WaveBuffer + wave)[90:218]
            
            #Eye movement Processing
            sm=0
            for i in filtered60:
                sm+=abs(i)
            avg = sm/128
            if DEBUG: 
                print 'Noise Level: {0}'.format(avg)
            if avg<noiseTop:#should get rid of clipping as well as really noisy signals
                if not self.noise and avg>noiseBottom: 
                #this gives a 1second buffer to avoid ringing; also helps stop overamplifying small signals
                    waveform  = filter2hz(self.WaveBuffer + wave)[90:218]
                    for i in range(len(waveform)):
                        if abs(int(waveform[i]))>0:
                            waveform[i]=copysign(sig(abs(waveform[i]/350)),waveform[i])
                        else:
                            waveform[i]=0
                    self.EyeBuffer = self.EyeBuffer[128:] + waveform
                    mx = max(self.EyeBuffer)
                    mn = min(self.EyeBuffer)
                    mxi = self.EyeBuffer.index(mx)
                    mni = self.EyeBuffer.index(mn)
                    mDist = abs(mxi-mni)
                    if DEBUG: 
                        print 'Max Peak: {0}'.format(mx)
                        print 'Min Peak: {0}'.format(mn)
                        print 'Peak Distance: {0}'.format(mDist)
                    #if mx>eyeMax and mn<eyeMin and mDist>eyeDistMin and mDist<eyeDistMax:
                    if mx+abs(mn)>eyeDiff and mx>eyeMax and mn<eyeMin and mDist>eyeDistMin and mDist<eyeDistMax:   
                        if mxi>mni:
                            self.Slice['eye']=1
                            self.EyeBuffer=[0]*192
                            print '**EYE RIGHT**'
                            send_emotion_left(False);
                            startLeftClock = 0
                            print startRightClock, time.time()
                            if time.time() - startRightClock < signalSeconds:
                                self.Slice['log'] = 'eyeSignal'
                                print 'Eye Signal Sent'
                                startRightClock = 0
                                startLeftClock = 0
                            else:
                                startRightClock = time.time()
                        elif mxi<mni:
                            self.Slice['eye']=-1
                            self.EyeBuffer=[0]*192
                            print '**EYE LEFT**'
                            send_emotion_left(True);
                            startRightClock = 0
                            print startLeftClock, time.time()
                            if time.time() - startLeftClock < signalSeconds:
                                self.Slice['log'] = 'eyeSignal'
                                print 'Eye Signal Sent'
                                startLeftClock = 0
                                startRightClock = 0
                            else:
                                startLeftClock = time.time()
                else:
                    self.noise=False
            else:
                self.noise=True
            
            self.Slice['waveform'] = simplifyArray(filtered60) # grab the valid middle as the current second
            self.Slice['rwaveform'] = wave
            self.WaveBuffer = wave # store this second for processing the next second
            
            # NOTE: it is possible to mess this up easily for the first second.
            # A second could be stored, headband docked, then undocked and it would 
            # use the old data as the previous second. This is considered ok since it 
            # will only be bad for the first portion of the first second of data.
            
        elif datatype == 'FrequencyBins':
            A = []
            for bin in range(7):
                A.append((float(getUInt16(data[(bin*2+1):(bin*2+3)]))/0x8000)*100)
            self.Slice['frequency'] = simplifyArray(A)
            
        elif datatype == 'SleepStage':
            self.Slice['sleepstage'] = getUInt32(data[1:])
            
if __name__ == "__main__":

    # Set transmission timeout
    socket.setdefaulttimeout(1.5)
    
    # Initialize Serial Link
    link = BaseLink(port)
    parser = onlineParser()

    # Add Callbacks and Start the Link
    link.addCallback(parser.update)
    link.start()
    
    # Infinitely loop to allow the script to run continuously
    while(True):
        time.sleep(5)

