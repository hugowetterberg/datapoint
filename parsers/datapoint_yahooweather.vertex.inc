<?php
// $Id$

/**
 * @file
 * Definition of the Yahoo Weather Datapoint Vertex
 */

function datapoint_yahooweather_vertex_schema() {
  return array(
    'description' => 'Weather forecasts from Yahoo Weather',
    'fields' => array(
      'low' => array(
        'description' => 'The lowest temperature during the day.',
        'type' => 'int',
        'unsigned' => FALSE,
        'not null' => TRUE,
        'default' => 0,
        '#views' => array(
          'title' => 'Lowest',
          'field' => array(
            'handler' => 'views_handler_field_yahoo_weather_temperature',
          ),
        ),
      ),
      'high' => array(
        'description' => 'The highest temperature during the day.',
        'type' => 'int',
        'unsigned' => FALSE,
        'not null' => TRUE,
        'default' => 0,
        '#views' => array(
          'title' => 'Highest',
          'field' => array(
            'handler' => 'views_handler_field_yahoo_weather_temperature',
          ),
        ),
      ),
      'text' => array(
        'description' => 'The english description of the weather conditions.',
        'type' => 'varchar',
        'length' => 64,
        'not null' => TRUE,
        'default' => '',
        '#views' => array(
          'title' => 'Weather description',
        ),
      ),
      'code' => array(
        'description' => 'The code for the weather conditions.',
        'type' => 'int',
        'unsigned' => FALSE,
        'not null' => TRUE,
        'default' => 0,
        '#views' => array(
          'title' => 'Weather code',
          'field' => array(
            'handler' => 'views_handler_field_yahoo_weather_code',
          ),
        ),
      ),
    ),
  );
}