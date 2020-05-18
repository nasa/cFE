import time
import os
import struct

from struct import *
from lib.plugin_manager import Plugin, ArgTypes
from lib.logger import logger as log

class ReqValidation(Plugin):
    def __init__(self):
        super().__init__()
        self.name = "ReqValidation"
        self.description = "CTF Example Plugin"
        self.command_map = {
             "InsertComment": (self.InsertComment, [ArgTypes.string] ), 
             "TO_DO": (self.TO_DO, [ArgTypes.string], ArgTypes.number ),              
             "ParseTxtFile": (self.ParseTxtFile, [ArgTypes.string], [ArgTypes.string], ArgTypes.number), 
             "InterpretEventLog": (self.InterpretEventLog, [ArgTypes.string] *2),                          
            # mid, cc, args, set_length=None, name="default"
        }
        self.verify_required_commands = ["TestVerifyCommand"]
        self.example_counter = 0

    def initialize(self):
        #print("Initialize runs before a script is executed")
        return True  
  
    #This command is used to insert comments into the test log      
    def InsertComment(self, Comment):
        log.info("TESTER COMMENT - {}".format(Comment))     
        return True   
 
    #This command is used as a placeholder to document steps still need to be done.
    #A user may set the ReturnErrorFlag to 1. This will cause the test to fail until
    #the 'TO_DO' plugin is removed.       
    def TO_DO(self, Comment, ReturnErrorFlag):
        log.info("TO DO - {}".format(Comment)) 
        if ( ReturnErrorFlag == 1 ):
            return False 
        else:
            return True           

    #This command is used to interpret the cFE Event Log (binary file created via the
    #CFE_EVS_WRITE_LOG_DATA_FILE_CC command) and output a human readable text file
    def InterpretEventLog(self, InputFile, OutputFile):
        BuildDir = str(os.path.dirname(os.path.dirname(os.path.dirname(os.getcwd()))))
        #Temporary - plan to remove hardcoding /build/exe/lx2
        InputFilePath = BuildDir + "/cfs_gw/build/exe/lx2" + InputFile
        OutputFilePath = BuildDir + "/cfs_gw/build/exe/lx2" + OutputFile
        
        my_file=open(InputFilePath, mode='rb')
        my_ParsedFile = open(OutputFilePath, mode="w",encoding="utf-8")

        eventData = my_file.read()        

        CFE_EVS_PacketID_t = 32
        CFE_MISSION_EVS_MAX_MESSAGE_LENGTH = 122
        CFE_EVS_LongEventTlm_Payload_t = CFE_EVS_PacketID_t + CFE_MISSION_EVS_MAX_MESSAGE_LENGTH + 2
        CFE_EVS_LongEventTlm_t = 16 + CFE_EVS_LongEventTlm_Payload_t

        for i in range(20):

            offset = i*CFE_EVS_LongEventTlm_t
   
            seconds = int.from_bytes(eventData[74+offset:78+offset], byteorder='little', signed=False)
            subsecs = int.from_bytes(eventData[78+offset:80+offset], byteorder='little', signed=False)
 
            appName = eventData[80+offset:100+offset].decode('utf-8','ignore')

            EventID = int.from_bytes(eventData[100+offset:102+offset], byteorder='little')
            EventType = int.from_bytes(eventData[102+offset:104+offset], byteorder='little')
  
            SpacecraftID = int.from_bytes(eventData[104+offset:108+offset], byteorder='little')
            ProcessorID = int.from_bytes(eventData[108+offset:112+offset], byteorder='little')

            eventText = eventData[112+offset:234+offset].decode('utf-8','ignore')

            appName = appName.split("\0")[0]
            eventText = eventText.split("\0")[0]
    
            appName_length = len(appName)
            addedPad = 20 - len(appName)
            appName = appName.ljust(addedPad + appName_length)
  
            EventType = str(EventType)
        
            if ( EventType == "1" ):
                EventType = "DEBUG      "
            elif ( EventType == "2" ):
                EventType = "INFORMATION"
            elif ( EventType == "3" ):
                EventType = "ERROR      "
            elif ( EventType == "4" ):
                EventType = "CRITICAL   "
            else:
                EventType = "Invalid Event Type" 

            years = seconds // 31556926
            months = seconds // 2629743
            days = seconds // 86400
            hours = seconds // 3600 % 24
            minutes = seconds // 60 % 60
            seconds = seconds % 60

            message = ("Entry-%.2i %.4i:%.3i:%.2i:%.2i:%.2i.%.2i %s  %.3i  %s - %s \n" % 
            (i+1, years, days, hours, minutes, seconds, subsecs, appName, EventID, EventType, eventText))

            my_ParsedFile.write(message)

        my_file.close()
        my_ParsedFile.close()        
                        
        return True

    #This function is used to search a text file for a given text string.  The SearchNonExistFlag is used to control if the 
    #function returns True or False in the event of a match.  If the flag is set to 1, it will return false/FAIL upon match.
    #If the flag is set to 0, it will return True/PASS upon match.
    def ParseTxtFile(self, FileName, SearchString, SearchNonExistFlag):
        BuildDir = str(os.path.dirname(os.path.dirname(os.path.dirname(os.getcwd()))))
        #Temporary - plan to remove hardcoding /build/exe/lx2        
        FilePath = BuildDir + "/cfs_gw/build/exe/lx2" + FileName   
        file = open(FilePath, 'r')
        lines = file.readlines()
        file.close()      
        myFoundCount = 0
        myReturnValue = False 
        for line in lines:
            line = line.strip()
            if line.find(SearchString) != -1:
                log.info("Found Count: {}".format(myFoundCount))
                log.info("String value Found - {} ".format(SearchString))
                myFoundCount = myFoundCount + 1
                myReturnValue = True 

        if SearchNonExistFlag == 1 and myFoundCount == 0:
            myReturnValue = True
        if SearchNonExistFlag == 1 and myFoundCount > 0:
            myReturnValue = False            

        return myReturnValue 


    def test_verify_command(self):
        log.info("Test Verify Executed")

        # Non-blocking verification code goes here...

        # Here, we intentionally wait for example_counter > 5
        # before allowing the verification to pass

        self.example_counter += 1
        if self.example_counter > 5:
            status = True
        else:
            status = False

        # Return status of verification
        return status

    def shutdown(self):
        log.info("Optional shutdown/cleanup implementation for plugin")
