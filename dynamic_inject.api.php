<?php
/**
 * @file
 * Hooks provided by the Dynamic Inject module.
 */

/**
 * @defgroup dynamic_inject_[submodule]_hooks Dynamic Inject [Submodule] Hooks
 * @{
 * Functions to define new inject elements.
 *
 * Here is a list of the hooks that are used to define an inject element type.
 * - Providing configuration of inject element:
 *   - hook_dynamic_inject_type_info()
 * - Rendering Inline Element:
 *   - hook_render_element()
 * @}
 */


/**
 *
 * hook_dynamic_inject_type_info().
 *
 * Defines all the fields available from Dynamic Inject context.
 *
 * @return array
 *   An associative array which key is the name of the inject element type. It's structured
 *   as follows:
 *   - [element_name]: A machine name naming the inject element type.
 *   - [element_name][name]: Name of the inject element type.
 *   - [element_name][config]: Array containing the configuration for each field.
 *   - [element_name][config][field_1]: machine name for the inject element field to show in context.
 *   - [element_name][config][field_1][type]: Form element type. @see Drupal Form API.
 *   - [element_name][config][field_1][name]: Name of the form element to show in the context.
 *   - [element_name][config][field_1][options]: Options for the form element. (optional)
 */
function hook_dynamic_inject_type_info() {
  return array(
    'element_name' => array(
      'name' => t('Element Name'),
      'config' => array(
        'field_1' => array(
          'type' => 'field_type',
          'name' => t('Element Field name'),
          'options' => array('option_1' => 'Option 1'),
        ),
      ),
    ),
  );
}

/**
 * hook_dynamic_inject_render_TYPE().
 *
 * Defines how the rendering array is built variable per inject element type.
 *
 * @param $item
 *   Array containing the configuration fo the field from the context.
 *   See hook_dynamic_inject_type_info() for information on what this may contain.
 * @return string
 *   The rendered element.
 */
function hook_dynamic_inject_render_TYPE($item) {
  $render_array = array(/* Build render array depending on element type.*/);
  $output = render($render_array);

  return $output;
}
