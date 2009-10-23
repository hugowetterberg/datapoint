<?php
// $Id$

/**
 * @file
 *
 * Provide views data and handlers for the datapoint simplepie
 */

/**
 * Implementation of hook_views_data().
 */
function datapoint_views_data() {
  $data = array();

  // Datapoint table.

  $data['datapoint']['table']['group']  = t('Datapoint');

  $data['datapoint']['table']['base'] = array(
    'field' => 'pid',
    'title' => t('Datapoint'),
    'help' => t('Datapoints are content fetched from other sites.'),
  );

  $data['datapoint']['table']['join'] = array(
    'node' => array(
      'left_field' => 'nid',
      'field' => 'nid',
    ),
  );

  $data['datapoint']['nid'] = array(
    'title' => t('Node'),
    'help'  => t('The node created for the datapoint'),
    'relationship' => array(
      'label'      => t('Datapoint Node'),
      'base'       => 'node',
      'base field' => 'nid',
      'skip base'  => array('node', 'node_revisions'),
    ),
  );

  $data['datapoint']['sid'] = array(
    'title' => t('Source Id'),
    'help'  => t('The source of the datapoint'),
    'argument' => array(
      'handler' => 'views_handler_argument_number',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'relationship' => array(
      'label'      => t('Datapoint Source'),
      'base'       => 'datapoint_source',
      'base field' => 'sid',
    ),
  );

  $data['datapoint']['name'] = array(
    'title' => t('Name'),
    'help' => t('The name of the datapoint.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  $data['datapoint']['dirty'] = array(
    'title' => t('Dirty'),
    'help' => t('Whether the datapoint is awaiting an update or not.'),
    'field' => array(
      'handler' => 'views_handler_field_boolean',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_boolean_operator',
      'label' => t('Dirty'),
    ),
  );

  $data['datapoint']['data'] = array(
    'title' => t('Data'),
    'help' => t('The configuration data of the datapoint - will need to be styled.'),
    'field' => array(
      'handler' => 'views_handler_field_datapoint_data',
    ),
  );

  //Datapoint source table
  $data['datapoint_source']['table']['group']  = t('Datapoint');

  $data['datapoint_source']['table']['join'] = array(
    'datapoint' => array(
      'left_field' => 'sid',
      'field' => 'sid',
      'type' => 'INNER',
    ),
    'node' => array(
      'left_table' => 'datapoint',
      'left_field' => 'sid',
      'field' => 'sid',
      'type' => 'INNER',
    ),
  );

  $data['datapoint_source']['name'] = array(
    'title' => t('Source Name'),
    'help' => t('The name of the datapoint source.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  $data['datapoint_source']['source_type'] = array(
    'title' => t('Source Type'),
    'help' => t('The type of a datapoint source'),
    'filter' => array(
      'handler' => 'views_handler_filter_datapoint_source_type',
    ),
  );

  $modules = module_implements('datapoint_parser');
  foreach ($modules as $module) {
    // Make sure that the vertex module doesn't provide it's own views integration
    if (!is_callable($module . '_views_api')) {
      $data[$module . '_vertex'] = datapoint_automatic_views_integration($module);
    }

    //Add the vertex join to the datapoint table
    $data['datapoint']['table']['join'][$module . '_vertex'] = array(
      'left_field' => 'vid',
      'field' => 'vid',
    );

    drupal_alter('datapoint_vertex_views_integration', $mview, $module . '_vertex');
  }

  return $data;
}

function datapoint_automatic_views_integration($module) {
  module_load_include('inc', 'datapoint', 'datapoint.vertex-install');
  $schema = datapoint_vertex_module_fields($module);
  $info = datapoint_parser_info($module);

  $mview = array();
  $mview['table'] = array(
    'group' => 'Datapoint ' . $info['name'] . ' Vertex',
    'base' => array(
      'field' => 'vid',
      'title' => $info['name'] . ' vertex',
    ),
    'join' => array(
      'datapoint' => array(
        'left_field' => 'pid',
        'field' => 'pid',
      ),
    )
  );

  $mview['vid'] = array(
    'title' => 'Vertex id',
    'help' => 'The id of the vertex',
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
  );

  $mview['pid'] = array(
    'title' => 'Point id',
    'help' => 'The id of the point that the vertex belongs to',
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
  );

  $mview['valid_from'] = array(
    'title' => 'Valid from',
    'help' => 'The time the vertex is valid from',
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ),
  );

  foreach ($schema['fields'] as $name => $def) {
    $vfield = array(
      'title' => $name,
      'help' => $def['description'],
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
    );

    switch ($def['type']) {
      case 'int':
        $vfield['filter']['handler'] = 'views_handler_filter_numeric';
        $vfield['argument']['handler'] = 'views_handler_argument_numeric';
      break;
      case 'varchar':
      case 'char':
      case 'text':
        $vfield['filter']['handler'] = 'views_handler_filter_string';
        $vfield['argument']['handler'] = 'views_handler_argument_string';
      break;
      case 'datetime':
        $vfield['field']['handler'] = 'views_handler_field_date';
        $vfield['filter']['handler'] = 'views_handler_filter_date';
        $vfield['argument']['handler'] = 'views_handler_argument_date';
      break;
    }

    if (isset($def['#views'])) {
      datapoint_merge_views_field($vfield, $def['#views']);
    }

    $mview[$name] = $vfield;
  }

  return $mview;
}

function datapoint_merge_views_field(&$base_def, $extend) {
  foreach ($extend as $key => $value) {
    if (is_array($value) && is_array($base_def[$key])) {
      datapoint_merge_views_field($base_def[$key], $value);
    }
    else {
      if (is_numeric($key)) {
        $base_def[] = $value;
      }
      else {
        $base_def[$key] = $value;
      }
    }
  }
}

/**
 * Implementation of hook_views_handlers() to register all of the basic handlers
 * views uses.
 */
function datapoint_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'datapoint') . '/includes',
    ),
    'handlers' => array(
      // field
      'views_handler_field_datapoint_data' => array(
        'parent' => 'views_handler_field',
      ),
      // filters
      'views_handler_filter_datapoint_source_type' => array(
        'parent' => 'views_handler_filter_in_operator',
      ),
      // relationships
      'views_handler_relationship_datapoint_vertex' => array(
        'parent' => 'views_handler_relationship',
      ),
    ),
  );
}