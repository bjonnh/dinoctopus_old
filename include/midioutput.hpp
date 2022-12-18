//
// Created by bjo on 12/17/22.
//

#ifndef CODE_MIDIOUTPUT_HPP
#define CODE_MIDIOUTPUT_HPP

#include <MIDI.h>
using namespace midi;
#include "controlleroutput.hpp"

class MidiOutput: public ControllerOutput {
private:
    MidiInterface<SerialMIDI<HardwareSerial>> *midiInterface = nullptr;
public:
    explicit MidiOutput(MidiInterface<SerialMIDI<HardwareSerial>> *midiInterface = nullptr, byte controller = 1, byte channel = 16)
            : midiInterface(midiInterface), controller(controller), channel(channel) {}

    void update_value(byte value) {
        if (midiInterface != nullptr)
            midiInterface->sendControlChange(controller,
                               value,
                               channel);
    }

    void set_channel(byte i) {
        if (i<=16)
            channel = i;
    }

    void set_controller(byte i) {
        controller = i;
    }

    byte controller;
    byte channel;
};


#endif //CODE_MIDIOUTPUT_HPP
