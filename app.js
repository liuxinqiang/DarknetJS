const addon = require("./build/Release/darknet");

const text = addon.detectInit();
console.log(text);

addon.detectImage("1.jpg");
addon.detectImage("2.jpg");
addon.detectImage("3.jpg");
addon.detectImage("4.jpg");