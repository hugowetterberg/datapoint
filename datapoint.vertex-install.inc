<?php

function datapoint_vertex_module_schema($module) {
  require_once(drupal_get_path('module', $module) . '/' . $module . '.vertex.inc');

  $def = call_user_func($module . '_vertex_schema');

  $def['fields']['vid'] = array(
    'description' => t('The unique identifier for the vertex.'),
    'type' => 'serial',
    'unsigned' => TRUE,
    'not null' => TRUE,
  );
  $def['fields']['pid'] = array(
    'description' => t('The unique identifier for the data point.'),
    'type' => 'int',
    'unsigned' => TRUE,
    'not null' => TRUE,
    'default' => 0,
  );
  $def['fields']['valid_from'] = array(
    'description' => t('The start timestamp for the vertex.'),
    'type' => 'int',
    'unsigned' => TRUE,
    'not null' => TRUE,
    'default' => 0,
  );
  $def['primary key'] = array('vid');

  return array(
    $module . '_vertex' => $def,
  );
}