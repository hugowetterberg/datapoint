<?php

function _data_point_admin_index_page() {
  return 'Index?';
}

function _data_point_admin_source_types() {
  return 'List?';
}

function _data_point_admin_add_source_type($state) {
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
    '#required' => TRUE,
    '#options' => $node_type_options,
    '#description' => t('This is the node type that will be used to publish data points'),
  );
  
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Add'),
  );
  
  return $form;
}

function _data_point_admin_add_source_type_configure($form_state, $parser, $content_type) {
  // Build the data point field options
  $dp_fields = datapoint_source_fields($parser);
  $dp_field_options = array_merge(array('' => t('-- Empty')));
  foreach ($dp_fields as $name => $def) {
    $dp_field_options[$name] = $def['description'];
  }
  
  $fields = datapoint_managed_fields($content_type);
  $form = array();
  
  foreach ($fields as $name => $finfo) {
    $form['field_source_' . $name] = array(
      '#type' => 'select',
      '#title' => $finfo['label'],
      '#required' => isset($finfo['required']) && $finfo['required'],
      '#options' => $dp_field_options,
      '#ahah' => array(
        'method' => 'replace',
        'wrapper' => 'field-source-' . $name . '-configuration',
        'path' => 'datapoint/field_source_configuration_ahah/' . $content_type . '/'  . $name,
      ),
    );
  }
  
  return $form;
}

function datapoint_ahah_field_source_configuration($content_type, $field) {
  $cached_form_state = array();
  $cached_form = form_get_cache($_POST['form_build_id'], $cached_form_state);
  
  $module = $_POST['source_type'];
  
  $cached_form['source_configuration'] = $set;
  form_set_cache($_POST['form_build_id'], $cached_form, $cached_form_state);
  $form_state = array('submitted' => FALSE);
  $set = form_builder('datapoint_source_configuration_ahah', $set, $form_state);
  $output = drupal_render($set);
  print drupal_to_js(array('status' => TRUE, 'data' => $output));
  exit;
}

function datapoint_datapoint_managed_fields_alter(&$fields, $type) {
  if ($type->has_title) {
    $fields['title'] = array(
      'label' => $type->title_label,
      'required' => TRUE,
      'setter' => '_datapoint_set_node_title',
    );
  }
  if ($type->has_body) {
    $fields['body'] = array(
      'label' => $type->body_label,
      'required' => $type->min_word_count>0,
      'setter' => '_datapoint_set_node_body',
    );
  }
}

function _datapoint_set_node_title(&$node, $value) {
  $node->title = $value;
}

function _datapoint_set_node_body(&$node, $value) {
  $node->body = $value;
}

function _data_point_admin_add_source_type_submit($form, $form_state) {
  $values = $form_state['values'];
  drupal_goto('admin/content/datapoint/add-source-type/' . $values['parser_type'] . '/' . $values['content_type']);
}

function _datapoint_source_type_configure($form_state, $module) {
  $info = call_user_func($module . '_datapoint');
  drupal_set_title(t('Configure !name', array('!name'=>$info['name'])));
  $node_type = variable_get('datapoint_' . $module . '_node_type', 0);
  
  //Build the node type options
  $node_types = node_get_types();
  $node_type_options = array(
    0 => t('-- Select a node type'),
  );
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
  
  $modules = module_implements('datapoint');
  $source_types = array(
    0 => t('-- Select a source type'),
  );
  foreach ($modules as $module) {
    $info = call_user_func($module . '_datapoint');
    $source_types[$module] = $info['name'];
  }
  
  $form['source_type'] = array(
    '#type' => 'select',
    '#title' => t('Source types'),
    '#options' => $source_types,
    '#default_value' => 0,
    '#ahah' => array(
      'method' => 'replace',
      'wrapper' => 'source-configuration',
      'path' => 'datapoint/source_configuration_ahah'
    ),
  );
  
  $form['source_configuration_placeholder'] = array(
    '#type' => 'markup',
    '#prefix' => '<div id="source-configuration">',
    '#suffix' => '</div>',
    '#value' => '<span></span>',
  );
  
  $form['custom_node_configuration'] = array(
    '#type' => 'checkbox',
    '#title' => t('Custom node configuration'),
    '#description' => t('Check this box if you want the nodes from this source to be created differently than for other sources of this type'),
    '#default_value' => FALSE,
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
  
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  
  return $form;
}

function datapoint_node_configuration_ahah() {
  $cached_form_state = array();
  $cached_form = form_get_cache($_POST['form_build_id'], $cached_form_state);
  
  $content_type = $_POST['content_type'];
  $module = $_POST['source_type'];
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
  else if (empty($module)) {
    $set = array(
      '#value' => '<div class="message notice message-notice">' . 
        t('Please select a source type before customizing node settings') . '</span>',
    );
  }
  else if (empty($content_type)) {
    $set = array(
      '#value' => '<div class="message notice message-notice">' . 
        t('Please select a content type before customizing node settings') . '</span>',
    );
  }
  else {
    $set = datapoint_node_configuration_form($module, array());
  }
  
  $cached_form['node_configuration'] = $set;
  form_set_cache($_POST['form_build_id'], $cached_form, $cached_form_state);
  $form_state = array('submitted' => FALSE);
  $set = form_builder('datapoint_node_configuration_ahah', $set, $form_state);
  $output = drupal_render($set);
  print drupal_to_js(array('status' => TRUE, 'data' => $output));
  exit;
}

function datapoint_node_configuration_form($module, $configuration) {
  // Build the field options
  $schema = datapoint_vertex_module_schema($module);
  $fields = $schema[$module . '_vertex']['fields'];
  
  $field_options = array(
    '__datapoint_name' => t('The name of the data point'),
    '__external_id' => t('The external id for the data point'),
  );
  foreach ($fields as $key => $column) {
    $field_options[$key] = $column['description'];
  }
  
  $form = array();
  
  $form['field_select'] = array(
    '#type' => 'select',
    '#options' => $field_options,
  );
  
  return $form;
}

function datapoint_source_configuration_form($module, $configuration) {
  $info = call_user_func($module . '_datapoint');
  $form = call_user_func($info['configuration'], array());
  
  return $form;
}

function datapoint_source_configuration_ahah() {
  $cached_form_state = array();
  $cached_form = form_get_cache($_POST['form_build_id'], $cached_form_state);
  
  $module = $_POST['source_type'];
  if ($module) {
    $set = datapoint_source_configuration_form($module, array());
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