<?php

function datapoint_vertex_module_fields($module) {
  module_load_include('inc', $module, $module . '.vertex');

  return call_user_func($module . '_vertex_schema');
}

function datapoint_vertex_module_schema($module) {
  module_load_include('inc', $module, $module . '.vertex');

  $def = call_user_func($module . '_vertex_schema');

  // Add the 
  $fields = array(
    'vid' => array(
      'description' => t('The unique identifier for the vertex.'),
      'type' => 'serial',
      'unsigned' => TRUE,
      'not null' => TRUE,
    ),
    'pid' => array(
      'description' => t('The unique identifier for the data point.'),
      'type' => 'int',
      'unsigned' => TRUE,
      'not null' => TRUE,
      'default' => 0,
    ),
    'valid_from' => array(
      'description' => t('The start timestamp for the vertex.'),
      'type' => 'int',
      'unsigned' => TRUE,
      'not null' => TRUE,
      'default' => 0,
    ),
  );
  foreach ($def['fields'] as $name => $field) {
    if (!isset($fields[$name])) {
      $fields[$name] = $field;
    }
  }
  $def['fields'] = $fields;
  $def['primary key'] = array('vid');

  if(!isset($def['indexes'])) {
    $def['indexes'] = array();
  }

  $def['indexes']['pid'] = array('pid');
  $def['indexes']['valid_from'] = array('valid_from');

  return array(
    $module . '_vertex' => $def,
  );
}