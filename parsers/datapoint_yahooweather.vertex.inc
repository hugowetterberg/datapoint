<?php

function datapoint_yahooweather_vertex_schema() {
  return array(
    'description' => t('Weather forecasts from Yahoo Weather'),
    'fields' => array(
      'low' => array(
        'description' => t('The lowest temperature during the day.'),
        'type' => 'int',
        'unsigned' => FALSE,
        'not null' => TRUE,
        'default' => 0
      ),
      'high' => array(
        'description' => t('The highest temperature during the day.'),
        'type' => 'int',
        'unsigned' => FALSE,
        'not null' => TRUE,
        'default' => 0,
      ),
      'text' => array(
        'description' => t('The english description of the weather conditions.'),
        'type' => 'varchar',
        'length' => 64,
        'not null' => TRUE,
        'default' => '',
      ),
      'code' => array(
        'description' => t('The code for the weather conditions.'),
        'type' => 'int',
        'unsigned' => FALSE,
        'not null' => TRUE,
        'default' => 0,
      ),
    ),
  );
}