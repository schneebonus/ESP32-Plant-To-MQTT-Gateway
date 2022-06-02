/*
Expected msg:
msg.topic: plants/devicename
msg.payload: {"t":int * 10,"h":int * 10,"b":int,"s":int}
*/

let plants = {
    "lilygo1" : "Minze",
    "lilygo2" : "Schnittlauch",
    "lilygo3" : "Oregano",
    "lilygo4" : "Thymian",
    };

plant_name = plants[(msg.topic.split("/"))[1]];
parsed = JSON.parse(msg.payload);

// format for influxdb
msg.payload = {
      bucket: 'plants',
    
      precision: 'ms',
    
      data: [
        {
          measurement: 'plant_health',
    
          tags: {
            deviceId: plant_name
          },
    
          fields: {
            temperature: +(parsed.t) / 10.0,
            humidity: +(parsed.h) / 10.0,
            water: +(parsed.s),
            battery: parsed.b > 100 ? 100 : parsed.b,
          },
    
          timestamp: Date.now()
        },
      ]
    };

return msg;
