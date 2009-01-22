<?php
// $Id: views_handler_filter_node_type.inc,v 1.1 2008/09/03 19:21:29 merlinofchaos Exp $
/**
 * Filter by node type
 */
class views_handler_filter_datapoint_source_type extends views_handler_filter_in_operator {
  function get_value_options() {
    if (!isset($this->value_options)) {
      $this->value_title = t('Source type');
      $types = datapoint_source_types();
      foreach ($types as $type => $name) {
        $options[$type] = $name;
      }
      $this->value_options = $options;
    }
  }
}