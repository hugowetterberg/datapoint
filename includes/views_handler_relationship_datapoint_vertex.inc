<?php
// $Id: views_handler_relationship.inc,v 1.1 2008/09/03 19:21:28 merlinofchaos Exp $
/**
 * @file
 * Views' datapoint vertex relationship handlers.
 */

class views_handler_relationship_datapoint_vertex extends views_handler_relationship {
  /**
   * Called to implement a relationship in a query.
   */
  function query() {
    parent::query();
    if (isset($this->definition['datapoint source type'])) {
      //TODO: Add some error handling
      $datapoint_source_alias = $this->query->ensure_table('datapoint_source');
      $this->query->add_where(0, "$datapoint_source_alias.source_type = '%s'", $this->definition['datapoint source type']);
    }
  }
}