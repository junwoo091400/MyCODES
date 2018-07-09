// Daniel Shiffman
// http://codingtra.in
// http://patreon.com/codingtrain

// Subscriber Mapping Visualization
// https://youtu.be/Ae73YY_GAU8

//------------------------------------------

//2018-07-10 Junwoo HWANG mod.

//Must Run 'python server'. Because if you just execute html, loadTable() and loadJSON() functions doesn't work.
//COMMAND : python -m http.server
//1. URL that helped me (https://github.com/processing/p5.js/wiki/Local-server).
//2. Toggle Switch (http://imivory.tistory.com/15) & (https://www.w3schools.com/howto/howto_css_switch.asp)

//So things chagned 'Toggle Pulse option' + '2 more stats to see(per subscriber values).'

let youtubeData;
let countries;

const mappa = new Mappa('Leaflet');
let trainMap;
let canvas;
let dataSource;

let data = [];

let currentColor;

const options = {
  lat: 0,
  lng: 0,
  zoom: 1.5,
  style: "http://{s}.tile.osm.org/{z}/{x}/{y}.png"
}

function preload() {
  // youtubeData = loadTable('subscribers_geo.csv', 'header');
  youtubeData = loadTable('watch_time_geo.csv', 'header');//p5 function...
  //countries = loadJSON('https://github.com/CodingTrain/500k-Subscriber-Map/blob/master/countries.json');
  countries = loadJSON('countries.json');
}

function setup() {
  canvas = createCanvas(800, 600);
  trainMap = mappa.tileMap(options);
  trainMap.overlay(canvas);
  dataSource = select('#dataSource'); // get the DOM element from the HTML
  dataSource.changed(processData); // assign callback!! <When CHANGED>

  currentColor = color(255, 0, 200, 100); // default color 
  processData();
}

function draw() {
  clear();
  pulseEnable = select('#pulseEn').checked();//Checked function(originally Attribute...)
  //var check = $("input[type='checkbox']");
  /*check.click(function(){
    $("p").toggle();
  });*/
  const zoom = trainMap.zoom();
  let scl = pow(2, zoom);
  if(pulseEnable) {
    scl *= sin(frameCount * 0.05);
  }
  for (let country of data) {
    const pix = trainMap.latLngToPixel(country.lat, country.lon);
    fill(currentColor);
    ellipse(pix.x, pix.y, country.diameter * scl);
  }
}

function processData() {
  data = []; // always clear the array when picking a new type

  let type = dataSource.value(); //get from html.
  let isTypeSpecialCalculation = false;

  switch (type) {
    case 'subscribers':
      currentColor = color(64, 250, 200, 100);
      break;
    case 'views':
      currentColor = color(255, 0, 200, 100);
      break;
    case 'watch_time_minutes':
      currentColor = color(200, 0, 100, 100);
      break;

    case 'views_per_sub':
      currentColor = color(255, 0, 200, 100);
      isTypeSpecialCalculation = true;
      break;
    case 'watch_time_minutes_per_sub':
      currentColor = color(200, 0, 100, 100);
      isTypeSpecialCalculation = true;
      break;

  }

  let maxValue = 0; // changed to something more generic, as we no longer only work with subs
  let minValue = Infinity;

  for (let row of youtubeData.rows) {
    let isInvalidData = false;

    let country = row.get('country_id').toLowerCase();
    let latlon = countries[country];

    if (latlon) {
      let lat = latlon[0];
      let lon = latlon[1];
      let count = 0;

      if(!isTypeSpecialCalculation) {//plain old GET from list.
        count = Number(row.get(type));
        //console.log(count);
      }
      else {//special calc.
        switch(type) {
          case 'views_per_sub':
            subs = Number(row.get('subscribers'));//CAN be 0!!
            if(subs != 0) {
              count = Number(row.get('views')) / subs;
            }
            else {
              isInvalidData = true;//inVALID.
            }
            break;
          case 'watch_time_minutes_per_sub':
            subs = Number(row.get('subscribers'));//CAN be 0!!
            if(subs != 0) {
              count = Number(row.get('watch_time_minutes')) / subs;
            }
            else {
              isInvalidData = true;//inVALID.
            }
            break;
        }
      }

      if(!isInvalidData) {
        data.push({
        lat,
        lon,
        count
        });
        if (count > maxValue) {
          maxValue = count;
        }
        if (count < minValue) {
          minValue = count;
        }
      }//if data is VALID...

    }
  }

  let minD = sqrt(minValue);
  let maxD = sqrt(maxValue);
  //console.log(minValue,maxValue);//debug.

  for (let country of data) {
    country.diameter = map(sqrt(country.count), minD, maxD, 1, 20);
  }
}
