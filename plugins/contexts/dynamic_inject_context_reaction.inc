<?php
/**
 * @file
 * Context reaction plugin for Dynamic Inject.
 */

/**
 * Reaction form for inject elements.
 */
class dynamic_inject_context_reaction extends context_reaction {
  /**
   * Options form.
   * @see context_reaction::options_form()
   */
  function options_form($context) {
    // Get existing values for this form.
    $settings = $this->fetch_from_context($context);
    $existing_elements = isset($settings['elements']) ? $settings['elements'] : array();

    return _dynamic_inject_elements_form($existing_elements);
  }

  /**
   * Options form submit handler.
   */
  function options_form_submit($values) {
    // Remove form elements, such as buttons, that shouldn't be exported in context.
    foreach ($values['elements'] as &$element) {
      unset($element['remove']);
    }
    unset($values['dynamic_inject_more_elements']);

    return $values;
  }

  /**
   * Return a keyed array containing the settings.
   */
  function execute(&$variables) {
    foreach ($this->get_contexts() as $context) {
      if (!isset($context->reactions['dynamic_inject_context'])) {
        continue;
      }
      if (($context->reactions['dynamic_inject_context']['elements'][0]['type']) != NULL) {
        $this->injectElements($variables, $context->reactions['dynamic_inject_context']['elements']);
      }
    }
  }

  /**
   * Inject elements into node fields.
   *
   * @param $variables
   * @param $config
   */
  protected function injectElements(&$variables, $config) {
    foreach ($config as $item) {

      // Skip if the configured field isn't available.
      $field_name = $item['field'];
      if (!isset($variables['content'][$field_name][0]['#markup'])) {
        continue;
      }
      // Only in the selected view mode.
      if ($variables['view_mode'] != $item['view_mode']) {
        continue;
      }
      // Create Dom Document object.
      // Pass it the html from the body field.
      $dom = filter_dom_load($variables['content'][$field_name][0]['#markup']);

      // Create xpath object.
      $xPath_dom = new DOMXPath($dom);
      $elements_html = '';

      if (module_exists('dynamic_inject_' . $item['type'])) {
        // Find out if we want to exclude the element.
        $result = module_invoke_all('dynamic_inject_exclude', $xPath_dom, $item);
        if (isset($result[0]) && $result[0] == 1) {
          return;
        }
        // We want to get the render html element (block, placeholder..)
        $elements_html = module_invoke_all('dynamic_inject_render_' . $item['type'], $item);
      }

      // P, H1, H2, H3.
      $html_tag_name = strtolower($item['tag_name']);
      $tag_count = $xPath_dom->query('//' . $html_tag_name)->length;
      $min_siblings = isset($item['min_siblings']) ? $item['min_siblings'] : 0;
      $min_word_count = isset($item['min_word_count']) ? $item['min_word_count'] : 0;

      $word_count = str_word_count($variables['content'][$field_name][0]['#markup']);
      if ($word_count < $min_word_count) {
        return;
      }

      // Skip placement if the callback renders empty.
      if (empty($elements_html)) {
        continue;
      }
      // Get site basepath to get to the field.
      $base_path = dynamic_inject_get_xpath_basepath_for_element($xPath_dom, $html_tag_name);

      // Building the xpath query from position value.
      if ($item['guide'] == 'top') {
        $query = $xPath_dom->query($base_path . $html_tag_name . '[' . $item['pos'] . ']');
      }
      elseif ($item['guide'] == 'bottom') {
        $query = $xPath_dom->query($base_path . $html_tag_name . '[last()-' . $item['pos'] . ']');
      }

      // Get the widget markup.
      $inject_markup = array_pop($elements_html);
      // We insert elements where selectors found.
      foreach ($query as $key => $anchor) {

        // Create a placeholder to replace later with the widget markup.
        $placeholder = dynamic_inject_get_comment_placeholder($inject_markup, $dom);

        // Only display elements if we have enough elements before the bottom.
        if ($item['pos'] + $min_siblings < $tag_count) {
          if ($item['before_after'] == 'before') {
            // check if previous sibling exist, otherwise insert to first position
            $previous_position = ($anchor->previousSibling) ? $anchor->previousSibling : $anchor->parentNode->firstChild;
            $anchor->parentNode->insertBefore($placeholder, $previous_position);
          }
          elseif ($item['before_after'] == 'after') {
            $anchor->parentNode->insertBefore($placeholder, $anchor->nextSibling);
          }
        }
        // Replace the placeholder with the widget markup.
        $markup = dynamic_inject_get_markup($placeholder, $inject_markup, $dom);

        $variables['content'][$field_name][0]['#markup'] = $markup;
      }
    }
  }
}

/**
 * Get a unique string to use as our placeholder.
 *
 * @param $string
 * @param $dom
 *
 * @return string
 */
function dynamic_inject_get_comment_placeholder($string, $dom) {
  $comment = md5($string);
  $element = $dom->createComment($comment);

  return $element;
}

/**
 * Get the markup.
 * Replace the placeholder with the widget markup.
 *
 * @param $element
 * @param $html
 * @param $dom
 *
 * @return mixed
 */
function dynamic_inject_get_markup($element, $html, $dom) {
  $doc = filter_dom_serialize($dom);

  return str_replace('<!--' . $element->nodeValue . '-->', $html, $doc);
}

/**
 * Hook_get_xpath_basepath_for_element().
 * It returns the basepath for the body element as some
 * sites might have a div inside and some others not.
 *
 * @param $xPath
 * @param $html_tag_name
 *
 * @return string
 */
function dynamic_inject_get_xpath_basepath_for_element($xPath, $html_tag_name) {
  // Get all the elements in the page matching the context item html tag.
  $html_tag_name_result = $xPath->query('//' . $html_tag_name);

  $path_value = array();
  // Get their path.
  foreach ($html_tag_name_result as $key => $value) {
    $path_value[] = $value->getNodePath();
  }
  // Get the path length.
  $path_lenghts = array_map('strlen', $path_value);
  // Short the paths by length.
  array_multisort($path_lenghts, $path_value);
  // Get the shortest path.
  $shortest_path = $path_value[0];
  // Remove the context item html tag.
  if ($pieces = explode($html_tag_name, $shortest_path)) {
    // The fist part will be the basepath.
    $base_path = $pieces[0];
  }
  else {
    // Fallback if no basepath is found.
    $base_path = '//';
  }

  return $base_path;
}
