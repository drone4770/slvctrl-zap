#include <math.h>
#include <SerialCommands.h>
#include <ZapMe.h>

// Create a new collar using transmission pin 27 and a zero id
CH8803 collar = CH8803(27, 0);

const char* DEVICE_TYPE = "zap";
const int FM_VERSION = 10000; // 1.00.00
const int PROTOCOL_VERSION = 10000; // 1.00.00

int currentDuration = 1000;
int currentStrength = 10;
int currentVibration = 0;
int currentAudio = 0;
int currentShock = 0;

char serial_command_buffer[32];
SerialCommands serialCommands(&Serial, serial_command_buffer, sizeof(serial_command_buffer), "\n", " ");

void commandIntroduce(SerialCommands* sender);
void commandSetVibration(SerialCommands* sender);
void commandSetAudio(SerialCommands* sender);
void commandSetShock(SerialCommands* sender);
void commandSetDuration(SerialCommands* sender);
void commandSetStrength(SerialCommands* sender);
void commandGetVibration(SerialCommands* sender);
void commandGetAudio(SerialCommands* sender);
void commandGetShock(SerialCommands* sender);
void commandGetDuration(SerialCommands* sender);
void commandGetStrength(SerialCommands* sender);


void setup() {
    
    collar.setId(13, 37);

    Serial.begin(9600);

    // Add commands
    serialCommands.SetDefaultHandler(commandUnrecognized);
    serialCommands.AddCommand(new SerialCommand("introduce", commandIntroduce));
    serialCommands.AddCommand(new SerialCommand("status", commandStatus));
    serialCommands.AddCommand(new SerialCommand("attributes", commandAttributes));
    serialCommands.AddCommand(new SerialCommand("set-vibration", commandSetVibration));
    serialCommands.AddCommand(new SerialCommand("set-audio", commandSetAudio));
    serialCommands.AddCommand(new SerialCommand("set-shock", commandSetShock));
    serialCommands.AddCommand(new SerialCommand("get-vibration", commandGetVibration));
    serialCommands.AddCommand(new SerialCommand("get-audio", commandGetAudio));
    serialCommands.AddCommand(new SerialCommand("get-shock", commandGetShock));
    serialCommands.AddCommand(new SerialCommand("get-duration", commandGetDuration));
    serialCommands.AddCommand(new SerialCommand("set-duration", commandSetDuration));
    serialCommands.AddCommand(new SerialCommand("get-stength", commandGetStrength));
    serialCommands.AddCommand(new SerialCommand("set-strength", commandSetStrength));

    serialCommands.GetSerial()->write(0x07);

}

void loop() {
    serialCommands.ReadSerial();
    
}

void commandIntroduce(SerialCommands* sender) {
    serial_printf(sender->GetSerial(), "introduce;%s,%d,%d\n", DEVICE_TYPE, FM_VERSION, PROTOCOL_VERSION);
}

void commandStatus(SerialCommands* sender) {
    serial_printf(sender->GetSerial(), "status;duration:%d,strength:%d,vibration:%d,audio:%d,shock:%d\n", currentDuration, currentStrength, currentVibration, currentAudio, currentShock);
}

void commandAttributes(SerialCommands* sender) {
    serial_printf(sender->GetSerial(), "attributes;duration:rw[0-60000],strength:rw[0-100],vibration:rw[bool],audio:rw[bool],shock:rw[bool]\n");
}

void commandSetDuration(SerialCommands* sender) {
    char* durationStr = sender->Next();

    if (durationStr == NULL) {
        sender->GetSerial()->println("set-duration;;status:failed,reason:duration_param_missing\n");
        return;
    }
  
    currentDuration = atoi(durationStr);
  
    serial_printf(sender->GetSerial(), "set-duration;%d;status:successful\n", currentDuration);
}

void commandSetStrength(SerialCommands* sender) {
    char* percentageStr = sender->Next();

    if (percentageStr == NULL) {
        sender->GetSerial()->println("set-flow;;status:failed,reason:percentage_param_missing\n");
        return;
    }
  
    currentStrength = atoi(percentageStr);

    serial_printf(sender->GetSerial(), "set-flow;%d;status:successful\n", currentStrength);
}

void commandGetDuration(SerialCommands* sender) {
    serial_printf(sender->GetSerial(), "get-duration;%d\n", currentDuration);
}

void commandGetStrength(SerialCommands* sender) {
    serial_printf(sender->GetSerial(), "get-strength,%d\n", currentStrength);
}

void commandSetVibration(SerialCommands* sender) {
    char* stateStr = sender->Next();

    if (stateStr == NULL) {
        sender->GetSerial()->println("set-state;;status:failed,reason:state_param_missing\n");
        return;
    }
  
    int state = atoi(stateStr);
    if (state > 0) {
      currentVibration = 1;
      serial_printf(sender->GetSerial(), "set-vibration;%d;status:successful\n", currentVibration);
      collar.sendVibration(currentStrength, currentDuration);
      currentVibration = 0;
    } else {
      serial_printf(sender->GetSerial(), "set-vibration;%d;status:successful\n", currentVibration);
    }
    
}

void commandGetVibration(SerialCommands* sender) {
    serial_printf(sender->GetSerial(), "get-vibration;%d\n", currentVibration);
}

void commandSetAudio(SerialCommands* sender) {
    char* stateStr = sender->Next();

    if (stateStr == NULL) {
        sender->GetSerial()->println("set-state;;status:failed,reason:state_param_missing\n");
        return;
    }
  
    int state = atoi(stateStr);
    if (state > 0) {
      currentAudio = 1;
      serial_printf(sender->GetSerial(), "set-audio;%d;status:successful\n", currentAudio);
      collar.sendAudio(0, currentDuration);
      currentAudio = 0;
    } else {
      serial_printf(sender->GetSerial(), "set-audio;%d;status:successful\n", currentAudio);
    }
}

void commandGetAudio(SerialCommands* sender) {
    serial_printf(sender->GetSerial(), "get-audio;%d\n", currentAudio);
}

void commandSetShock(SerialCommands* sender) {
    char* stateStr = sender->Next();

    if (stateStr == NULL) {
        sender->GetSerial()->println("set-state;;status:failed,reason:state_param_missing\n");
        return;
    }
  
    int state = atoi(stateStr);
    if (state > 0) {
      currentShock = 1;
      serial_printf(sender->GetSerial(), "set-shock;%d;status:successful\n", currentShock);
      collar.sendShock(currentStrength, currentDuration);
      currentShock = 0;
    } else {
      serial_printf(sender->GetSerial(), "set-shock;%d;status:successful\n", currentShock);
    }
    
}

void commandGetShock(SerialCommands* sender) {
    serial_printf(sender->GetSerial(), "get-shock;%d\n", currentShock);
}

void commandUnrecognized(SerialCommands* sender, const char* cmd)
{
    serial_printf(sender->GetSerial(), "Unrecognized command [%s]\n", cmd);
}

