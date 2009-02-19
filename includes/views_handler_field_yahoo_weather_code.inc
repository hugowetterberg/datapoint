<?php
// $Id$

class views_handler_field_yahoo_weather_code extends views_handler_field {
  function render($values) {
    $graphics_path = drupal_get_path('module', 'datapoint_yahooweather') . '/images';
    $graphics = array(
      'thunder' => $graphics_path . '/weather-thunder.png',
      'snow-rain' => $graphics_path . '/weather-snow-rain.png',
      'rain' => $graphics_path . '/weather-rain.png',
      'snow' => $graphics_path . '/weather-snow.png',
      'sun' => $graphics_path . '/weather-sun.png',
      'sun-cloud' => $graphics_path . '/weather-sun-cloud.png',
      'cloud' => $graphics_path . '/weather-cloud.png',
    );
    drupal_alter('yahoo_weather_graphics', $graphics);
    
    $mappings = array(
      0 => "thunder",   10 => "rain",      20 => "sun-cloud", 30 => "sun-cloud", 41 => "snow",
      1 => "thunder",   11 => "rain",      21 => "sun-cloud", 31 => "sun",       42 => "snow",
      2 => "thunder",   12 => "rain",      22 => "sun-cloud", 32 => "sun",       43 => "snow",
      3 => "thunder",   13 => "snow",      23 => "sun-cloud", 33 => "sun",       44 => "sun-cloud",
      4 => "thunder",   14 => "snow",      24 => "sun-cloud", 34 => "sun",       45 => "thunder",
      5 => "snow-rain", 15 => "snow",      25 => "sun-cloud", 35 => "snow-rain", 46 => "snow",
      6 => "snow-rain", 16 => "snow",      26 => "cloud",     36 => "sun",       47 => "thunder",
      7 => "snow-rain", 17 => "snow",      27 => "sun-cloud", 37 => "thunder",   3200 => "sun-cloud",
      8 => "rain",      18 => "snow-rain", 28 => "sun-cloud", 38 => "thunder",   
      9 => "rain",      19 => "sun",       29 => "sun-cloud", 39 => "thunder",   
    );
    drupal_alter('yahoo_weather_graphics_mapping', $graphics);
    
    $code = $values->{$this->field_alias};
    $path = $graphics[$mappings[$code]];
    $title = $mappings[$code];
    if (isset($values->datapoint_yahooweather_vertex_text)) {
      $title = t($values->datapoint_yahooweather_vertex_text);
    }
    return theme('image', $path, $title, $title);
  }
}