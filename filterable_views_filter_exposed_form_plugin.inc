<?php
/**
 * @file
 * Provides an Better Exposed Filters exposed form plugin for View 3.x.
 * The form should use #states and a multiple select but this is blocked
 * by core issue: #1149078
 */

class filterable_views_filter_exposed_form_plugin extends views_plugin_exposed_form_basic {

  function summary_title() {
    return t('Settings');
  }

  function option_definition() {
    $options = parent::option_definition();

    // @TODO: add an option to choose which filters to hide.
    $options['pvf'] = array('default' => array());

    // @TODO: still needed?
    // Options for the input required setting.
    $options['input_required'] = array('default' => FALSE);
    $options['text_input_required'] = array('default' => t('Select any filter and click on Apply to see results'), 'translatable' => TRUE);
    $options['text_input_required_format'] = array('default' => NULL);
    // @TODO: Eventually add the ajax loading as option.

    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
  }

  /**
   * Tweak the exposed filter form to show Better Exposed Filter options.
   *
   * @param array $form
   *   Exposed form array
   * @param array $form_state
   *   Current state of form variables
   */
  function exposed_form_alter(&$form, &$form_state) {
    parent::exposed_form_alter($form, $form_state);

    // Get all labelnames and hide labels.
    // We need to hide the labels since we can't hide them otherwise.
    $options = array(t('Select'));
    foreach ($form['#info'] as $key => $info) {
      $options[$key] = $info['label'];
      unset($info['label']);
      $form['#info'][$key] = $info;
    }

    // Wrap all elements in a container and hide it
    // Don't hide if it has a submitted value.
    $defaults = array();
    foreach ($form as $key => $element) {

      // Skip all non form elements except submit.
      if (substr($key, 0, 1) != '#' && $key != 'submit') {
        // @TODO: create a theming function.
        $element['#suffix'] = '<div class="state-close">X</div>';
        // @TODO: try rendering the labels.
        $element['#prefix'] = '<label>' . $options['filter-' . $key] . '</label>';

        // Fix checkboxes with no values.
        if ($element['#type'] == 'select') {
          $fixed_boolean = $this::brokenCheckboxValues($element);
          if ($fixed_boolean) {
            $form[$key] = $element;
          }
        }

        // Check if value is set.
        $form[$key] = array(
          '#type' => 'container',
          '#attributes' => array(
            'class' => array(
              'filter-' . $key,
              'filter-closable',
            ),
          ),
          $key => $element,
        );

        // Hide the elements by default when no value is set.
        if (!isset($form_state['input'][$key]) || $form_state['input'][$key] == '' || $form_state['input'][$key] == 'All') {
          $form[$key]['#attributes']['class'][] = 'element-hidden';
          // Save default values.
        }
        else {
          $form[$key]['#attributes']['class'][] = 'element-not-hidden';
        }
        $defaults['filter-' . $key] = $element['#default_value'];
      }
    }

    // Pass the defaults to settings.
    drupal_add_js(array('filterable_views_filter' => $defaults), array('type' => 'setting'));
    $this::addSelectElement($form, $options);

    // AJAXifying the form.

    // $ajaxify = array(
    //   'callback' => 'filterable_views_filter_ajax_callback',
    //   'event' => 'change',
    //   'wrapper' => 'filterable_views_filter-exposed-form-wrapper',
    //   'keypress' => TRUE,
    // );

    // $form['submit']['#ajax'] = $ajaxify;
    // $form['submit']['#prefix'] = '<div id="filterable_views_filter-exposed-form-wrapper">';
    // $form['submit']['#suffix'] = '</div>';

    // // AJAX behaviors need these data, so we add it back in #after_build.
    // // @see : https://www.drupal.org/node/1183418
    // $form['#after_build'][] = 'filterable_views_filter_exposed_form_ajax_enable';

    // $this::addAjaxAttributes($form, $ajaxify, array('filterable_views_filter_filter', 'submit'));

    // Hide the submit button.
    // $form['submit']['#attributes']['class'][] = 'element-hidden';
  }


  /**
   * Add select Filter field and push it to first position.
   */
  private function addSelectElement(&$form, $options) {
    $form = array(
      'filterable_views_filter_filter' => array(
        '#type' => 'select',
        '#options' => $options,
        '#weight' => -100,
        '#attributes' => array(
          'class' => array('state-selector'),
        ),
        '#title' => t('Filter'),
      ),
    ) + $form;

    $form['#info'] = array(
      'filter-filterable_views_filter_filter' => array(
        'operator'  => '',
        'value'  => 'filterable_views_filter_filter',
        'description'  => '',
        '#weight'  => '',
      ),
    ) + $form['#info'];

  }

  /**
   * Helper function to add ajax on elements.
   * @param $element The element to be ajaxified.
   * @param $exclude_keys  The element matching any values is the array is
   *  excluded from ajaxifying.
   */
  private function addAjaxAttributes(&$element, $ajax_attributes, $exclude_keys = array()) {
    $keys = element_children($element);
    if ($keys AND is_array($keys)) {
      foreach ($keys as $key) {
        if (isset($element[$key]['#type'])) {
          switch ($element[$key]['#type']) {
            case 'container':
            case 'fieldset':
              $this::addAjaxAttributes($element[$key], $ajax_attributes, $exclude_keys);
              break;
            default:
              if (!in_array($key, $exclude_keys)) {
                $element[$key]['#ajax'] = $ajax_attributes;
              }
              break;
          }
        }
      }
    }
  }

  /**
   * Check if form element is boolean with no proper field value labels.
   *
   * If a useful patch will get committed for #1750950 this get's obsolete.
   */
  private function brokenCheckboxValues(&$element) {
    if (!isset($element['#options']) || !is_array($element['#options'])) {
      return FALSE;
    }
    foreach ($element['#options'] as $key => $value) {
      if (empty($value) && $key == 0) {
        $element['#options'][0] = t('No');
        $set = TRUE;
      }
      if (empty($value) && $key == 1) {
        $element['#options'][1] = t('Yes');
        $set = TRUE;
      }
    }
    if (isset($set)) {
      return TRUE;
    }
  }
}
