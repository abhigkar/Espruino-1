const i2cAddress = 0x44;
var i2c = null;
var rdyPin;
var dbg = true;
var toc=100;
var sleeping = false;
var irqHandleId = 0;
var prevEvt;
var cbHand;
var longTapTO;

const read = (reg, len, stop)=>{
    let c = toc;  while(digitalRead(rdyPin) && c--);if(c==0) return false;
    i2c.writeTo({address:i2cAddress, stop:false}, reg);
    if(stop)
        return i2c.readFrom(i2cAddress,len);
    else
        return i2c.readFrom({address:i2cAddress, stop:false},len);
};
const write = (data)=>{
  let c = toc;  while(digitalRead(rdyPin) && c--);if(c==0) return false;
  i2c.writeTo(i2cAddress, data); return true;
};

const setDebugMode = (mode)=> dbg = mode;

const redoATI = ()=>{  write([0x09,0x10]); };

function setPollMode(){  return write([0x09, 0x0, 0x15]); }

const initSensor = ()=>{
    write([0x01, 0x00]);  //set in projection mode**
    write([0x09,0x00,0x15,0x00,0x00,0xc6]);//ProxSettings
    write([0x0D,0x0f]);//Active Channels
    write([0x0A,0x10,0x15,0x15,0x10,0x08,0x00,0x14,0x04]);//Thresholds
    write([0x0B,0x02,0x40,0x80]);//Timings & Targets
    write([0x0C,0x0A,0x14,0x38]);//Gesture Timers//Gesture Timers
    write([0x09,0x10]);//REDO ATI
    var timeoutCount=10;
    var buff;
    do {
        buff = read(0x09,1);
    }
    while ((buff[0] & 0b00000100) && (timeoutCount-- > 0) );

    if (buff[0] & 0b00000100)
    {
       if(dbg) print("ATI Timed out");
    }
    write([0x09,0x00,0x55,0x00,0x00,0xc6]);
};

const dbgReadRegisters = ()=>{
    console.log(read(0x01,2)); //System Flags
    console.log(read(0x09,5)); //ProxSettings
    console.log(read(0x0D,1)); //Active Channels
    console.log(read(0x0A,8)); //Thresholds
    console.log(read(0x0B,3)); //Timings & Targets
    console.log(read(0x0C,3)); //Gesture Timers
};

function fc(){
    var promise = new Promise(function(resolve, reject) {
        rdyPin.mode("output");
        digitalWrite(rdyPin,0);
        setTimeout(()=>{
            rdyPin.mode("input_pullup");
            setTimeout(function() {
                resolve('done!');
              },10);
        },10);
    });
    return promise;
}
function sleep(){
  write([0x01, 0x00]);  //set in projection mode**
  write([0x09,0x00,0x92,0x10,0x00,0x02]);
  write([0x0A,0xff,0xff,0xff,0xff,0xff,0x00,0x14,0x04]);
  write([0x0B,0x10,0x30,0x40]);
  write([0x09,0x10,0xd2,0x10,0x00,0x00]);
}

const sleepDevice = () =>{
  clearWatch(irqHandleId);
  irqHandleId = -1;
  if(setPollMode()){
      sleep();
      sleeping = true;
  }
};
const wakeDevice = () =>{
  fc().then(()=>{
    setPollMode();
    start();
    sleeping = false;
  });
};

const readEvents = ()=>{
     let eventData = {flick:'NONE',tap:'NONE',cords:0};
     buf = read(0x01,2,false);// events
     if(buf[1] ==0 || buf[1] ==1 || buf[0] == 255 || buf[1] == 255) return false;
     buf2 = read(0x03,1,false);// touch
     buf3 = read(0x02,1,true); //coordinates
     //redoATI and POR init
     if(buf[0] & 0x80){
        if(dbg) console.log("POR Detected, calling init");
        initSensor();
        return false;
     }
     if(buf[0] & 0x32){
        if(dbg) console.log("ATI Error, calling redo ATI");
        redoATI();
        return false;
     }
     if(longTapTO>0){
       clearTimeout(longTapTO);
       longTapTO = 0;
     }
     if(dbg) console.log(buf, buf2, buf3);
     if(buf[1] &  0x80) {
       eventData.flick = "DOWN";//SLIDE DOWN
     }
     else if(buf[1] &  0x40) {
        eventData.flick = "UP"; //SLIDE UP
     }
     else if(buf2[0]  == 1 ) {// prox , event end
        if(prevEvt){
          switch(prevEvt.touch){
             case 2:
               eventData.tap = "DOWN";
               eventData.cords = prevEvt.slide;
               break;
             case 4:
               eventData.tap = "UP";
               eventData.cords = prevEvt.slide;
               break;
             case 6:
               eventData.tap = "MIDDLE";
               eventData.cords = prevEvt.slide;
               break;
             case 8:
               eventData.tap = "HOME";
               break;
          }
        }
     }
     else{
        prevEvt = {sys:buf[1],touch:buf2[0] & 0x0E,slide:buf3[0]};
       if(prevEvt.touch == 8){
         longTapTO = setTimeout(()=>{
          if(dbg) console.log("LONG HOME");
         },2000);
       }
     }
     return eventData;
};

const start =()=>{
    irqHandleId = setWatch(()=>{
       let e =  readEvents();
       if(e && cbHand) cbHand(e);
    }, rdyPin, {repeat: true, edge: 'falling', debounce:0 });
};
const setTouchCallback = (cb)=>{
    cbHand = cb;
};
function init(cfg){
  if(dbg) console.log("V2");
    i2c = cfg.i2c;
    sensorMode = cfg.mode || 0;
    rdyPin = cfg.rdyPin;
    prevEvt = null;
    const sensor = {
        dbgReadRegisters: dbgReadRegisters,
        start:start,
        setDebugMode:setDebugMode,
        setTouchCallback: setTouchCallback
    };
    return sensor;
}

module.exports = init;