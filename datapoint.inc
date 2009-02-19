<?php
// $Id$

class DatapointResults implements IteratorAggregate {
  private $source;
  private $points;

  function __construct($source) {
    $this->source = $source;
    $this->points = array();
  }

  public function point($identifier) {
    if (!isset($this->points[$identifier])) {
      $this->points[$identifier] = new Datapoint($identifier);
    }
    return $this->points[$identifier];
  }

  public function getIterator() {
    return new DatapointIterator($this->points);
  }

  public function flush() {
    datapoint_store($this->source, $this);
    foreach ($this->points as $extid => $point) {
      unset($this->points[$extid]);
    }
  }
}

class Datapoint implements IteratorAggregate {
  private $vertexes;
  private $externalId;
  private $fromTime;
  private $name;
  private $data;
  private $hasNodes = TRUE;

  function __construct($externalId) {
    $this->externalId = $externalId;
    $this->vertexes = array();
    $this->data = array();
    $this->fromTime = Null;
  }

  public function id() {
    return $this->externalId;
  }

  public function getName() {
    return $this->name;
  }

  public function setName($value) {
    $this->name = $value;
  }

  public function data($name, $value) {
    $this->data[$name] = $value;
  }

  public function addVertex($vertex) {
    // Keep track of
    if ($this->fromTime == Null) {
      $this->fromTime = $vertex->getTime();
    }
    else {
      $this->fromTime = min($vertex->getTime(), $this->fromTime);
    }

    $this->vertexes[] = $vertex;
  }

  public function getStartTime() {
    return $this->fromTime;
  }

  public static function xmlAttribute($node, $name, $ns=NULL) {
    if (!empty($ns)) {
      if ($node->hasAttributeNS($ns, $name)) {
        return $node->getAttributeNS($ns, $name);
      }
    }
    else {
      if ($node->hasAttribute($name)) {
        return $node->getAttribute($name);
      }
    }

    return NULL;
  }

  public function getIterator() {
    return new DatapointIterator($this->vertexes);
  }

  public function getData() {
    return $this->data;
  }

  public function setHasNodes($value = NULL) {
    if ($value !== NULL) {
      $this->hasNodes = $value;
    }
    return $this->hasNodes;
  }
}

class DatapointVertex {
  private $values;
  private $time;

  function __construct($time) {
    $this->time = $time;
  }

  public function getTime() {
    return $this->time;
  }

  public function value($name, $value) {
    $this->values[$name] = $value;
  }

  public function xmlAttributes($node, $mapping, $ns=NULL) {
    foreach ($mapping as $attribute => $value_name) {
      $av = Datapoint::xmlAttribute($node, $attribute, $ns);
      if ($av) {
        $this->value($value_name, $av);
      }
    }
  }

  public function getValues() {
    return $this->values;
  }
}

class DatapointIterator implements Iterator {
  private $var = array();

  public function __construct($array) {
    if (is_array($array)) {
      $this->var = $array;
    }
  }

  public function rewind() {
    reset($this->var);
  }

  public function current() {
    $var = current($this->var);
    return $var;
  }

  public function key() {
    $var = key($this->var);
    return $var;
  }

  public function next() {
    $var = next($this->var);
    return $var;
  }

  public function valid() {
    $var = $this->current() !== FALSE;
    return $var;
  }
}