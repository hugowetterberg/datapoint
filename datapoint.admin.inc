<?php

function _datapoint_admin_index_page() {
  return 'Index?';
}

function _datapoint_admin_source_types() {
  return 'List?';
}

function _datapoint_admin_add_source_type($state) {
  //Build the parser options
  $modules = module_implements('datapoint_parser');
  $parsers = array('' => t('-- Select a data parser'));
  foreach ($modules as $module) {
    $info = call_user_func($module . '_datapoint_parser');
    $parsers[$module] = $info['name'];
  }

  //Build the node type options
  $node_types = node_get_types();
  $node_type_options = array('' => t('-- Select a node type'));
  foreach($node_types as $type => $type_info) {
    $node_type_options[$type] = $type_info->name;
  }

  $form = array(
    '#type' => 'fieldset',
    '#title' => t('Add source type'),
  );

  $form['parser_type'] = array(
    '#type' => 'select',
    '#title' => t('Parser'),
    '#required' => TRUE,
    '#options' => $parsers,
    '#description' => t('This is the parser that will be used for the source'),
  );

  $form['content_type'] = array(
    '#type' => 'select',
    '#title' => t('Node type'),
    '#options' => $node_type_options,
    '#description' => t('Select a node type that will be used to publish data points'),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Add'),
  );

  return $form;
}

function _datapoint_admin_add_source_type_configure($form_state, $parser, $content_type=NULL) {
  $form = array(
    'parser' => array(
      '#type' => 'value',
      '#value' => $parser,
    ),
    'content_type' => array(
      '#type' => 'value',
      '#value' => $content_type,
    ),
  );

  $form['type'] = array(
    '#type' => 'textfield',
    '#title' => t('Type'),
    '#required' => TRUE,
    '#description' => t('The machine-readable name of this source type. This name must contain only lowercase letters, numbers, and underscores. This name must be unique.'),
  );

  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#required' => TRUE,
    '#description' => t('The human-readable name of this source type.'),
  );

  if ($content_type) {
    // Build the data point field options
    $dp_fields = datapoint_source_fields($parser);
    $dp_field_options = array_merge(array('' => t('-- Empty')));
    foreach ($dp_fields as $name => $def) {
      $dp_field_options[$name] = $def['description'];
    }

    $fields = datapoint_managed_fields($content_type);

    $form['field_mappings'] = array(
      '#type' => 'fieldset',
      '#title' => t('Field mappings'),
    );

    foreach ($fields as $name => $finfo) {
      $form['field_mappings']['field_source_' . $name] = array(
        '#type' => 'select',
        '#title' => $finfo['label'],
        '#required' => isset($finfo['required']) && $finfo['required'],
        '#options' => $dp_field_options,
        '#ahah' => array(
          'method' => 'replace',
          'wrapper' => 'field-source-' . $name . '-configuration-placeholder',
          'path' => 'datapoint/ahah/field-source-configuration/' . $parser . '/'  . $name,
        ),
      );

      $form['field_mappings']['field_source_' . $name . '_configuration_placeholder'] = array(
        '#type' => 'markup',
        '#prefix' => '<div id="field-source-' . $name . '-configuration-placeholder">',
        '#suffix' => '</div>',
        '#value' => '<span></span>',
      );
    }

    // Simulate a node form to get the taxonomy module to provide taxonomy fields
    $tax = array();
    $tax['type'] = array('#type' => 'value', '#value' => $content_type);
    $tax['#node'] = (object)array(
      'nid' => 0,
      'type' => $content_type,
    );
    taxonomy_form_alter($tax, array(), $content_type . '_node_form');
    $form['taxonomy'] = $tax['taxonomy'];
    $form['taxonomy']['#weight'] = 90;
    $form['taxonomy']['#description'] = t('Set the default categories and tags that should be used for nodes generated from sources of this type. This can be overridden on individual sources later.');
  }

  $form['submit'] = array(
    '#weight' => 99,
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

function _datapoint_admin_add_source_type_configure_validate($form, &$form_state) {
  $configuration = source_type_configuration_from_form_values($form_state['values']);
  if (!preg_match('!^[a-z0-9_]+$!', $configuration['type'])) {
    form_set_error('type', t('The machine-readable name must contain only lowercase letters, numbers, and underscores.'));
  }
  if (datapoint_source_type($configuration['type'])) {
    form_set_error('type', t('The machine-readable name %type is already taken.', array(
      '%type' => $configuration['type'])));
  }
}

function _datapoint_admin_add_source_type_configure_submit($form, $form_state) {
  $configuration = source_type_configuration_from_form_values($form_state['values']);
  datapoint_add_source_type($configuration);
  
  drupal_set_message(t('The datapoint source type "!name" has been added', array(
    '!name' => $configuration['name'],
  )));
  drupal_goto('admin/content/datapoint/source-types');
}

function source_type_configuration_from_form_values($values) {
  $parser = $values['parser'];
  $content_type = $values['content_type'];

  $configuration = array(
    'type' => $values['type'],
    'name' => $values['name'],
    'content_type' => $content_type,
    'parser' => $parser,
  );
  
  if ($content_type) {
    $configuration['fields'] = array(); 
    $configuration['taxonomy'] = taxonomy_preview_terms((object)array('taxonomy' => $values['taxonomy']));
    
    $fields = datapoint_managed_fields($content_type);
    $source_fields = datapoint_source_fields($parser);
    foreach ($fields as $name => $def) {
      if (!empty($values['field_source_' . $name])) {
        $source_name = $values['field_source_' . $name];
        $source = $source_fields[$source_name];

        $field_conf = array(
          'source' => $source_name,
        );

        if (!empty($source['configuration_form'])) {
          $field_conf['configuration'] = call_user_func($source['configuration_form'] . '_submit', $name, $values);
        }

        $configuration['fields'][$name] = $field_conf;
      }
      $dp_field_options[$name] = $def['description'];
    }
  }

  return $configuration;
}

function datapoint_ahah_field_source_configuration($parser, $field) {
  $cached_form_state = array();
  $cached_form = form_get_cache($_POST['form_build_id'], $cached_form_state);
  $dp_fields = datapoint_source_fields($parser);

  $source = $_POST['field_source_' . $field];
  if (empty($source) || empty($dp_fields[$source]['configuration_form'])) {
    $set = array(
      '#value' => '<span></span>',
    );
  }
  else {
    $set = call_user_func($dp_fields[$source]['configuration_form'], $cached_form_state, $field);
  }

  $cached_form['field_source_' . $field . '_configuration'] = $set;
  form_set_cache($_POST['form_build_id'], $cached_form, $cached_form_state);
  $form_state = array('submitted' => FALSE);
  $set = form_builder('datapoint_ahah_field_source_configuration', $set, $form_state);
  $output = drupal_render($set);
  print drupal_to_js(array('status' => TRUE, 'data' => $output));
  exit;
}

function _datapoint_admin_add_source_type_submit($form, $form_state) {
  $values = $form_state['values'];
  drupal_goto('admin/content/datapoint/source-types/add/' . $values['parser_type'] .
    (!empty($values['content_type'])?'/' . $values['content_type']:''));
}

function _datapoint_source_type_configure($form_state, $module) {
  $info = call_user_func($module . '_datapoint');
  drupal_set_title(t('Configure !name', array('!name'=>$info['name'])));
  $node_type = variable_get('datapoint_' . $module . '_node_type', 0);

  //Build the node type options
  $node_types = node_get_types();
  $node_type_options = array_merge(array(
    '' => t('-- Select a node type'),
  ), datapoint_source_types());
  foreach($node_types as $type => $type_info) {
    $node_type_options[$type] = $type_info->name;
  }

  $form = array();
  $form['source_type'] = array(
    '#type' => 'hidden',
    '#value' => $module,
  );

  $form['content_type'] = array(
    '#type' => 'select',
    '#title' => t('Node type'),
    '#options' => $node_type_options,
    '#description' => t('This is the node type that will be used to publish data points from !name', array(
      '!name'=>$info['name'])),
    '#ahah' => array(
      'method' => 'replace',
      'wrapper' => 'node-configuration',
      'path' => 'datapoint/node_configuration_ahah'
    ),
  );

  $form['node_configuration_placeholder'] = array(
    '#type' => 'markup',
    '#prefix' => '<div id="node-configuration">',
    '#suffix' => '</div>',
    '#value' => '<span></span>',
  );
  return $form;
}

function _datapoint_source_add() {
  $form = array();
  drupal_add_js('misc/autocomplete.js');

  $modules = module_implements('datapoint');
  $source_types = array_merge(array(
    0 => t('-- Select a source type'),
  ), datapoint_source_types());
  foreach ($modules as $module) {
    $info = call_user_func($module . '_datapoint');
    $source_types[$module] = $info['name'];
  }

  $form['source_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#default_value' => '',
    '#required' => TRUE,
  );

  $form['source_type'] = array(
    '#type' => 'select',
    '#title' => t('Source type'),
    '#options' => $source_types,
    '#default_value' => 0,
    '#ahah' => array(
      'method' => 'replace',
      'wrapper' => 'source-configuration',
      'path' => 'datapoint/ahah/source_configuration'
    ),
  );

  $form['source_configuration_placeholder'] = array(
    '#type' => 'markup',
    '#prefix' => '<div id="source-configuration">',
    '#suffix' => '</div>',
    '#value' => '<span></span>',
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#weight' => 99,
    '#value' => t('Save'),
  );

  return $form;
}

function source_configuration_from_form_values($values) {
  $configuration = array(
    'type' => $values['source_type'],
    'name' => $values['source_name'],
    'taxonomy' => taxonomy_preview_terms((object)$values),
  );

  $type = datapoint_source_type($values['source_type']);
  $configuration['parser'] = $type['parser'];

  $info = datapoint_parser_info($type['parser']);
  if (isset($info['source configuration submit'])) {
    $configuration['source configuration'] = call_user_func($info['source configuration submit'], $values);
  }
  return $configuration;
}

function _datapoint_source_add_submit($form, $form_state) {
  $values = $form_state['values'];
  $configuration = source_configuration_from_form_values($values);
  datapoint_add_source($configuration);
}

function datapoint_ahah_custom_node_configuration() {
  $cached_form_state = array();
  $cached_form = form_get_cache($_POST['form_build_id'], $cached_form_state);

  $type_name = $_POST['source_type'];
  if (isset($_POST['custom_node_configuration'])) {
    $custom = $_POST['custom_node_configuration'];
  }
  else {
    $custom = TRUE;
  }

  if (!$custom) {
    $set = array(
      '#value' => '<span></span>',
    );
  }
  else if (empty($type_name)) {
    $set = array(
      '#value' => '<div class="message notice message-notice">' .
        t('Please select a source type before customizing node settings') . '</span>',
    );
  }
  else {
    $type = datapoint_source_type($type_name);
    $set = datapoint_node_configuration_form($type, array());
  }

  $cached_form['node_configuration'] = $set;
  form_set_cache($_POST['form_build_id'], $cached_form, $cached_form_state);
  $form_state = array('submitted' => FALSE);
  $set = form_builder('datapoint_ahah_custom_node_configuration', $set, $form_state);
  $output = drupal_render($set);
  print drupal_to_js(array('status' => TRUE, 'data' => $output));
  exit;
}

function datapoint_source_configuration_form($type, $configuration) {
  $info = datapoint_parser_info($type['parser']);
  if (isset($info['source configuration'])) {
    $form = call_user_func($info['source configuration'], array());
  }
  else {
    $form = array();
  }

  // Simulate a node form to get the taxonomy module to alter our form
  $form['type'] = array('#type' => 'value', '#value' => $type['content_type']);
  $form['#node'] = (object)array(
    'nid' => 0,
    'type' => $type['content_type'],
    'taxonomy' => $type['taxonomy'],
  );
  taxonomy_form_alter($form, array(), $type['content_type'] . '_node_form');

  //Remove the attributes needed for taxonomy_form_alter
  unset($form['type']);
  unset($form['#node']);
  return $form;
}

function datapoint_ahah_source_configuration() {
  $cached_form_state = array();
  $cached_form = form_get_cache($_POST['form_build_id'], $cached_form_state);

  $type_name = $_POST['source_type'];
  if (!empty($type_name)) {
    $type = datapoint_source_type($type_name);
    $set = datapoint_source_configuration_form($type, array());
  }
  else {
    $set = array(
      '#value' => '<span></span>',
    );
  }

  $cached_form['source_configuration'] = $set;
  form_set_cache($_POST['form_build_id'], $cached_form, $cached_form_state);
  $form_state = array('submitted' => FALSE);
  $set = form_builder('datapoint_source_configuration_ahah', $set, $form_state);
  $output = drupal_render($set);
  print drupal_to_js(array('status' => TRUE, 'data' => $output));
  exit;
}

function datapoint_export_source_type($type) {
  $source_type = datapoint_source_type($type);
  if ($source_type) {
    return '<textarea>' . htmlspecialchars(var_export($source_type, true)) . '</textarea>';
  }
  else {
    drupal_not_found();
  }
}

function datapoint_export_source($sid) {
  $source = datapoint_source($sid);
  if ($source) {
    return '<textarea>' . htmlspecialchars(var_export($source, true)) . '</textarea>';
  }
  else {
    drupal_not_found();
  }
}

/**
 * Marks the source as pinged and gives it priority during the next update
 *
 * @return void
 * @author /bin/bash: niutil: command not found
 **/
function datapoint_ping($sid) {
  db_query("UPDATE {datapoint_source} SET pinged=1 WHERE sid=%d", $sid);
}