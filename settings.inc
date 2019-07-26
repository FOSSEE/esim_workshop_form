<?php
// $Id$
function esim_workshop_settings_form($form, &$form_state)
  {
    $form['extensions']['workshop_project_file_extensions'] = array(
        '#type' => 'textfield',
        '#title' => t('Allowed source file extensions'),
        '#description' => t('A comma separated list WITHOUT SPACE of source file extensions that are permitted to be uploaded on the server'),
        '#size' => 50,
        '#maxlength' => 255,
        '#required' => TRUE,
        '#default_value' => variable_get('workshop_project_file_extensions', '')
    );
    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Submit')
    );
    return $form;
  }
function esim_workshop_settings_form_validate($form, &$form_state)
  {
    return;
  }
function esim_workshop_settings_form_submit($form, &$form_state)
  {
    variable_set('workshop_project_file_extensions', $form_state['values']['workshop_project_file_extensions']);
    drupal_set_message(t('Settings updated'), 'status');
  }
