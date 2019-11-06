<?php

use Drupal\image\Entity\ImageStyle;

/**
 * Implements hook_preprocess_HOOK() for HTML document templates.
 */
function umd_terp_preprocess_html(&$variables) {
  $variables['is_front_page'] = \Drupal::service('path.matcher')->isFrontPage();
  // Add body classes based on path.
  $variables['current_path'] = \Drupal::service('path.current')->getPath();
  $variables['current_path_alias'] = ltrim(\Drupal::service('path.alias_manager')->getAliasByPath($variables['current_path']), '/');
  $variables['attributes']['class'][] = 'path--' . \Drupal\Component\Utility\Html::cleanCssIdentifier($variables['current_path_alias'], array('/' => '--'));

  if ($variables['is_front_page']) {
    $variables['attributes']['class'][] = 'home';
  }
  else {
    $variables['attributes']['class'][] = 'internal';
  }
}

/**
 * Implements hook_preprocess_HOOK() for page templates.
 */
function umd_terp_preprocess_page(&$variables) {
  $variables['hero_image'] = '';
  $variables['hero_title'] = $variables['page']['#title'];

  if ($node = _umd_terp_get_node_by_route()) {
    // Set hero details
    if ($node->hasField('field_umdt_ct_hero_image') && $node->get('field_umdt_ct_hero_image')->entity) {
      $hero_uri = $node->get('field_umdt_ct_hero_image')->entity->get('field_media_image')->entity->getFileUri();
    }
    if (!empty($hero_uri)) {
      $variables['hero_image'] = ImageStyle::load('optimized')->buildUrl($hero_uri);
    }

    switch($node->getType()) {
      case 'umd_terp_basic_page':
        $variables['hide_sidebar'] = $node->get('field_umdt_ct_hide_sidebar')->value;
      break;
    }
  }
  // Make any custom theme settings available in the page tpl for header/footer/etc
  _umd_terp_extend_theme_variables($variables);
}

/**
 * Implements hook_preprocess_breadcrumb() for node.html.twig.
 */
function umd_terp_preprocess_breadcrumb(&$variables){
  $request = \Drupal::request();
  $route_match = \Drupal::routeMatch();
  if(($node = $route_match->getParameter('node')) && $variables['breadcrumb']){
    if (is_numeric($node)) {
      $node = Node::load($node);
    }
    $variables['breadcrumb'][] = array(
      'text' => $node->getTitle()
    );
    $variables['#cache']['contexts'][] = 'url';
  }
}

/**
 * Implements template_preprocess_node
 *
 * Add template suggestions and classes
 */
function umd_terp_preprocess_node(&$variables) {
  // Add node--node_type--view_mode.html.twig suggestions
  $variables['theme_hook_suggestions'][] = 'node__' . $variables['elements']['#node']->bundle() . '__' . $variables['view_mode'];
  // Add node--view_mode.html.twig suggestions
  $variables['theme_hook_suggestions'][] = 'node__' . $variables['view_mode'];
  // Add a class for the view mode.
  if (!$variables['teaser']) {
    $variables['content_attributes']['class'][] = 'view-mode-' . $variables['view_mode'];
  }

  // Note: Is this needed if we handle breadcrumb and sidebar on page level?
  // // Allow breadcrumbs/sidebar to be available in the node template.
  // $allowed_regions = ['breadcrumbs', 'sidebar'];

  // _umd_terp_add_regions_to_entity($allowed_regions, $variables);
  // Allow use of theme varables in node templates
  _umd_terp_extend_theme_variables($variables);

}

/**
 * Implements hook_preprocess_form_element().
 */
function umd_terp_preprocess_form_element(array &$variables) {
  if (!isset($variables['label'])) {
    return;
  }

  if (isset($variables['element']['#form_id'])) {
    $variables['label']['#form_id'] = $variables['element']['#form_id'];
  }
  if (isset($variables['element']['#type'])) {
    $variables['label']['#form_element_type'] = $variables['element']['#type'];
  }
}

function umd_terp_preprocess_menu_local_task(&$variables) {
  $class = array('nav-link');
  if($variables['element']["#active"])
  {
    $class[] = 'active';
  }
  $variables['element']['#link']['url']->setOption('attributes', array('class' => $class));
}