<?php
// $Id$

/**
 * @file
 *
 * Provide views data and handlers for the datapoint simplepie
 */

/**
 * Implementation of hook_views_data()
 */
function datapoint_views_data() {
  $data = array();

  //Datapoint table

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
    'title' => t('Datapoint Source'),
    'help'  => t('The source of the datapoint'),
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

  return $data;
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