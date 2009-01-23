<?php

class views_handler_field_yahoo_weather_temperature extends views_handler_field {
  function render($values) {
    return $values->{$this->field_alias} . '°C';
  }
}