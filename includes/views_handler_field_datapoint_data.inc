<?php
// $Id$

/**
 * @file
 * Field handler to provide a representation of a serialized configuration.
 */

class views_handler_field_datapoint_data extends views_handler_field {
  function option_definition() {
    return array();
  }

  function options_form(&$form, &$form_state) {
    // Empty on purpose
  }

  function render($values) {
    $field = $values->{$this->field_alias};
    if (!empty($field)) {
      $conf = unserialize($field);
    }
    else {
      $conf = array();
    }
    return theme('datapoint_data', $conf);
  }
}