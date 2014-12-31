#include "platecontrol.h"
#include "qtinclude.h"

PlateControl::PlateControl(int plateControlDAC,
                           int plateTriggerBit,
                           int platePolarityBit,
                           int headStageSelectBit0)
{
    // Select the DAC used to control the current or voltage source
    dacNumber = plateControlDAC;

    // Select the digital control lines
    plateBit = plateTriggerBit;
    polarityBit = platePolarityBit;
    headstageSelect0 = headStageSelectBit0;

    // Bitmasks
    plateMask =  1 << plateBit;
    polarityMask =  1 << polarityBit;
    headstageMask =  3 << headstageSelect0;

    // TTL array
    ttl = 0;

}

int PlateControl::selectHeadstage(int headstageNumber) {

    if (headstageNumber < 3 && headstageNumber >= 0) {
        selectedHeadstage = headstageNumber;
        ttl = ((ttl & ~headstageMask) | selectedHeadstage << headstageSelect0);

        return 0;
    }
    else {
        cerr << "Invalid headstage selection. Please choose a headstage between 0 and 3.";
        return -1;
    }

}

void PlateControl::setPolarity(bool polarity) {

    ttl = ((ttl & ~polarityMask) | polarity << polarityBit);

    if (polarity) {
        cout << "Plating polarity set to +." << endl;
    }
    else {
        cout << "Plating polarity set to -." << endl;
    }


}

int PlateControl::setPlateParameters(double currentuA, double durationSec) {

    // Set the polarity
    if (currentuA < 0) {
        setPolarity(false);
    }
    else {
        setPolarity(true);
    }

    // Set the DAC voltage
    currentuA = abs(currentuA);
    dacVoltage = 32767 + qFloor((32768/3.3) * currentuA/uAPerVolt);
    plateDurationUSec = durationSec * 1e6;

    if (dacVoltage > 65536) {
        dacVolt = 65536;
        cerr << "The requested plating current is too high. It has been set to the max value." << endl;
        return -1;
    }
    else {
        cout << "Plating current set to " << currentuA << " uA" << endl;
        return 0;
    }

}

void PlateControl::applyPlatingDelay(){
    usleep(plateDurationUSec);
}

void PlateControl::turnPlatingOn() {
    ttl = ((ttl & ~plateMask) | 0x0001 << plateBit);
}

void PlateControl::turnPlatingOff() {
    ttl = ((ttl & ~plateMask) | 0x0000 << plateBit);
}

// Get the current TTL state in the format required by the Rhythm driver
int* PlateControl::getTTLState() {


    // Turn the TTL int into an array
    int ttlState[16];
    for (int i=0; i < 16; i++) {
        ttlState[i] = (0x0001 & (ttl << i));
    }

    return ttlState;

}


