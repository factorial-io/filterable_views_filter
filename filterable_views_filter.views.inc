<?php
/**
 * @file
 * Adds Views3 support.
 */

/**
 * Implements hook_views_plugins().
 */
function filterable_views_filter_views_plugins() {
  return array(
    'exposed_form' => array(
      'filterable_views_filter' => array(
        'title' => t('Filters for exposed filter'),
        'help' => t('Adds an additional form element to filter multiple exposed
        filter'),
        'handler' => 'filterable_views_filter_exposed_form_plugin',
        'uses row plugin' => FALSE,
        'uses fields' => TRUE,
        'uses options' => TRUE,
        'help topic' => 'exposed-form-basic',
        'type' => 'normal',
        'parent' => 'basic',
      ),
    ),
  );
}
