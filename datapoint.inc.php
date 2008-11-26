<?php

class DatapointResults {
  private $points;
  
  function DatapointResults() {
    $this->points = array();
  }
  
  public function addPoint($point) {
    $this->points[] = $point;
  }
}

class Datapoint {
  private $vertexes=array();
  private $externalId;
  private $fromTime;
  
  function Datapoint($externalId) {
    $this->externalId = $externalId;
    $this->vertexes = array();
    $this->fromTime = Null;
  }
  
  public function addVertex($time, $data) {
    // Keep track of
    if ($this->fromTime == Null) {
      $this->fromTime = $time;
    }
    else {
      $this->fromTime = min($time, $this->fromTime);
    }
    
    $this->vertextes[] = array(
      'time' => $time,
      'data' => $data,
    );
  }
}