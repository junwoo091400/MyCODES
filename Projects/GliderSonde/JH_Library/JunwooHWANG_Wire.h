//1
boolean Sensor_Present_Check(byte address){ Wire.beginTransmission(address);return Wire.endTransmission()==0?true:false;}

//2
boolean WriteByte(byte address,byte reg,byte data){Wire.beginTransmission(address); Wire.write(reg); Wire.write(data); return Wire.endTransmission()?false:true;}

//3
byte ReadByte(byte address,byte reg){Wire.beginTransmission(address); Wire.write(reg); if(Wire.endTransmission(false))return 0xFF;//keep connection(read mode)
  Wire.requestFrom(address,1); while(!Wire.available()); return Wire.read();}

//4
