//https://github.com/wollewald/SI1145_WE/blob/master/src/SI1145_WE.cpp
//add pause/stop and reset command implementation here
//intrrupt not working ok
//led are not bright
//only ps1 reading

//This is Si1142 sensor with 2 LED drivers only 
//below are not available
//PS_LED3 0x10
//PSLED3_SELECT 0x03
const i2cAddr = 0x5a;
var i2c = null;
var ledPin = null;
var deviceMeasureType = null;

function getReg(reg, len) {
    if (len == undefined) len = 1;
    i2c.writeTo(i2cAddr, reg);
    return i2c.readFrom(i2cAddr, len);
}

function setReg(reg, val) {
    i2c.writeTo(i2cAddr, [reg, val]);
}

function readParam(addr) {
    write_cmd((0x80 | addr));//PARAM_QUERY
    return getReg(0x2E, 1);//PARAM_RD
}

function writeParam(addr, val) {
    setReg(0x17, val);//PARAM_WR
    write_cmd(0xA0 | addr);//PARAM_SET
}

function write_cmd(cmd) {
    let val;
    val = getReg(0x20, 1);
    while (val != 0) {
        setReg(0x18, 0);
        val = getReg(0x20, 1);
    }
    do {
        setReg(0x18, cmd);
        if (cmd == 0x01) break;
        val = getReg(0x20, 1);
    } while (val == 0);
}
//https://siliconlabs.github.io/Gecko_SDK_Doc/efm32g/html/si114x__functions_8c_source.html#l00645

function uncompress(input) {
    let output = 0;
    let exponent = 0;

    if (input < 8) return 0;

    exponent = (input & 0xF0) >> 4;      // extracts the exponent
    output = 0x10 | (input & 0x0F);      // extracts the fraction and adds
    // in the implicit integer

    if (exponent >= 4) return (output << (exponent - 4));
    return (output >> (4 - exponent));
}

function compress(input) {
    let tmp = 0;
    let exponent = 0;
    let significand = 0;

    if (input == 0)
        return 0;

    if (input == 0x0000) return 0x00;
    if (input == 0x0001) return 0x08;

    exponent = 0;
    tmp = input;
    while (1) {
        tmp >>= 1;  // Shift until there is only the integer in the lease
        //  significant position
        exponent += 1;
        if (tmp == 1) {
            break;  // the integer bit has been found. Stop.
        }
    }


    if (exponent < 5) // shift left to align the significant and return the
    // result
    {
        significand = (input << (4 - exponent));
        return ((exponent << 4) | (significand & 0xF));
    }

    significand = input >> (exponent - 5);

    if (significand & 1) // Check if we need to round up
    {
        significand += 2;   // Increment the 4th fraction (in bit1 position)
        if (significand & 0x0040)         // Check for a carry
        {
            exponent += 1;                // A carry occurred. Increment the exponent
            significand >>= 1;            // shift the signficand right by one
        }
    }
    return ((exponent << 4) | ((significand >> 1) & 0xF));
}

function setRegBit(reg, bit, state) {
    let val = getReg(reg, 1);
    if (state == 0) {
        val = val & ~(0x01 << bit);
    } else {
        val = val | (0x01 << bit);
    }
    return setReg(reg, val);
}

const initSensor = () => {
    resetSensor();
    ////enableInterrupt(0x05);//PSALS_INT
    /* pause between measurements =  31.25 µs x rate */
    ////setMeasurementRate(10);//255
    /* set LED Current, 22.4 mA */
    setLEDCurrent(0);
    /* prox sensor 1 uses LED1 */
    writeParam(0x02, 0x21);//SI1145_PARA_PSLED12_SELECT =>  LED1 for PS1 and LED2 for PS2
    /* PS Photodiode Selection */
    selectPsDiode(0x03);//LARGE_DIODE
    /* Pulse Width Selection of IR LED pulse; pulse width = pulse width * 2^PS_ADC_Gain 
    No value > 5 shall be taken */
    setPsAdcGain(0);
    disableHighSignalPsRange();
    /* ALS-IR Photodiode Selection */
    selectIrDiode(0x00);//SMALL_DIODE
    /* Integration time for ALS-VIS measurements = integration time * 2^ALS_VIS_ADC_GAIN 
    No value > 7 (factor 128) shall be taken */
    setAlsVisAdcGain(0);
    disableHighSignalVisRange();
    /* Integration time for ALS-IR measurements = integration time * 2^ALS_IR_ADC_GAIN 
    No value > 7 (factor 128) shall be taken */
    setAlsIrAdcGain(0);
    disableHighSignalIrRange();
};

const resetSensor = () => {
    write_cmd(0x01);//reset

    setReg(0x07, 0x17);//enable sensor
    setReg(0x0F, 0x00);//led off/no power
    writeParam(0x01, 0x77);//CLHLIST enable all channel

    // interrupt settings
    setReg(0x03, 0);
    setReg(0x04, 0);
    setReg(0x05, 0);
    setReg(0x06, 0);
}

const enableMeasurements = (t) => {
    deviceMeasureType = t;
    let chListValue = 0;

    switch (t) {
        case 0b00000001://PS_TYPE
            chListValue = 0b00000011;
            break;
        case 0b00000010://ALS_TYPE
            chListValue = 0b00110000;
            break;
        case 0b00000011://PSALS_TYPE
            chListValue = 0b00110011;
            break;
    }
    writeParam(0x01, chListValue);//SI1145_PARA_CHLIST
}

const startMeasurement = ()=>{
    write_cmd(deviceMeasureType | 0b00001100);//AUTO
};
const pauseMeasurement = ()=>{
    write_cmd(deviceMeasureType | 0b00001000);//PAUSE
};

const startSingleMeasurement = () => {
    write_cmd(deviceMeasureType | 0b00000100);//FORCE
}

const enableInterrupt = (t) => {
    setReg(0x03, 1);//SI1145_REG_INT_CFG
    setReg(0x04, t);//SI1145_REG_IRQEN
}

const disableAllInterrupts = () => {
    setReg(0x03, 0);//SI1145_REG_INT_CFG
}

const setMeasurementRate = (dataRate) => {
    // 0 breaks math - so handle that case first
    if (dataRate == 0) {
        setReg(0x08, 0x00);//REG_MEAS_RATE
        setReg(0x09, 0x08);//ALS_RATE
        setReg(0x0A, 0x08);//PS_RATE
        _measRate = 0;
        return 0;
    }

    // Convert Hz to 31.25us ticks
    let rate = Math.floor(32000.0 / dataRate);
    // clamp it to allowed values
    if (rate < 0) rate = 0;
    if (rate > 65535) rate = 65535;

    // Write the MEAS_RATE registers
    setReg(0x08, compress(rate));//REG_MEAS_RATE1
    setReg(0x09, 0x08);//ALS_RATE
    setReg(0x0A, 0x08);//PS_RATE

    // Set PSALS_AUTO flag for autonomous mode
    write_cmd(0x0F)//REG_COMMAND CMD_PSALS_AUTO

    // Convert to Hz and store MEAS Rate
    _measRate = 32000.0 / rate;

    // Return MEAS Rate in Hz
    return _measRate;
}

const getDataRate = () => {
    // Read the current MEAS_RATE registers
    let rate = getReg(0x08, 1)[0];

    // Convert 31.25us ticks to Hz and set _measRate
    _measRate = 32000.0 / uncompress(rate);

    // Convert MEAS Rate to Hz
    return _measRate;
}

const setLEDCurrent = (curr) => {
    (curr == 0) ? ledPin.reset() : ledPin.set();
    setReg(0x0F, curr);//SI1145_REG_PSLED21
}

const selectPsDiode = (diode) => {
    writeParam(0x07, diode);//SI1145_PARA_PS1_ADCMUX
    writeParam(0x08, diode);//SI1145_PARA_PS2_ADCMUX
}

const selectIrDiode = (diode) => {
    writeParam(0x0E, diode);//SI1145_PARA_ALS_IR_ADCMUX
}

const enableHighResolutionPs = () => {
    writeParam(0x05, 0b0011000);//SI1145_PARA_PS_ENCODING ONLY PS1 and PS2
}

const disableHighResolutionPs = () => {
    writeParam(0x05, 0b0000000);//SI1145_PARA_PS_ENCODING
}

const enableHighResolutionVis = () => {
    let ALSEncodingStatus = readParam(0x06);//SI1145_PARA_ALS_ENCODING
    ALSEncodingStatus |= 0b00010000;
    writeParam(0x06, ALSEncodingStatus);//SI1145_PARA_ALS_ENCODING
}

const disableHighResolutionVis = () => {
    let ALSEncodingStatus = readParam(0x06);//SI1145_PARA_ALS_ENCODING
    ALSEncodingStatus &= 0b11101111;
    writeParam(0x06, 0b0000000);//SI1145_PARA_ALS_ENCODING
}

const enableHighResolutionIr = () => {
    let ALSEncodingStatus = readParam(0x06);//SI1145_PARA_ALS_ENCODING
    ALSEncodingStatus |= 0b00100000;
    writeParam(0x06, ALSEncodingStatus);//SI1145_PARA_ALS_ENCODING
}

const disableHighResolutionIr = () => {
    let ALSEncodingStatus = readParam(0x06);//SI1145_PARA_ALS_ENCODING
    ALSEncodingStatus &= 0b11011111;
    writeParam(0x06, ALSEncodingStatus);//SI1145_PARA_ALS_ENCODING
}

const setPsAdcGain = (psAdcGain) => {
    /* psAdcGain <= 5! */
    if (psAdcGain > 5) return;
    writeParam(0x0B, psAdcGain);//SI1145_PARA_PS_ADC_GAIN
    /* data sheet recommendation: psAdcRec = one's complement of psAdcGain */
    let psAdcRec = ((~psAdcGain) & 0b00000111) << 4;
    writeParam(0x0A, psAdcRec);//SI1145_PARA_PS_ADCOUNTER
}

const enableHighSignalPsRange = () => {
    writeParam(0x0C, 0x24); // SI1145_PARA_PS_ADC_MISC change of PS_ADC_MODE not implemented
}

const disableHighSignalPsRange = () => {
    writeParam(0x0C, 0x04); //SI1145_PARA_PS_ADC_MISC change of PS_ADC_MODE not implemented
}

const setAlsVisAdcGain = (alsVisAdcGain) => {
    /* alsVisAdcGain <= 7 */
    if (alsVisAdcGain > 7) return;
    writeParam(0x11, alsVisAdcGain);//SI1145_PARA_ALS_VIS_ADC_GAIN
    /* data sheet recommendation: visAdcRec = one's complement of alsVisAdcGain */
    visAdcRec = ((~alsVisAdcGain) & 0b00000111) << 4;
    writeParam(0x10, visAdcRec);//SI1145_PARA_ALS_VIS_ADC_COUNTER
}

const enableHighSignalVisRange = () => {
    writeParam(0x12, 0x20); //SI1145_PARA_ALS_VIS_ADC_MISC change of PS_ADC_MODE not implemented
}

const disableHighSignalVisRange = () => {
    writeParam(0x12, 0x00); //SI1145_PARA_ALS_VIS_ADC_MISC change of PS_ADC_MODE not implemented
}

const setAlsIrAdcGain = (alsIrAdcGain) => {
    /* irAdcGain <= 7 */
    if (alsIrAdcGain > 7) return;
    writeParam(0x1E, alsIrAdcGain);//SI1145_PARA_ALS_IR_ADC_GAIN
    /* data sheet recommendation: irAdcRec = one's complement of alsIrAdcGain */
    let irAdcRec = ((~alsIrAdcGain) & 0b00000111) << 4;
    writeParam(0x1D, irAdcRec);//SI1145_PARA_ALS_IR_ADC_COUNTER
}

const enableHighSignalIrRange = () => {
    writeParam(0x1F, 0x20);//SI1145_PARA_ALS_IR_ADC_MISC
}

const disableHighSignalIrRange = () => {
    writeParam(0x1F, 0x00);//SI1145_PARA_ALS_IR_ADC_MISC
}

const getAlsVisData = (asInt16) => {
    if(asInt16){
        return new Int16Array(getReg(0x22, 2).buffer)[0];//SI1145_REG_ALS_VIS_DATA
    }
    return getReg(0x22, 2);
}

const getAlsIrData = (asInt16) => {
    if(asInt16){
        return new Int16Array(getReg(0x24, 2).buffer)[0];//SI1145_REG_ALS_IR_DATA
    }
    return getReg(0x24, 2);
}

const getPs1Data = (asInt16) => {
    if(asInt16){
        return new Int16Array(getReg(0x26, 2).buffer)[0];//SI1145_REG_PS1_DATA
    }
    return getReg(0x26, 2);
}

const getPs2Data = (asInt16) => {
    if(asInt16){
        return new Int16Array(getReg(0x28, 2).buffer)[0];//SI1145_REG_PS_DATA
    }
    return getReg(0x28, 2);
}

const getFailureMode = () => {
    return getReg(0x20, 1);//SI1145_REG_RESPONSE
}

const clearFailure = () => {
    write_cmd(0b00000000);//SI1145_NOP
}

const clearAllInterrupts = () => {
    setReg(0x21, 0xFF);//SI1145_REG_IRQ_STAT
}

const clearAlsInterrupt = () => {
    setReg(0x21, 0x01);//SI1145_REG_IRQ_STAT
}

const clearPsInterrupt = () => {
    setReg(0x21, 0x0C);//SI1145_REG_IRQ_STAT PS1 PS2
}

const clearCmdInterrupt = () => {
    setReg(0x21, 0x20);//SI1145_REG_IRQ_STAT
}

const getInterruptStatus = () => {
    return getReg(0x21, 1);//SI1145_REG_IRQ_STAT
}
const getVersion = () => { return "Si1142v4" };

function init(cfg) {
    i2c = cfg.i2c;
    ledPin = cfg.LedPin;
    const sensor = {
        getVersion: getVersion,
        initSensor: initSensor,
        resetSensor: resetSensor,
        enableMeasurements: enableMeasurements,
        startMeasurement: startMeasurement,
        pauseMeasurement: pauseMeasurement,
        startSingleMeasurement: startSingleMeasurement,
        enableInterrupt: enableInterrupt,
        disableAllInterrupts: disableAllInterrupts,
        setMeasurementRate: setMeasurementRate,
        getDataRate: getDataRate,
        setLEDCurrent: setLEDCurrent,
        selectPsDiode: selectPsDiode,
        selectIrDiode: selectIrDiode,
        enableHighResolutionPs: enableHighResolutionPs,
        disableHighResolutionPs: disableHighResolutionPs,
        enableHighResolutionVis: enableHighResolutionVis,
        disableHighResolutionVis: disableHighResolutionVis,
        enableHighResolutionIr: enableHighResolutionIr,
        disableHighResolutionIr: disableHighResolutionIr,
        setPsAdcGain: setPsAdcGain,
        enableHighSignalPsRange: enableHighSignalPsRange,
        disableHighSignalPsRange: disableHighSignalPsRange,
        setAlsVisAdcGain: setAlsVisAdcGain,
        enableHighSignalVisRange: enableHighSignalVisRange,
        disableHighSignalVisRange: disableHighSignalVisRange,
        setAlsIrAdcGain: setAlsIrAdcGain,
        enableHighSignalIrRange: enableHighSignalIrRange,
        disableHighSignalIrRange: disableHighSignalIrRange,
        getAlsVisData: getAlsVisData,
        getAlsIrData: getAlsIrData,
        getPs1Data: getPs1Data,
        getPs2Data: getPs2Data,
        getFailureMode: getFailureMode,
        clearFailure: clearFailure,
        clearAllInterrupts: clearAllInterrupts,
        clearAlsInterrupt: clearAlsInterrupt,
        clearPsInterrupt: clearPsInterrupt,
        clearCmdInterrupt: clearCmdInterrupt,
        getInterruptStatus: getInterruptStatus
    };
    return sensor;
}

module.exports = init;