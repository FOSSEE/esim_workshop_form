<?php
/**
 * Fetches and displays the form.
 */
function esim_workshop_form_page()
{
	global $user;
	$data = '';
	if ($user->uid == 0)
	{
		$msg = drupal_set_message(t('It is mandatory to ' . l('login', 'user') . ' on this website to access the workshop form. If you are new user please create a new account first.'), 'error');
		drupal_goto('user');
		return $msg;
	} //!$user
	/************* Check if user has already submitted a proposal **************/
	$query = db_select('esim_workshop_2019_form_step1_feedback');
	$query->fields('esim_workshop_2019_form_step1_feedback');
	$query->condition('uid', $user->uid);
	$query->orderBy('id', 'DESC');
	$query->range(0, 1);
	$feedback_q = $query->execute();
	if ($feedback_q)
	{
		if ($feedback_data = $feedback_q->fetchObject())
		{
			if ($feedback_data->submitted == 1)
			{
				$data .= drupal_set_message(t('You have already submitted the feedback.'), 'status');
				$data .= drupal_goto('');
				return;
			} //$feedback_data->submitted == 1
			else
			{	//$data .= drupal_goto('esim-workshop/form/step1/'.$user->uid);
				$esim_workshop_step1_form = drupal_get_form("esim_workshop_step1_form");
				$data .= drupal_render($esim_workshop_step1_form);
			}
		} //$feedback_data = $feedback_q->fetchObject()
		else
		{
			$esim_workshop_step1_form = drupal_get_form("esim_workshop_step1_form");
			$data .= drupal_render($esim_workshop_step1_form);
		}
	} //$feedback_q
	//$data .= drupal_get_form('esim_workshop_step1_form');
	return $data;
}
/**
 * Displays simple confirmation page after finishing the whole process.
 */
function esim_workshop_success()
{
	$data .= drupal_set_message(t('You have successfully submitted the feedback.'), 'status');
	$data .= drupal_goto('home');
	return $data;
}
/**
 * Generates the form.
 */
/********* Step1 starts - contains collecting basic information *****************************/
function esim_workshop_step1_form($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	global $base_url;
	if ($user->uid == 0)
	{
		$msg = drupal_set_message(t('It is mandatory to ' . l('login', 'user') . ' on this website to access the flowsheet proposal form. If you are new user please create a new account first.'), 'error');
		//drupal_goto('om-flowsheet-project');
		drupal_goto('user');
		return $msg;
	}
	// All the real form fields.
	$query = db_select('esim_workshop_2019_form_step1_feedback');
	$query->fields('esim_workshop_2019_form_step1_feedback');
	$query->condition('uid', $user->uid);
	$form_step1_data = $query->execute();
	if ($form_step1_data)
	{
		if ($step1_data = $form_step1_data->fetchObject())
		{
			/* everything ok */
		} //$step1_data = $form_step1_data->fetchObject()
	} //$form_step1_data
	$form = array();
	$form['step1_field1_name'] = array(
		'#title' => 'Name',
		'#type' => 'textfield',
		'#maxlength' => 100,
		'#default_value' => $step1_data->name,
		'#required' => TRUE
	);
	$form['step1_field5_rcid'] = array(
		'#title' => 'RC Id number',
		'#type' => 'textfield',
		'#default_value' => $step1_data->rcid,
		'#description' => t('<span style="color:red;">For eg. 4312</span>'),
		'#required' => TRUE
	);
	$form['step1_field2_email_id'] = array(
		'#type' => 'textfield',
		'#title' => t('Email'),
		'#size' => 30,
		'#value' => $user->mail,
		'#disabled' => TRUE,
		'#required' => TRUE
		//'#default_value' => $form_state['storage']['values'][1]['step1_field2_email_id']
	);
	$form['step1_field3_institute'] = array(
		'#title' => 'Institute name',
		'#type' => 'textfield',
		'#maxlength' => 100,
		'#default_value' => $step1_data->institute,
		'#required' => TRUE
	);
	$form['step1_field6_department'] = array(
		'#title' => 'Department',
		'#type' => 'textfield',
		'#maxlength' => 100,
		'#default_value' => $step1_data->department,
		'#required' => TRUE
	);
	$form['step1_field4_designation'] = array(
		'#title' => 'Designation',
		'#type' => 'textfield',
		'#maxlength' => 100,
		'#default_value' => $step1_data->designation,
		'#required' => TRUE
	);
	$form["step1_page_id"] = array(
		"#type" => "hidden",
		"#value" => "step1"
	);
	$form['next1'] = array(
		'#type' => 'submit',
		'#value' => 'Save & Next',
		'#submit' => array(
			'esim_workshop_step1_form_submit'
		)
	);
	return $form;
}
function esim_workshop_step1_form_validate($form, &$form_state, $no_js_use = FALSE)
{
	if (!preg_match('/^[0-9]{4}$/', $form_state['values']['step1_field5_rcid']))
	{
		form_set_error('step1_field5_rcid', t('RCID should be a 4 digit number'));
	}
}

function esim_workshop_step1_form_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$query = db_select('esim_workshop_2019_form_step1_feedback');
	$query->fields('esim_workshop_2019_form_step1_feedback');
	$query->condition('uid', $user->uid);
	$form_step1_data = $query->execute();
	if ($form_step1_data)
	{
		if ($step1_data = $form_step1_data->fetchObject())
		{
			$query = "UPDATE esim_workshop_2019_form_step1_feedback SET name=:name, rcid=:rcid, email_id=:email_id, institute=:institute, designation=:designation, department = :department WHERE uid=:uid ";
			$args = array(
				":uid" => $user->uid,
				":name" => $v['step1_field1_name'],
				":rcid" => $v['step1_field5_rcid'],
				":email_id" => $v['step1_field2_email_id'],
				":institute" => $v['step1_field3_institute'],
				":designation" => $v['step1_field4_designation'],
				":department" => $v['step1_field6_department']
			);
			db_query($query, $args);
		} //$step1_data = $form_step1_data->fetchObject()
		else
		{
			$query = "INSERT INTO {esim_workshop_2019_form_step1_feedback} (uid, name, rcid, email_id, institute, designation, department, data_reletion, inserted_on) VALUES (:uid, :name, :rcid, :email_id, :institute, :designation, :department, :data_reletion,:inserted_on)";
			$args = array(
				":uid" => $user->uid,
				":name" => $v['step1_field1_name'],
				":rcid" => $v['step1_field5_rcid'],
				":email_id" => $v['step1_field2_email_id'],
				":institute" => $v['step1_field3_institute'],
				":designation" => $v['step1_field4_designation'],
				":department" => $v['step1_field6_department'],
				":data_reletion" => "step1",
				":inserted_on" => date('Y-m-d')
			);
			db_query($query, $args);
		}
	} //$form_step1_data
	$form_state['redirect'] = 'esim-workshop/form/step2/' . $user->uid;
}

/****************************** Step 1 ends ****************************************************/

/************ Step 2 form starts - contains instructions and a checkbox ****************/
function esim_workshop_step2_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	$img_path = $base_url . '/' . drupal_get_path("module", "esim_workshop_form") . '/images';
	$form = array();
	$form['step2_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step2-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('esim_workshop_2019_form_step2_feedback');
	$query->fields('esim_workshop_2019_form_step2_feedback');
	$query->condition('uid', $user->uid);
	$form_step2_data = $query->execute();
	if ($form_step2_data)
	{
		if ($step2_data = $form_step2_data->fetchObject())
		{
			$form['step2_field1_fieldset']['step2_field1_question1'] = array(
				'#markup' => '<h4>Important instructions for participants during the workshop</h4><ol>
<li style="font-weight: 400;"><span style="font-weight: 400;">The list of tasks to do during the workshop is available in the subsequent pages. Every page will describe some activity. It may also ask you to input some text or make a note in order for you to proceed to the next page.</span></li>
<li style="font-weight: 400;"><span style="font-weight: 400;">Use the &lt;next page&gt;, &lt;previous page&gt; buttons to navigate between the tasks.</span></li>
<li style="font-weight: 400;"><span style="font-weight: 400;">It is important for you to finish all tasks during the workshop in order to avail a certificate etc. In case if you could not finish all tasks, use the &lt;save draft&gt; button (if available) to save your progress until the current page.
</span></li>
<li style="font-weight: 400;"><span style="font-weight: 400;">If you come across any difficulty during the workshop, you are expected to post a question on the forums.</span>
<ol>
<li style="font-weight: 400;"><span style="font-weight: 400;">For questions related to the workshop activity, eSim software or the PCB that will be given to you, post on </span><a href="https://forums.fossee.in/" target="_blank"><span style="font-weight: 400;">https://forums.fossee.in/</span></a></li>
<li style="font-weight: 400;"><span style="font-weight: 400;">For questions related to the eSim Spoken Tutorials, post on </span><a href="https://forums.spoken-tutorial.org/" target="_blank" target="_blank"><span style="font-weight: 400;">https://forums.spoken-tutorial.org/</span></a></li>
</ol></li>
<li><span style="font-weight: 400;">It is mandatory for you to upload your project files at the end of the workshop. We may publish your submissions on our website.</span></li>
</ol>',
				'#type' => 'item',
			);
			$form['step2_field1_fieldset']['step2_checkbox_status'] = array(
				'#type' => 'checkbox',
	            '#title' => t('I have read, understood and accepted the instructions to be followed during the workshop'),
	            '#required' => TRUE,
	            '#default_value' => $step2_data->step2_checkbox_status,
        	);
			$form["step2_page_id"] = array(
				"#type" => "hidden",
				"#value" => "step2"
			);
			$form['previous1'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'esim_workshop_step2_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'esim_workshop_step2_form_next_submit'
				)
			);
		} //$step2_data = $form_step2_data->fetchObject()
		else
		{
			$form['step2_field1_fieldset']['step2_field1_question1'] = array(
				'#markup' => '<h4>Important instructions for participants during the workshop</h4><ol>
<li style="font-weight: 400;"><span style="font-weight: 400;">The list of tasks to do during the workshop is available in the subsequent pages. Every page will describe some activity. It may also ask you to input some text or make a note in order for you to proceed to the next page.</span></li>
<li style="font-weight: 400;"><span style="font-weight: 400;">Use the &lt;next page&gt;, &lt;previous page&gt; buttons to navigate between the tasks.</span></li>
<li style="font-weight: 400;"><span style="font-weight: 400;">It is important for you to finish all tasks during the workshop in order to avail a certificate etc. In case if you could not finish all tasks, use the &lt;save draft&gt; button (if available) to save your progress until the current page.
</span></li>
<li style="font-weight: 400;"><span style="font-weight: 400;">If you come across any difficulty during the workshop, you are expected to post a question on the forums.</span>
<ol>
<li style="font-weight: 400;"><span style="font-weight: 400;">For questions related to the workshop activity, eSim software or the PCB that will be given to you, post on </span><a href="https://forums.fossee.in/" target="_blank"><span style="font-weight: 400;">https://forums.fossee.in/</span></a></li>
<li style="font-weight: 400;"><span style="font-weight: 400;">For questions related to the eSim Spoken Tutorials, post on </span><a href="https://forums.spoken-tutorial.org/" target="_blank"><span style="font-weight: 400;">https://forums.spoken-tutorial.org/</span></a></li>
</ol></li>
<li><span style="font-weight: 400;">It is mandatory for you to upload your project files at the end of the workshop. We may publish your submissions on our website.</span></li>
</ol>',
				'#type' => 'item',
			);
			$form['step2_field1_fieldset']['step2_checkbox_status'] = array(
				'#type' => 'checkbox',
	            '#title' => t('I have read, understood and accepted the instructions to be followed during the workshop'),
	            '#required' => TRUE,
        	);
			$form["step3_page_id"] = array(
				"#type" => "hidden",
				"#value" => "step3"
			);
			$form['previous1'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'esim_workshop_step2_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'esim_workshop_step2_form_next_submit'
				)
			);
		} //$step2_data = $form_step2_data->fetchObject()
	} //$form_step2_data
	return $form;
}
function esim_workshop_step2_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$query = db_select('esim_workshop_2019_form_step2_feedback');
	$query->fields('esim_workshop_2019_form_step2_feedback');
	$query->condition('uid', $user->uid);
	$form_step2_data = $query->execute()->fetchObject();
	if ($form_step2_data)
	{
		if ($form_state['values']['step2_field1_fieldset']['step2_checkbox_status'] == 1){
			$step2_checkbox_status = 1;
		}
		$query = "UPDATE esim_workshop_2019_form_step2_feedback SET step2_checkbox_status=:step2_checkbox_status, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":step2_checkbox_status" => $step2_checkbox_status,
			":data_relation" => "step2"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
	} //$form_step2_data
	else
	{
		if ($form_state['values']['step2_field1_fieldset']['step2_checkbox_status'] == 1){
			$step2_checkbox_status = 1;
		}
		$query = "INSERT INTO {esim_workshop_2019_form_step2_feedback} (uid, step2_checkbox_status, data_relation,inserted_on) VALUES (:uid,:step2_checkbox_status, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":step2_checkbox_status" => $step2_checkbox_status,
			":data_relation" => "step2",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
	}
	$form_state['redirect'] = 'esim-workshop/form/step1/' . $user->uid;
}
function esim_workshop_step2_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$query = db_select('esim_workshop_2019_form_step2_feedback');
	$query->fields('esim_workshop_2019_form_step2_feedback');
	$query->condition('uid', $user->uid);
	$form_step2_data = $query->execute()->fetchObject();
	if ($form_step2_data)
	{
		if ($form_state['values']['step2_field1_fieldset']['step2_checkbox_status'] == 1){
			$step2_checkbox_status = 1;
		}
		$query = "UPDATE esim_workshop_2019_form_step2_feedback SET step2_checkbox_status=:step2_checkbox_status, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":step2_checkbox_status" => $step2_checkbox_status,
			":data_relation" => "step2"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
	} //$form_step2_data
	else
	{
		if ($form_state['values']['step2_field1_fieldset']['step2_checkbox_status'] == 1){
			$step2_checkbox_status = 1;
		}
		$query = "INSERT INTO {esim_workshop_2019_form_step2_feedback} (uid, step2_checkbox_status, data_relation,inserted_on) VALUES (:uid,:step2_checkbox_status, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":step2_checkbox_status" => $step2_checkbox_status,
			":data_relation" => "step2",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
	}
	$form_state['redirect'] = 'esim-workshop/form/step3/' . $user->uid;
}
/***************************************** step 2 from ends ****************************/

/***** Step 3 form starts - contains circuit schematic and a checkbox *********/
function esim_workshop_step3_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	$img_path = $base_url . '/' . drupal_get_path("module", "esim_workshop_form") . '/images';
	$step3_options = array(
		'Yes' => 'Yes',
		'No' => 'No'
	);
	$form = array();
	$form['step3_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step3-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('esim_workshop_2019_form_step3_feedback');
	$query->fields('esim_workshop_2019_form_step3_feedback');
	$query->condition('uid', $user->uid);
	$form_step3_data = $query->execute();
	if ($form_step3_data)
	{
		if ($step3_data = $form_step3_data->fetchObject())
		{
			$form['step3_field1_fieldset']['step3_field1_question1'] = array(
				'#markup' => '<h4>Task 1 : Creation of circuit schematic</h4><span>
Create a circuit schematic of an ‘Astable Multivibrator’ as shown in Figure 1 using eSim. You may refer to the corresponding Spoken Tutorials, if required. Use components only from eSim_Devices, eSim_Sources and eSim_Plot category during component selection.</span><br>
<center><img src="' . $img_path . '/AstableMultivibrator.png" width="70%" height="80%" align="middle"/><br><span>Figure 1: Astable Multivibrator</span></center>',
				'#type' => 'item',
			);
			$form['step3_field1_fieldset']['step3_checkbox_status'] = array(
				'#type' => 'checkbox',
	            '#title' => t('I could successfully create the circuit schematic'),
	            '#required' => TRUE,
	            '#default_value' => $step3_data->step3_checkbox_status,
        	);
        	$form['step3_field1_fieldset']['step3_checkbox_status_1'] = array(
				'#type' => 'checkbox',
	            '#title' => t('I have carefully chosen components only from eSim_Devices, eSim_Sources and eSim_Plot category during component selection'),
	            '#required' => TRUE,
	            '#default_value' => $step3_data->step3_checkbox_status_1,
        	);
  			$form["step3_page_id"] = array(
				"#type" => "hidden",
				"#value" => "step3"
			);
			$form['previous1'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'esim_workshop_step3_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'esim_workshop_step3_form_next_submit'
				)
			);
		} //$step3_data = $form_step3_data->fetchObject()
		else
		{
			$form['step3_field1_fieldset']['step3_field1_question1'] = array(
				'#markup' => '<h4>Task 1 : Creation of circuit schematic</h4><span>
Create a circuit schematic of an ‘Astable Multivibrator’ as shown in Figure 1 using eSim. You may refer to the corresponding Spoken Tutorials, if required. Use components only from eSim_Devices, eSim_Sources and eSim_Plot category during component selection.</span><br>
<center><img src="' . $img_path . '/AstableMultivibrator.png" width="70%" height="80%" align="middle"/><br><span>Figure 1: Astable Multivibrator</span></center>',
				'#type' => 'item',
			);
			$form['step3_field1_fieldset']['step3_checkbox_status'] = array(
				'#type' => 'checkbox',
	            '#title' => t('I could successfully create the circuit schematic'),
	            '#required' => TRUE,
        	);
        	$form['step3_field1_fieldset']['step3_checkbox_status_1'] = array(
				'#type' => 'checkbox',
	            '#title' => t('I have carefully chosen components only from eSim_Devices, eSim_Sources and eSim_Plot category during component selection'),
	            '#required' => TRUE,
        	);
			$form["step3_page_id"] = array(
				"#type" => "hidden",
				"#value" => "step3"
			);
			$form['previous1'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'esim_workshop_step3_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'esim_workshop_step3_form_next_submit'
				)
			);
		} //$step3_data = $form_step3_data->fetchObject()
	} //$form_step3_data
	return $form;
}
function esim_workshop_step3_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$query = db_select('esim_workshop_2019_form_step3_feedback');
	$query->fields('esim_workshop_2019_form_step3_feedback');
	$query->condition('uid', $user->uid);
	$form_step3_data = $query->execute()->fetchObject();
	if ($form_step3_data)
	{
		$query = "UPDATE esim_workshop_2019_form_step3_feedback SET step3_checkbox_status=:step3_checkbox_status, step3_checkbox_status_1=:step3_checkbox_status_1, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":step3_checkbox_status" => $v['step3_field1_fieldset']['step3_checkbox_status'],
			":step3_checkbox_status_1" => $v['step3_field1_fieldset']['step3_checkbox_status_1'],
			":data_relation" => "step3"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
	} //$form_step3_data
	else
	{
		if ($form_state['values']['step3_field1_fieldset']['step3_checkbox_status'] == 1){
			$step3_checkbox_status = 1;
		}
		$query = "INSERT INTO {esim_workshop_2019_form_step3_feedback} (uid, step3_checkbox_status,step3_checkbox_status_1, data_relation,inserted_on) VALUES (:uid,:step3_checkbox_status,s:step3_checkbox_status_1, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":step3_checkbox_status" => $v['step3_field1_fieldset']['step3_checkbox_status'],
			":step3_checkbox_status_1" => $v['step3_field1_fieldset']['step3_checkbox_status_1'],
			":data_relation" => "step3",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
	}
	$form_state['redirect'] = 'esim-workshop/form/step2/' . $user->uid;
}
function esim_workshop_step3_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$query = db_select('esim_workshop_2019_form_step3_feedback');
	$query->fields('esim_workshop_2019_form_step3_feedback');
	$query->condition('uid', $user->uid);
	$form_step3_data = $query->execute()->fetchObject();
	if ($form_step3_data)
	{
		$query = "UPDATE esim_workshop_2019_form_step3_feedback SET step3_checkbox_status=:step3_checkbox_status, step3_checkbox_status_1=:step3_checkbox_status_1, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":step3_checkbox_status" => $v['step3_field1_fieldset']['step3_checkbox_status'],
			":step3_checkbox_status_1" => $v['step3_field1_fieldset']['step3_checkbox_status_1'],
			":data_relation" => "step3"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
	} //$form_step3_data
	else
	{
		$query = "INSERT INTO {esim_workshop_2019_form_step3_feedback} (uid, step3_checkbox_status,step3_checkbox_status_1, data_relation,inserted_on) VALUES (:uid,:step3_checkbox_status,:step3_checkbox_status_1, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":step3_checkbox_status" => $v['step3_field1_fieldset']['step3_checkbox_status'],
			":step3_checkbox_status_1" => $v['step3_field1_fieldset']['step3_checkbox_status_1'],
			":data_relation" => "step3",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
	}
	$form_state['redirect'] = 'esim-workshop/form/step4/' . $user->uid;
}
/*************************************** step 3 from ends ****************************/

/******* Step 4 form starts  - Includes form with observation table******************/

function esim_workshop_step4_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	// All the real form fields.
	$img_path = $base_url . '/' . drupal_get_path("module", "esim_workshop_form") . '/images';
	$form = array();
	$form['step4_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step4-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('esim_workshop_2019_form_step4_feedback');
	$query->fields('esim_workshop_2019_form_step4_feedback');
	$query->condition('uid', $user->uid);
	$form_step4_data = $query->execute();
	if ($form_step4_data)
	{
		if ($step4_data = $form_step4_data->fetchObject())
		{
			$form['step4_field1_fieldset']['step4_field1_question1'] = array(
				'#markup' => '<h4><strong>Task 2: Simulating the &lsquo;</strong><strong><em>Astable Multivibrator</em></strong><strong>&rsquo; circuit in eSim for various values of resistors and comparing its results with the output on PCB</strong></h4>
<p><span style="text-decoration: underline;"><span style="font-weight: 400;">Step 1:</span></span><span style="font-weight: 400;"> Simulate the schematic created in Task 1 using eSim for the first combination of R3 and R4 given in Table 1. Use the following settings for simulation:</span><br>
<em><span style="font-weight: 400;">Analysis type : Transient</span></em><br>
<em><span style="font-weight: 400;">Start time = 0 sec, Step = 10msec, Stop time = 12 sec</span></em></p>
<p><span style="text-decoration: underline;"><span style="font-weight: 400;">Step 2: </span></span><span style="font-weight: 400;">Count the number of positive peaks across LED1 in an interval of 12 seconds and enter your observations under the column &ldquo;Simulation results using eSim&rdquo; in Table 1</span></p>
<p><span style="text-decoration: underline;"><span style="font-weight: 400;">Step 3: </span></span><span style="font-weight: 400;">Count the number of positive peaks across LED2 in an interval of 12 seconds and enter your observations under the column &ldquo;Simulation results using eSim&rdquo; in Table 1</span></p>
<p><span style="text-decoration: underline;"><span style="font-weight: 400;">Step 4:</span></span><span style="font-weight: 400;"> Connect the resistors R3 and R4(used for simulation in Step 1) on the PCB in the required slots as shown in Figure 2.</span></p>
<center><img src="' . $img_path . '/resistors.svg" width="40%" height="40%"><br><span>Figure 2. Resistor connections</span></center>
<p><span style="text-decoration: underline;"><span style="font-weight: 400;">Step 5:</span></span><span style="font-weight: 400;"> Connect the 9V battery to its connector as shown in Figure 3.</span></p>
<center><img src="' . $img_path . '/9vbat.svg" width="40%" height="40%" align="middle"/><br><span>Figure 3. 9V Battery connection to its connector</span></center>
<p><span style="text-decoration: underline;"><span style="font-weight: 400;">Step 6:</span></span><span style="font-weight: 400;"> Connect the other end of the 9V Battery connector to the PCB as shown in Figure 4</span></p>
<center><img src="' . $img_path . '/bat-pcb.svg" width="40%" height="40%"><br><span>Figure 4. 9V Battery connection to PCB</span></center>
<p><span style="text-decoration: underline;"><span style="font-weight: 400;">Step 7:</span></span><span style="font-weight: 400;"> Verify your circuit connections as shown in Figure 5</span></p>
<center><img src="' . $img_path . '/done.svg" width="40%" height="40%"><br><span>Figure 5. Resistor connections</span></center>
<p><span style="text-decoration: underline;"><span style="font-weight: 400;">Step 8:</span></span><span style="font-weight: 400;"> Count the number of times LED1 blinks in an interval of 12 seconds and enter your observations under the column &ldquo;Physical observation over PCB&rdquo; in Table 1 </span></p>
<p><span style="text-decoration: underline;"><span style="font-weight: 400;">Step 9:</span></span><span style="font-weight: 400;"> Count the number of times LED2 blinks in an interval of 12 seconds and enter your observations under the column &ldquo;Physical observation over PCB&rdquo; in Table 1 </span></p>
<p><span style="text-decoration: underline;"><span style="font-weight: 400;">Step 10:</span></span><span style="font-weight: 400;"> Disconnect the 9 V battery from the PCB </span></p>
<p><span style="text-decoration: underline;"><span style="font-weight: 400;">Step 11:</span></span><span style="font-weight: 400;"> Repeat Step 1 to Step 10 for the remaining combinations of R3 and R4</span></p>',
				'#type' => 'item',
			);
		  	$form['step4_field1_fieldset']['observation_table_header_1'] = array(
				'#type' => 'item',
				'#prefix' => '<table><tr><td colspan="2">',
				'#markup' => '<strong><p>Resistor Values</p></strong>',
				'#suffix' => '</td>',

			);
			$form['step4_field1_fieldset']['observation_table_header_2'] = array(
				'#type' => 'item',
				'#prefix' => '<td colspan="2">',
				'#markup' => '<strong><p>Physical observation over PCB</p></strong>',
				'#suffix' => '</td>',

			);
			$form['step4_field1_fieldset']['observation_table_header_3'] = array(
				'#type' => 'item',
				'#prefix' => '<td colspan="2">',
				'#markup' => '<strong><p>Simulation results using eSim</p></strong>',
				'#suffix' => '</td></tr><tr>',

			);
			$form['step4_field1_fieldset']['observation_table_sub_header_1'] = array(
				'#type' => 'item',
				'#prefix' => '<td>',
				'#markup' => '<strong><p>R3</p></strong>',
				'#suffix' => '</td>',

			);
			$form['step4_field1_fieldset']['observation_table_sub_header_2'] = array(
				'#type' => 'item',
				'#prefix' => '<td>',
				'#markup' => '<strong><p>R4</p></strong>',
				'#suffix' => '</td>',

			);
			$form['step4_field1_fieldset']['observation_table_sub_header_3'] = array(
				'#type' => 'item',
				'#prefix' => '<td>',
				'#markup' => '<strong><p>Number of times LED1 blinks in 12 seconds</p></strong>',
				'#suffix' => '</td>',

			);
			$form['step4_field1_fieldset']['observation_table_sub_header_4'] = array(
				'#type' => 'item',
				'#prefix' => '<td>',
				'#markup' => '<strong><p>Number of times LED2 blinks in 12 seconds</p></strong>',
				'#suffix' => '</td>',

			);
			$form['step4_field1_fieldset']['observation_table_sub_header_5'] = array(
				'#type' => 'item',
				'#prefix' => '<td>',
				'#markup' => '<strong><p>Number of positive peaks across LED1 in 12 seconds</p></strong>',
				'#suffix' => '</td>',

			);
			$form['step4_field1_fieldset']['observation_table_sub_header_6'] = array(
				'#type' => 'item',
				'#prefix' => '<td>',
				'#markup' => '<strong><p>Number of positive peaks across LED2 in 12 seconds</p></strong>',
				'#suffix' => '</td></tr><tr>',

			);
			$form['step4_field1_fieldset']['observation_table_reading_row1_r3'] = array(
				'#type' => 'item',
				'#prefix' => '<td>',
				'#markup' => '<strong><p>68K</p></strong>',
				'#suffix' => '</td>',

			);
			$form['step4_field1_fieldset']['observation_table_reading_row1_r4'] = array(
				'#type' => 'item',
				'#prefix' => '<td>',
				'#markup' => '<strong><p>68K</p></strong>',
				'#suffix' => '</td>',

			);
			$form['step4_field1_fieldset']['row1_value1'] = array(
				'#type' => 'textfield',
				'#prefix' => '<td>',
				'#markup' => '<p></p>',
				'#suffix' => '</td>',
				'#default_value' => $step4_data->row1_value1,
			);
			$form['step4_field1_fieldset']['row1_value2'] = array(
				'#type' => 'textfield',
				'#prefix' => '<td>',
				'#markup' => '<p></p>',
				'#suffix' => '</td>',
				'#default_value' => $step4_data->row1_value2,
			);
			$form['step4_field1_fieldset']['row1_value3'] = array(
				'#type' => 'textfield',
				'#prefix' => '<td>',
				'#markup' => '<p></p>',
				'#suffix' => '</td>',
				'#default_value' => $step4_data->row1_value3,
			);
			$form['step4_field1_fieldset']['row1_value4'] = array(
				'#type' => 'textfield',
				'#prefix' => '<td>',
				'#markup' => '<p></p>',
				'#suffix' => '</td></tr><tr>',
				'#default_value' => $step4_data->row1_value4,
			);
			$form['step4_field1_fieldset']['observation_table_reading_row2_r3'] = array(
				'#type' => 'item',
				'#prefix' => '<td>',
				'#markup' => '<strong><p>150K</p></strong>',
				'#suffix' => '</td>',

			);
			$form['step4_field1_fieldset']['observation_table_reading_row2_r4'] = array(
				'#type' => 'item',
				'#prefix' => '<td>',
				'#markup' => '<strong><p>150K</p></strong>',
				'#suffix' => '</td>',

			);
			$form['step4_field1_fieldset']['row2_value1'] = array(
				'#type' => 'textfield',
				'#prefix' => '<td>',
				'#markup' => '<p></p>',
				'#suffix' => '</td>',
				'#default_value' => $step4_data->row2_value1,
			);
			$form['step4_field1_fieldset']['row2_value2'] = array(
				'#type' => 'textfield',
				'#prefix' => '<td>',
				'#markup' => '<p></p>',
				'#suffix' => '</td>',
				'#default_value' => $step4_data->row2_value2,
			);
			$form['step4_field1_fieldset']['row2_value3'] = array(
				'#type' => 'textfield',
				'#prefix' => '<td>',
				'#markup' => '<p></p>',
				'#suffix' => '</td>',
				'#default_value' => $step4_data->row2_value3,
			);
			$form['step4_field1_fieldset']['row2_value4'] = array(
				'#type' => 'textfield',
				'#prefix' => '<td>',
				
				'#suffix' => '</td></tr><tr>',
				'#default_value' => $step4_data->row2_value4,
			);
			$form['step4_field1_fieldset']['observation_table_reading_row3_r3'] = array(
				'#type' => 'item',
				'#prefix' => '<td>',
				'#markup' => '<strong><p>33K</p></strong>',
				'#suffix' => '</td>',

			);
			$form['step4_field1_fieldset']['observation_table_reading_row3_r4'] = array(
				'#type' => 'item',
				'#prefix' => '<td>',
				'#markup' => '<strong><p>33K</p></strong>',
				'#suffix' => '</td>',

			);
			$form['step4_field1_fieldset']['row3_value1'] = array(
				'#type' => 'textfield',
				'#prefix' => '<td>',
				'#markup' => '<p></p>',
				'#suffix' => '</td>',
				'#default_value' => $step4_data->row3_value1,
			);
			$form['step4_field1_fieldset']['row3_value2'] = array(
				'#type' => 'textfield',
				'#prefix' => '<td>',
				'#markup' => '<p></p>',
				'#suffix' => '</td>',
				'#default_value' => $step4_data->row3_value2,
			);
			$form['step4_field1_fieldset']['row3_value3'] = array(
				'#type' => 'textfield',
				'#prefix' => '<td>',
				'#markup' => '<p></p>',
				'#suffix' => '</td>',
				'#default_value' => $step4_data->row3_value3,
			);
			$form['step4_field1_fieldset']['row3_value4'] = array(
				'#type' => 'textfield',
				'#prefix' => '<td>',
				'#markup' => '<p></p>',
				'#suffix' => '</td></tr><tr>',
				'#default_value' => $step4_data->row3_value4,
			);
			$form['step4_field1_fieldset']['observation_table_reading_row4_r3'] = array(
				'#type' => 'item',
				'#prefix' => '<td>',
				'#markup' => '<strong><p>101K</p></strong>',
				'#suffix' => '</td>',

			);
			$form['step4_field1_fieldset']['observation_table_reading_row4_r4'] = array(
				'#type' => 'item',
				'#prefix' => '<td>',
				'#markup' => '<strong><p>33K</p></strong>',
				'#suffix' => '</td>',

			);
			$form['step4_field1_fieldset']['row4_value1'] = array(
				'#type' => 'textfield',
				'#prefix' => '<td>',
				'#markup' => '<p></p>',
				'#suffix' => '</td>',
				'#default_value' => $step4_data->row4_value1,
			);
			$form['step4_field1_fieldset']['row4_value2'] = array(
				'#type' => 'textfield',
				'#prefix' => '<td>',
				'#markup' => '<p></p>',
				'#suffix' => '</td>',
				'#default_value' => $step4_data->row4_value2,
			);
			$form['step4_field1_fieldset']['row4_value3'] = array(
				'#type' => 'textfield',
				'#prefix' => '<td>',
				'#markup' => '<p></p>',
				'#suffix' => '</td>',
				'#default_value' => $step4_data->row4_value3,
			);
			$form['step4_field1_fieldset']['row4_value4'] = array(
				'#type' => 'textfield',
				'#prefix' => '<td>',
				
				'#suffix' => '</td></tr><tr>',
				'#default_value' => $step4_data->row4_value4,
			);
			$form['step4_field1_fieldset']['observation_table_reading_row5_r3'] = array(
				'#type' => 'item',
				'#prefix' => '<td>',
				'#markup' => '<strong><p>183K</p></strong>',
				'#suffix' => '</td>',

			);
			$form['step4_field1_fieldset']['observation_table_reading_row5_r4'] = array(
				'#type' => 'item',
				'#prefix' => '<td>',
				'#markup' => '<strong><p>218K</p></strong>',
				'#suffix' => '</td>',

			);
			$form['step4_field1_fieldset']['row5_value1'] = array(
				'#type' => 'textfield',
				'#prefix' => '<td>',
				'#markup' => '<p></p>',
				'#suffix' => '</td>',
				'#default_value' => $step4_data->row5_value1,
			);
			$form['step4_field1_fieldset']['row5_value2'] = array(
				'#type' => 'textfield',
				'#prefix' => '<td>',
				'#markup' => '<p></p>',
				'#suffix' => '</td>',
				'#default_value' => $step4_data->row5_value2,
			);
			$form['step4_field1_fieldset']['row5_value3'] = array(
				'#type' => 'textfield',
				'#prefix' => '<td>',
				'#markup' => '<p></p>',
				'#suffix' => '</td>',
				'#default_value' => $step4_data->row5_value3,
			);
			$form['step4_field1_fieldset']['row5_value4'] = array(
				'#type' => 'textfield',
				'#prefix' => '<td>',
				'#markup' => '<p></p>',
				'#suffix' => '</td></tr><tr>',
				'#default_value' => $step4_data->row5_value4,
			);
			$form['step4_field1_fieldset']['observation_table_reading_row6_r3'] = array(
				'#type' => 'item',
				'#prefix' => '<td>',
				'#markup' => '<strong><p>218K</p></strong>',
				'#suffix' => '</td>',

			);
			$form['step4_field1_fieldset']['observation_table_reading_row6_r4'] = array(
				'#type' => 'item',
				'#prefix' => '<td>',
				'#markup' => '<strong><p>101K</p></strong>',
				'#suffix' => '</td>',

			);
			$form['step4_field1_fieldset']['row6_value1'] = array(
				'#type' => 'textfield',
				'#prefix' => '<td>',
				'#markup' => '<p></p>',
				'#suffix' => '</td>',
				'#default_value' => $step4_data->row6_value1,
			);
			$form['step4_field1_fieldset']['row6_value2'] = array(
				'#type' => 'textfield',
				'#prefix' => '<td>',
				'#markup' => '<p></p>',
				'#suffix' => '</td>',
				'#default_value' => $step4_data->row6_value2,
			);
			$form['step4_field1_fieldset']['row6_value3'] = array(
				'#type' => 'textfield',
				'#prefix' => '<td>',
				'#markup' => '<p></p>',
				'#suffix' => '</td>',
				'#default_value' => $step4_data->row6_value3,
			);
			$form['step4_field1_fieldset']['row6_value4'] = array(
				'#type' => 'textfield',
				'#prefix' => '<td>',
				
				'#suffix' => '</td></tr></table>',
				'#default_value' => $step4_data->row6_value4,
			);
			$form['step4_field1_fieldset']["step4_table_no"] = array(
				"#type" => "item",
				"#markup" => '<center><span>Table 1: Observation Table</span></center>'
			);
			$form["step4_page_id"] = array(
				"#type" => "hidden",
				"#value" => "step4"
			);
			$form['previous1'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'esim_workshop_step4_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'esim_workshop_step4_form_next_submit'
				)
			);
			$form['save_submit'] = array(
				'#type' => 'submit',
				'#value' => 'Save as draft',
				'#submit' => array(
					'esim_workshop_step4_form_save_submit'
				)
			);
		} //$step4_data = $form_step4_data->fetchObject()
		else
		{
			$form['step4_field1_fieldset']['step4_field1_question1'] = array(
				'#markup' => '<h4><strong>Task 2: Simulating the &lsquo;</strong><strong><em>Astable Multivibrator</em></strong><strong>&rsquo; circuit in eSim for various values of resistors and comparing its results with the output on PCB</strong></h4>
<p><span style="text-decoration: underline;"><span style="font-weight: 400;">Step 1:</span></span><span style="font-weight: 400;"> Simulate the schematic created in Task 1 using eSim for the first combination of R3 and R4 given in Table 1. Use the following settings for simulation:</span><br>
<em><span style="font-weight: 400;">Analysis type : Transient</span></em><br>
<em><span style="font-weight: 400;">Start time = 0 sec, Step = 10msec, Stop time = 12 sec</span></em></p>
<p><span style="text-decoration: underline;"><span style="font-weight: 400;">Step 2: </span></span><span style="font-weight: 400;">Count the number of positive peaks across LED1 in an interval of 12 seconds and enter your observations under the column &ldquo;Simulation results using eSim&rdquo; in Table 1</span></p>
<p><span style="text-decoration: underline;"><span style="font-weight: 400;">Step 3: </span></span><span style="font-weight: 400;">Count the number of positive peaks across LED2 in an interval of 12 seconds and enter your observations under the column &ldquo;Simulation results using eSim&rdquo; in Table 1</span></p>
<p><span style="text-decoration: underline;"><span style="font-weight: 400;">Step 4:</span></span><span style="font-weight: 400;"> Connect the resistors R3 and R4(used for simulation in Step 1) on the PCB in the required slots as shown in Figure 2.</span></p>
<center><img src="' . $img_path . '/resistors.svg" width="40%" height="40%"><br><span>Figure 2. Resistor connections</span></center>
<p><span style="text-decoration: underline;"><span style="font-weight: 400;">Step 5:</span></span><span style="font-weight: 400;"> Connect the 9V battery to its connector as shown in Figure 3.</span></p>
<center><img src="' . $img_path . '/9vbat.svg" width="40%" height="40%" align="middle"/><br><span>Figure 3. 9V Battery connection to its connector</span></center>
<p><span style="text-decoration: underline;"><span style="font-weight: 400;">Step 6:</span></span><span style="font-weight: 400;"> Connect the other end of the 9V Battery connector to the PCB as shown in Figure 4</span></p>
<center><img src="' . $img_path . '/bat-pcb.svg" width="40%" height="40%"><br><span>Figure 4. 9V Battery connection to PCB</span></center>
<p><span style="text-decoration: underline;"><span style="font-weight: 400;">Step 7:</span></span><span style="font-weight: 400;"> Verify your circuit connections as shown in Figure 5</span></p>
<center><img src="' . $img_path . '/done.svg" width="40%" height="40%"><br><span>Figure 5. Resistor connections</span></center>
<p><span style="text-decoration: underline;"><span style="font-weight: 400;">Step 8:</span></span><span style="font-weight: 400;"> Count the number of times LED1 blinks in an interval of 12 seconds and enter your observations under the column &ldquo;Physical observation over PCB&rdquo; in Table 1 </span></p>
<p><span style="text-decoration: underline;"><span style="font-weight: 400;">Step 9:</span></span><span style="font-weight: 400;"> Count the number of times LED2 blinks in an interval of 12 seconds and enter your observations under the column &ldquo;Physical observation over PCB&rdquo; in Table 1 </span></p>
<p><span style="text-decoration: underline;"><span style="font-weight: 400;">Step 10:</span></span><span style="font-weight: 400;"> Disconnect the 9 V battery from the PCB </span></p>
<p><span style="text-decoration: underline;"><span style="font-weight: 400;">Step 11:</span></span><span style="font-weight: 400;"> Repeat Step 1 to Step 10 for the remaining combinations of R3 and R4</span></p>',
				'#type' => 'item',
			);
			$form["step4_page_id"] = array(
				"#type" => "hidden",
				"#value" => "step4"
			);
			$form['step4_field1_fieldset']['observation_table_header_1'] = array(
				'#type' => 'item',
				'#prefix' => '<table><tr><td colspan="2">',
				'#markup' => '<strong><p>Resistor Values</p></strong>',
				'#suffix' => '</td>',

			);
			$form['step4_field1_fieldset']['observation_table_header_2'] = array(
				'#type' => 'item',
				'#prefix' => '<td colspan="2">',
				'#markup' => '<strong><p>Physical observation over PCB</p></strong>',
				'#suffix' => '</td>',

			);
			$form['step4_field1_fieldset']['observation_table_header_3'] = array(
				'#type' => 'item',
				'#prefix' => '<td colspan="2">',
				'#markup' => '<strong><p>Simulation results using eSim</p></strong>',
				'#suffix' => '</td></tr><tr>',

			);
			$form['step4_field1_fieldset']['observation_table_sub_header_1'] = array(
				'#type' => 'item',
				'#prefix' => '<td>',
				'#markup' => '<strong><p>R3</p></strong>',
				'#suffix' => '</td>',

			);
			$form['step4_field1_fieldset']['observation_table_sub_header_2'] = array(
				'#type' => 'item',
				'#prefix' => '<td>',
				'#markup' => '<strong><p>R4</p></strong>',
				'#suffix' => '</td>',

			);
			$form['step4_field1_fieldset']['observation_table_sub_header_3'] = array(
				'#type' => 'item',
				'#prefix' => '<td>',
				'#markup' => '<strong><p>Number of times LED1 blinks in 12 seconds</p></strong>',
				'#suffix' => '</td>',

			);
			$form['step4_field1_fieldset']['observation_table_sub_header_4'] = array(
				'#type' => 'item',
				'#prefix' => '<td>',
				'#markup' => '<strong><p>Number of times LED2 blinks in 12 seconds</p></strong>',
				'#suffix' => '</td>',

			);
			$form['step4_field1_fieldset']['observation_table_sub_header_5'] = array(
				'#type' => 'item',
				'#prefix' => '<td>',
				'#markup' => '<strong><p>Number of positive peaks across LED1 in 12 seconds</p></strong>',
				'#suffix' => '</td>',

			);
			$form['step4_field1_fieldset']['observation_table_sub_header_6'] = array(
				'#type' => 'item',
				'#prefix' => '<td>',
				'#markup' => '<strong><p>Number of positive peaks across LED2 in 12 seconds</p></strong>',
				'#suffix' => '</td></tr><tr>',

			);
			$form['step4_field1_fieldset']['observation_table_reading_row1_r3'] = array(
				'#type' => 'item',
				'#prefix' => '<td>',
				'#markup' => '<strong><p>68K</p></strong>',
				'#suffix' => '</td>',

			);
			$form['step4_field1_fieldset']['observation_table_reading_row1_r4'] = array(
				'#type' => 'item',
				'#prefix' => '<td>',
				'#markup' => '<strong><p>68K</p></strong>',
				'#suffix' => '</td>',

			);
			$form['step4_field1_fieldset']['row1_value1'] = array(
				'#type' => 'textfield',
				'#prefix' => '<td>',
				'#markup' => '<p></p>',
				'#suffix' => '</td>',
				'#default_value' => $step4_data->row1_value1,
			);
			$form['step4_field1_fieldset']['row1_value2'] = array(
				'#type' => 'textfield',
				'#prefix' => '<td>',
				'#markup' => '<p></p>',
				'#suffix' => '</td>',
				'#default_value' => $step4_data->row1_value2,
			);
			$form['step4_field1_fieldset']['row1_value3'] = array(
				'#type' => 'textfield',
				'#prefix' => '<td>',
				'#markup' => '<p></p>',
				'#suffix' => '</td>',
				'#default_value' => $step4_data->row1_value3,
			);
			$form['step4_field1_fieldset']['row1_value4'] = array(
				'#type' => 'textfield',
				'#prefix' => '<td>',
				'#markup' => '<p></p>',
				'#suffix' => '</td></tr><tr>',
				'#default_value' => $step4_data->row1_value4,
			);
			$form['step4_field1_fieldset']['observation_table_reading_row2_r3'] = array(
				'#type' => 'item',
				'#prefix' => '<td>',
				'#markup' => '<strong><p>150K</p></strong>',
				'#suffix' => '</td>',

			);
			$form['step4_field1_fieldset']['observation_table_reading_row2_r4'] = array(
				'#type' => 'item',
				'#prefix' => '<td>',
				'#markup' => '<strong><p>150K</p></strong>',
				'#suffix' => '</td>',

			);
			$form['step4_field1_fieldset']['row2_value1'] = array(
				'#type' => 'textfield',
				'#prefix' => '<td>',
				'#markup' => '<p></p>',
				'#suffix' => '</td>',
				'#default_value' => $step4_data->row2_value1,
			);
			$form['step4_field1_fieldset']['row2_value2'] = array(
				'#type' => 'textfield',
				'#prefix' => '<td>',
				'#markup' => '<p></p>',
				'#suffix' => '</td>',
				'#default_value' => $step4_data->row2_value2,
			);
			$form['step4_field1_fieldset']['row2_value3'] = array(
				'#type' => 'textfield',
				'#prefix' => '<td>',
				'#markup' => '<p></p>',
				'#suffix' => '</td>',
				'#default_value' => $step4_data->row2_value3,
			);
			$form['step4_field1_fieldset']['row2_value4'] = array(
				'#type' => 'textfield',
				'#prefix' => '<td>',
				
				'#suffix' => '</td></tr><tr>',
				'#default_value' => $step4_data->row2_value4,
			);
			$form['step4_field1_fieldset']['observation_table_reading_row3_r3'] = array(
				'#type' => 'item',
				'#prefix' => '<td>',
				'#markup' => '<strong><p>33K</p></strong>',
				'#suffix' => '</td>',

			);
			$form['step4_field1_fieldset']['observation_table_reading_row3_r4'] = array(
				'#type' => 'item',
				'#prefix' => '<td>',
				'#markup' => '<strong><p>33K</p></strong>',
				'#suffix' => '</td>',

			);
			$form['step4_field1_fieldset']['row3_value1'] = array(
				'#type' => 'textfield',
				'#prefix' => '<td>',
				'#markup' => '<p></p>',
				'#suffix' => '</td>',
				'#default_value' => $step4_data->row3_value1,
			);
			$form['step4_field1_fieldset']['row3_value2'] = array(
				'#type' => 'textfield',
				'#prefix' => '<td>',
				'#markup' => '<p></p>',
				'#suffix' => '</td>',
				'#default_value' => $step4_data->row3_value2,
			);
			$form['step4_field1_fieldset']['row3_value3'] = array(
				'#type' => 'textfield',
				'#prefix' => '<td>',
				'#markup' => '<p></p>',
				'#suffix' => '</td>',
				'#default_value' => $step4_data->row3_value3,
			);
			$form['step4_field1_fieldset']['row3_value4'] = array(
				'#type' => 'textfield',
				'#prefix' => '<td>',
				'#markup' => '<p></p>',
				'#suffix' => '</td></tr><tr>',
				'#default_value' => $step4_data->row3_value4,
			);
			$form['step4_field1_fieldset']['observation_table_reading_row4_r3'] = array(
				'#type' => 'item',
				'#prefix' => '<td>',
				'#markup' => '<strong><p>101K</p></strong>',
				'#suffix' => '</td>',

			);
			$form['step4_field1_fieldset']['observation_table_reading_row4_r4'] = array(
				'#type' => 'item',
				'#prefix' => '<td>',
				'#markup' => '<strong><p>33K</p></strong>',
				'#suffix' => '</td>',

			);
			$form['step4_field1_fieldset']['row4_value1'] = array(
				'#type' => 'textfield',
				'#prefix' => '<td>',
				'#markup' => '<p></p>',
				'#suffix' => '</td>',
				'#default_value' => $step4_data->row4_value1,
			);
			$form['step4_field1_fieldset']['row4_value2'] = array(
				'#type' => 'textfield',
				'#prefix' => '<td>',
				'#markup' => '<p></p>',
				'#suffix' => '</td>',
				'#default_value' => $step4_data->row4_value2,
			);
			$form['step4_field1_fieldset']['row4_value3'] = array(
				'#type' => 'textfield',
				'#prefix' => '<td>',
				'#markup' => '<p></p>',
				'#suffix' => '</td>',
				'#default_value' => $step4_data->row4_value3,
			);
			$form['step4_field1_fieldset']['row4_value4'] = array(
				'#type' => 'textfield',
				'#prefix' => '<td>',
				
				'#suffix' => '</td></tr><tr>',
				'#default_value' => $step4_data->row4_value4,
			);
			$form['step4_field1_fieldset']['observation_table_reading_row5_r3'] = array(
				'#type' => 'item',
				'#prefix' => '<td>',
				'#markup' => '<strong><p>183K</p></strong>',
				'#suffix' => '</td>',

			);
			$form['step4_field1_fieldset']['observation_table_reading_row5_r4'] = array(
				'#type' => 'item',
				'#prefix' => '<td>',
				'#markup' => '<strong><p>218K</p></strong>',
				'#suffix' => '</td>',

			);
			$form['step4_field1_fieldset']['row5_value1'] = array(
				'#type' => 'textfield',
				'#prefix' => '<td>',
				'#markup' => '<p></p>',
				'#suffix' => '</td>',
				'#default_value' => $step4_data->row5_value1,
			);
			$form['step4_field1_fieldset']['row5_value2'] = array(
				'#type' => 'textfield',
				'#prefix' => '<td>',
				'#markup' => '<p></p>',
				'#suffix' => '</td>',
				'#default_value' => $step4_data->row5_value2,
			);
			$form['step4_field1_fieldset']['row5_value3'] = array(
				'#type' => 'textfield',
				'#prefix' => '<td>',
				'#markup' => '<p></p>',
				'#suffix' => '</td>',
				'#default_value' => $step4_data->row5_value3,
			);
			$form['step4_field1_fieldset']['row5_value4'] = array(
				'#type' => 'textfield',
				'#prefix' => '<td>',
				'#markup' => '<p></p>',
				'#suffix' => '</td></tr><tr>',
				'#default_value' => $step4_data->row5_value4,
			);
			$form['step4_field1_fieldset']['observation_table_reading_row6_r3'] = array(
				'#type' => 'item',
				'#prefix' => '<td>',
				'#markup' => '<strong><p>218K</p></strong>',
				'#suffix' => '</td>',

			);
			$form['step4_field1_fieldset']['observation_table_reading_row6_r4'] = array(
				'#type' => 'item',
				'#prefix' => '<td>',
				'#markup' => '<strong><p>101K</p></strong>',
				'#suffix' => '</td>',

			);
			$form['step4_field1_fieldset']['row6_value1'] = array(
				'#type' => 'textfield',
				'#prefix' => '<td>',
				'#markup' => '<p></p>',
				'#suffix' => '</td>',
				'#default_value' => $step4_data->row6_value1,
			);
			$form['step4_field1_fieldset']['row6_value2'] = array(
				'#type' => 'textfield',
				'#prefix' => '<td>',
				'#markup' => '<p></p>',
				'#suffix' => '</td>',
				'#default_value' => $step4_data->row6_value2,
			);
			$form['step4_field1_fieldset']['row6_value3'] = array(
				'#type' => 'textfield',
				'#prefix' => '<td>',
				'#markup' => '<p></p>',
				'#suffix' => '</td>',
				'#default_value' => $step4_data->row6_value3,
			);
			$form['step4_field1_fieldset']['row6_value4'] = array(
				'#type' => 'textfield',
				'#prefix' => '<td>',
				
				'#suffix' => '</td></tr></table>',
				'#default_value' => $step4_data->row6_value4,
			);
			$form['step4_field1_fieldset']["step4_table_no"] = array(
				"#type" => "item",
				"#markup" => '<center><span>Table 1: Observation Table</span></center>'
			);
			$form['previous1'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'esim_workshop_step4_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'esim_workshop_step4_form_next_submit'
				)
			);
			$form['save_submit'] = array(
				'#type' => 'submit',
				'#value' => 'Save as draft',
				'#submit' => array(
					'esim_workshop_step4_form_save_submit'
				)
			);

		} //$step4_data = $form_step4_data->fetchObject()
	} //$form_step4_data
	return $form;
}
function esim_workshop_step4_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$query = db_select('esim_workshop_2019_form_step4_feedback');
	$query->fields('esim_workshop_2019_form_step4_feedback');
	$query->condition('uid', $user->uid);
	$form_step4_data = $query->execute()->fetchObject();
	//var_dump($form_step4_data);die;
	if ($form_step4_data)
	{
		$query = "UPDATE esim_workshop_2019_form_step4_feedback SET row1_value1=:row1_value1, row1_value2=:row1_value2,row1_value3=:row1_value3,row1_value4=:row1_value4,row2_value1=:row2_value1, row2_value2=:row2_value2,row2_value3=:row2_value3,row2_value4=:row2_value4,row3_value1=:row3_value1, row3_value2=:row3_value2,row3_value3=:row3_value3,row3_value4=:row3_value4,row4_value1=:row4_value1, row4_value2=:row4_value2,row4_value3=:row4_value3,row4_value4=:row4_value4,row5_value1=:row5_value1, row5_value2=:row5_value2,row5_value3=:row5_value3,row5_value4=:row5_value4,row6_value1=:row6_value1, row6_value2=:row6_value2,row6_value3=:row6_value3,row6_value4=:row6_value4, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":row1_value1" => $v['step4_field1_fieldset']['row1_value1'],
			":row1_value2" => $v['step4_field1_fieldset']['row1_value2'],
			":row1_value3" => $v['step4_field1_fieldset']['row1_value3'],
			":row1_value4" => $v['step4_field1_fieldset']['row1_value4'],
			":row2_value1" => $v['step4_field1_fieldset']['row2_value1'],
			":row2_value2" => $v['step4_field1_fieldset']['row2_value2'],
			":row2_value3" => $v['step4_field1_fieldset']['row2_value3'],
			":row2_value4" => $v['step4_field1_fieldset']['row2_value4'],
			":row3_value1" => $v['step4_field1_fieldset']['row3_value1'],
			":row3_value2" => $v['step4_field1_fieldset']['row3_value2'],
			":row3_value3" => $v['step4_field1_fieldset']['row3_value3'],
			":row3_value4" => $v['step4_field1_fieldset']['row3_value4'],
			":row4_value1" => $v['step4_field1_fieldset']['row4_value1'],
			":row4_value2" => $v['step4_field1_fieldset']['row4_value2'],
			":row4_value3" => $v['step4_field1_fieldset']['row4_value3'],
			":row4_value4" => $v['step4_field1_fieldset']['row4_value4'],
			":row5_value1" => $v['step4_field1_fieldset']['row5_value1'],
			":row5_value2" => $v['step4_field1_fieldset']['row5_value2'],
			":row5_value3" => $v['step4_field1_fieldset']['row5_value3'],
			":row5_value4" => $v['step4_field1_fieldset']['row5_value4'],
			":row6_value1" => $v['step4_field1_fieldset']['row6_value1'],
			":row6_value2" => $v['step4_field1_fieldset']['row6_value2'],
			":row6_value3" => $v['step4_field1_fieldset']['row6_value3'],
			":row6_value4" => $v['step4_field1_fieldset']['row6_value4'],
			":data_relation" => "step4"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		 //$i = 0; $i <= $v['step4_field1_fieldset']["step4_comment_count"]; $i++
	} //$form_step4_data
	else
	{
		$query = "INSERT INTO {esim_workshop_2019_form_step4_feedback} (uid, row1_value1, row1_value2 ,row1_value3,row1_value4, row2_value1, row2_value2 , row2_value3,row2_value4,row3_value1, row3_value2 ,row3_value3,row3_value4, row4_value1, row4_value2 , row4_value3,row4_value4,row5_value1, row5_value2 ,row5_value3,row5_value4, row6_value1, row6_value2 , row6_value3,row6_value4,data_relation,inserted_on) VALUES (:uid,:row1_value1, :row1_value2 , :row1_value3,:row1_value4, :row2_value1, :row2_value2 , :row2_value3,:row2_value4,
:row3_value1, :row3_value2 , :row3_value3,:row3_value4, :row4_value1, :row4_value2 , :row4_value3,:row4_value4,
:row5_value1, :row5_value2 , :row5_value3,:row5_value4, :row6_value1, :row6_value2 , :row6_value3,:row6_value4, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":row1_value1" => $v['step4_field1_fieldset']['row1_value1'],
			":row1_value2" => $v['step4_field1_fieldset']['row1_value2'],
			":row1_value3" => $v['step4_field1_fieldset']['row1_value3'],
			":row1_value4" => $v['step4_field1_fieldset']['row1_value4'],
			":row2_value1" => $v['step4_field1_fieldset']['row2_value1'],
			":row2_value2" => $v['step4_field1_fieldset']['row2_value2'],
			":row2_value3" => $v['step4_field1_fieldset']['row2_value3'],
			":row2_value4" => $v['step4_field1_fieldset']['row2_value4'],
			":row3_value1" => $v['step4_field1_fieldset']['row3_value1'],
			":row3_value2" => $v['step4_field1_fieldset']['row3_value2'],
			":row3_value3" => $v['step4_field1_fieldset']['row3_value3'],
			":row3_value4" => $v['step4_field1_fieldset']['row3_value4'],
			":row4_value1" => $v['step4_field1_fieldset']['row4_value1'],
			":row4_value2" => $v['step4_field1_fieldset']['row4_value2'],
			":row4_value3" => $v['step4_field1_fieldset']['row4_value3'],
			":row4_value4" => $v['step4_field1_fieldset']['row4_value4'],
			":row5_value1" => $v['step4_field1_fieldset']['row5_value1'],
			":row5_value2" => $v['step4_field1_fieldset']['row5_value2'],
			":row5_value3" => $v['step4_field1_fieldset']['row5_value3'],
			":row5_value4" => $v['step4_field1_fieldset']['row5_value4'],
			":row6_value1" => $v['step4_field1_fieldset']['row6_value1'],
			":row6_value2" => $v['step4_field1_fieldset']['row6_value2'],
			":row6_value3" => $v['step4_field1_fieldset']['row6_value3'],
			":row6_value4" => $v['step4_field1_fieldset']['row6_value4'],
			":data_relation" => "step4",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
	}
	$form_state['redirect'] = 'esim-workshop/form/step3/' . $user->uid;
}
function esim_workshop_step4_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$query = db_select('esim_workshop_2019_form_step4_feedback');
	$query->fields('esim_workshop_2019_form_step4_feedback');
	$query->condition('uid', $user->uid);
	$form_step4_data = $query->execute()->fetchObject();
	//var_dump($form_step4_data);die;
	if ($form_step4_data)
	{
		$query = "UPDATE esim_workshop_2019_form_step4_feedback SET row1_value1=:row1_value1, row1_value2=:row1_value2,row1_value3=:row1_value3,row1_value4=:row1_value4,row2_value1=:row2_value1, row2_value2=:row2_value2,row2_value3=:row2_value3,row2_value4=:row2_value4,row3_value1=:row3_value1, row3_value2=:row3_value2,row3_value3=:row3_value3,row3_value4=:row3_value4,row4_value1=:row4_value1, row4_value2=:row4_value2,row4_value3=:row4_value3,row4_value4=:row4_value4,row5_value1=:row5_value1, row5_value2=:row5_value2,row5_value3=:row5_value3,row5_value4=:row5_value4,row6_value1=:row6_value1, row6_value2=:row6_value2,row6_value3=:row6_value3,row6_value4=:row6_value4, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":row1_value1" => $v['step4_field1_fieldset']['row1_value1'],
			":row1_value2" => $v['step4_field1_fieldset']['row1_value2'],
			":row1_value3" => $v['step4_field1_fieldset']['row1_value3'],
			":row1_value4" => $v['step4_field1_fieldset']['row1_value4'],
			":row2_value1" => $v['step4_field1_fieldset']['row2_value1'],
			":row2_value2" => $v['step4_field1_fieldset']['row2_value2'],
			":row2_value3" => $v['step4_field1_fieldset']['row2_value3'],
			":row2_value4" => $v['step4_field1_fieldset']['row2_value4'],
			":row3_value1" => $v['step4_field1_fieldset']['row3_value1'],
			":row3_value2" => $v['step4_field1_fieldset']['row3_value2'],
			":row3_value3" => $v['step4_field1_fieldset']['row3_value3'],
			":row3_value4" => $v['step4_field1_fieldset']['row3_value4'],
			":row4_value1" => $v['step4_field1_fieldset']['row4_value1'],
			":row4_value2" => $v['step4_field1_fieldset']['row4_value2'],
			":row4_value3" => $v['step4_field1_fieldset']['row4_value3'],
			":row4_value4" => $v['step4_field1_fieldset']['row4_value4'],
			":row5_value1" => $v['step4_field1_fieldset']['row5_value1'],
			":row5_value2" => $v['step4_field1_fieldset']['row5_value2'],
			":row5_value3" => $v['step4_field1_fieldset']['row5_value3'],
			":row5_value4" => $v['step4_field1_fieldset']['row5_value4'],
			":row6_value1" => $v['step4_field1_fieldset']['row6_value1'],
			":row6_value2" => $v['step4_field1_fieldset']['row6_value2'],
			":row6_value3" => $v['step4_field1_fieldset']['row6_value3'],
			":row6_value4" => $v['step4_field1_fieldset']['row6_value4'],
			":data_relation" => "step4"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
	} //$form_step4_data
	else
	{
		$query = "INSERT INTO {esim_workshop_2019_form_step4_feedback} (uid, row1_value1, row1_value2 ,row1_value3,row1_value4, row2_value1, row2_value2 , row2_value3,row2_value4,row3_value1, row3_value2 ,row3_value3,row3_value4, row4_value1, row4_value2 , row4_value3,row4_value4,row5_value1, row5_value2 ,row5_value3,row5_value4, row6_value1, row6_value2 , row6_value3,row6_value4,data_relation,inserted_on) VALUES (:uid,:row1_value1, :row1_value2 , :row1_value3,:row1_value4, :row2_value1, :row2_value2 , :row2_value3,:row2_value4,
:row3_value1, :row3_value2 , :row3_value3,:row3_value4, :row4_value1, :row4_value2 , :row4_value3,:row4_value4,
:row5_value1, :row5_value2 , :row5_value3,:row5_value4, :row6_value1, :row6_value2 , :row6_value3,:row6_value4, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":row1_value1" => $v['step4_field1_fieldset']['row1_value1'],
			":row1_value2" => $v['step4_field1_fieldset']['row1_value2'],
			":row1_value3" => $v['step4_field1_fieldset']['row1_value3'],
			":row1_value4" => $v['step4_field1_fieldset']['row1_value4'],
			":row2_value1" => $v['step4_field1_fieldset']['row2_value1'],
			":row2_value2" => $v['step4_field1_fieldset']['row2_value2'],
			":row2_value3" => $v['step4_field1_fieldset']['row2_value3'],
			":row2_value4" => $v['step4_field1_fieldset']['row2_value4'],
			":row3_value1" => $v['step4_field1_fieldset']['row3_value1'],
			":row3_value2" => $v['step4_field1_fieldset']['row3_value2'],
			":row3_value3" => $v['step4_field1_fieldset']['row3_value3'],
			":row3_value4" => $v['step4_field1_fieldset']['row3_value4'],
			":row4_value1" => $v['step4_field1_fieldset']['row4_value1'],
			":row4_value2" => $v['step4_field1_fieldset']['row4_value2'],
			":row4_value3" => $v['step4_field1_fieldset']['row4_value3'],
			":row4_value4" => $v['step4_field1_fieldset']['row4_value4'],
			":row5_value1" => $v['step4_field1_fieldset']['row5_value1'],
			":row5_value2" => $v['step4_field1_fieldset']['row5_value2'],
			":row5_value3" => $v['step4_field1_fieldset']['row5_value3'],
			":row5_value4" => $v['step4_field1_fieldset']['row5_value4'],
			":row6_value1" => $v['step4_field1_fieldset']['row6_value1'],
			":row6_value2" => $v['step4_field1_fieldset']['row6_value2'],
			":row6_value3" => $v['step4_field1_fieldset']['row6_value3'],
			":row6_value4" => $v['step4_field1_fieldset']['row6_value4'],
			":data_relation" => "step4",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		}
	$form_state['redirect'] = 'esim-workshop/form/step5/' . $user->uid;
}

/*****************function to save the observation details as a draft**************/

function esim_workshop_step4_form_save_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$query = "SELECT * FROM esim_workshop_2019_form_step4_feedback WHERE uid=:uid";
	$args = array(
		":uid" => $user->uid
	);
	$form_step4_data = db_query($query, $args);
	$form_step4_data_count = $form_step4_data->rowCount();
	$form_step4_data = $form_step4_data->fetchObject();
	if ($form_step4_data_count != 0)
	{
		$query = "UPDATE esim_workshop_2019_form_step4_feedback SET row1_value1=:row1_value1, row1_value2=:row1_value2,row1_value3=:row1_value3,row1_value4=:row1_value4,row2_value1=:row2_value1, row2_value2=:row2_value2,row2_value3=:row2_value3,row2_value4=:row2_value4,row3_value1=:row3_value1, row3_value2=:row3_value2,row3_value3=:row3_value3,row3_value4=:row3_value4,row4_value1=:row4_value1, row4_value2=:row4_value2,row4_value3=:row4_value3,row4_value4=:row4_value4,row5_value1=:row5_value1, row5_value2=:row5_value2,row5_value3=:row5_value3,row5_value4=:row5_value4,row6_value1=:row6_value1, row6_value2=:row6_value2,row6_value3=:row6_value3,row6_value4=:row6_value4, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":row1_value1" => $v['step4_field1_fieldset']['row1_value1'],
			":row1_value2" => $v['step4_field1_fieldset']['row1_value2'],
			":row1_value3" => $v['step4_field1_fieldset']['row1_value3'],
			":row1_value4" => $v['step4_field1_fieldset']['row1_value4'],
			":row2_value1" => $v['step4_field1_fieldset']['row2_value1'],
			":row2_value2" => $v['step4_field1_fieldset']['row2_value2'],
			":row2_value3" => $v['step4_field1_fieldset']['row2_value3'],
			":row2_value4" => $v['step4_field1_fieldset']['row2_value4'],
			":row3_value1" => $v['step4_field1_fieldset']['row3_value1'],
			":row3_value2" => $v['step4_field1_fieldset']['row3_value2'],
			":row3_value3" => $v['step4_field1_fieldset']['row3_value3'],
			":row3_value4" => $v['step4_field1_fieldset']['row3_value4'],
			":row4_value1" => $v['step4_field1_fieldset']['row4_value1'],
			":row4_value2" => $v['step4_field1_fieldset']['row4_value2'],
			":row4_value3" => $v['step4_field1_fieldset']['row4_value3'],
			":row4_value4" => $v['step4_field1_fieldset']['row4_value4'],
			":row5_value1" => $v['step4_field1_fieldset']['row5_value1'],
			":row5_value2" => $v['step4_field1_fieldset']['row5_value2'],
			":row5_value3" => $v['step4_field1_fieldset']['row5_value3'],
			":row5_value4" => $v['step4_field1_fieldset']['row5_value4'],
			":row6_value1" => $v['step4_field1_fieldset']['row6_value1'],
			":row6_value2" => $v['step4_field1_fieldset']['row6_value2'],
			":row6_value3" => $v['step4_field1_fieldset']['row6_value3'],
			":row6_value4" => $v['step4_field1_fieldset']['row6_value4'],
			":data_relation" => "step4"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE esim_workshop_2019_form_step1_feedback SET submitted=:submitted WHERE uid=:uid ";
		$args_finish = array(
			":uid" => $user->uid,
			":submitted" => 0
		);
		/* storing the row id in $result */
		$resultfinish = db_query($queryfinish, $args_finish, array(
			'return' => Database::RETURN_INSERT_ID
		));
	} //$form_step4_data
	else
	{
		$query = "INSERT INTO {esim_workshop_2019_form_step4_feedback} (uid, row1_value1, row1_value2 ,row1_value3,row1_value4, row2_value1, row2_value2 , row2_value3,row2_value4,row3_value1, row3_value2 ,row3_value3,row3_value4, row4_value1, row4_value2 , row4_value3,row4_value4,row5_value1, row5_value2 ,row5_value3,row5_value4, row6_value1, row6_value2 , row6_value3,row6_value4,data_relation,inserted_on) VALUES (:uid,:row1_value1, :row1_value2 , :row1_value3,:row1_value4, :row2_value1, :row2_value2 , :row2_value3,:row2_value4,
:row3_value1, :row3_value2 , :row3_value3,:row3_value4, :row4_value1, :row4_value2 , :row4_value3,:row4_value4,
:row5_value1, :row5_value2 , :row5_value3,:row5_value4, :row6_value1, :row6_value2 , :row6_value3,:row6_value4, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":row1_value1" => $v['step4_field1_fieldset']['row1_value1'],
			":row1_value2" => $v['step4_field1_fieldset']['row1_value2'],
			":row1_value3" => $v['step4_field1_fieldset']['row1_value3'],
			":row1_value4" => $v['step4_field1_fieldset']['row1_value4'],
			":row2_value1" => $v['step4_field1_fieldset']['row2_value1'],
			":row2_value2" => $v['step4_field1_fieldset']['row2_value2'],
			":row2_value3" => $v['step4_field1_fieldset']['row2_value3'],
			":row2_value4" => $v['step4_field1_fieldset']['row2_value4'],
			":row3_value1" => $v['step4_field1_fieldset']['row3_value1'],
			":row3_value2" => $v['step4_field1_fieldset']['row3_value2'],
			":row3_value3" => $v['step4_field1_fieldset']['row3_value3'],
			":row3_value4" => $v['step4_field1_fieldset']['row3_value4'],
			":row4_value1" => $v['step4_field1_fieldset']['row4_value1'],
			":row4_value2" => $v['step4_field1_fieldset']['row4_value2'],
			":row4_value3" => $v['step4_field1_fieldset']['row4_value3'],
			":row4_value4" => $v['step4_field1_fieldset']['row4_value4'],
			":row5_value1" => $v['step4_field1_fieldset']['row5_value1'],
			":row5_value2" => $v['step4_field1_fieldset']['row5_value2'],
			":row5_value3" => $v['step4_field1_fieldset']['row5_value3'],
			":row5_value4" => $v['step4_field1_fieldset']['row5_value4'],
			":row6_value1" => $v['step4_field1_fieldset']['row6_value1'],
			":row6_value2" => $v['step4_field1_fieldset']['row6_value2'],
			":row6_value3" => $v['step4_field1_fieldset']['row6_value3'],
			":row6_value4" => $v['step4_field1_fieldset']['row6_value4'],
			":data_relation" => "step4",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE esim_workshop_2019_form_step1_feedback SET submitted=:submitted WHERE uid=:uid ";
		$args_finish = array(
			":uid" => $user->uid,
			":submitted" => 0
		);
		/* storing the row id in $result */
		$resultfinish = db_query($queryfinish, $args_finish, array(
			'return' => Database::RETURN_INSERT_ID
		));
	}
	drupal_set_message(t('You have successfully saved your observations. You can anytime edit the observation table.'), 'status');
	drupal_goto('esim-workshop/form/step4');
}

/********************************** step 4 form ends ****************************/

/***** step 5 form starts - PCB designing of ‘Astable Multivibrator’ circuit  *********/
function esim_workshop_step5_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	$img_path = $base_url . '/' . drupal_get_path("module", "esim_workshop_form") . '/images';
	$form = array();
	$form['step5_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step5-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('esim_workshop_2019_form_step5_feedback');
	$query->fields('esim_workshop_2019_form_step5_feedback');
	$query->condition('uid', $user->uid);
	$form_step5_data = $query->execute();
	if ($form_step5_data)
	{
		if ($step5_data = $form_step5_data->fetchObject())
		{
			$form['step5_field1_fieldset']['step5_field1_question1'] = array(
				'#markup' => '<h4><strong>Task 4: PCB designing of ‘Astable Multivibrator’ circuit</strong></h4>
<p><span style="font-weight: 400;"> Create PCB Layout and Generate Gerber files of the ‘Astable Multivibrator’ circuit designed by you in Task 1 using eSim. You may refer to the corresponding Spoken Tutorials, if required.</span></p>',
				'#type' => 'item',
			);
			$form['step5_field1_fieldset']['step5_checkbox_status_1'] = array(
				'#type' => 'checkbox',
	            '#title' => t('I could successfully create the PCB layout'),
	            '#required' => TRUE,
	            '#default_value' => $step5_data->step5_checkbox_status_1,
        	);
			$form['step5_field1_fieldset']['step5_checkbox_status_2'] = array(
				'#type' => 'checkbox',
	            '#title' => t('I could successfully generate the gerber files'),
	            '#default_value' => $step5_data->step5_checkbox_status_2,
        	);
  			$form["step5_page_id"] = array(
				"#type" => "hidden",
				"#value" => "step5"
			);
			$form['previous1'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'esim_workshop_step5_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'esim_workshop_step5_form_next_submit'
				)
			);
		} //$step5_data = $form_step5_data->fetchObject()
		else
		{
			$form['step5_field1_fieldset']['step5_field1_question1'] = array(
				'#markup' => '<h4><strong>Task 4: PCB designing of ‘Astable Multivibrator’ circuit</strong></h4>
<p><span style="font-weight: 400;"> Create PCB Layout and Generate Gerber files of the ‘Astable Multivibrator’ circuit designed by you in Task 1 using eSim. You may refer to the corresponding Spoken Tutorials, if required.</span></p>',
				'#type' => 'item',
			);
			$form['step5_field1_fieldset']['step5_checkbox_status_1'] = array(
				'#type' => 'checkbox',
	            '#title' => t('I could successfully create the PCB layout'),
	            '#required' => TRUE,
        	);
			$form['step5_field1_fieldset']['step5_checkbox_status_2'] = array(
				'#type' => 'checkbox',
	            '#title' => t('I could successfully generate the gerber files'),
	          );
			$form["step5_page_id"] = array(
				"#type" => "hidden",
				"#value" => "step5"
			);
			$form['previous1'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'esim_workshop_step5_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'esim_workshop_step5_form_next_submit'
				)
			);
		} //$step5_data = $form_step5_data->fetchObject()
	} //$form_step5_data
	return $form;
}
function esim_workshop_step5_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$query = db_select('esim_workshop_2019_form_step5_feedback');
	$query->fields('esim_workshop_2019_form_step5_feedback');
	$query->condition('uid', $user->uid);
	$form_step5_data = $query->execute()->fetchObject();
	if ($form_step5_data)
	{
		$query = "UPDATE esim_workshop_2019_form_step5_feedback SET step5_checkbox_status_1=:step5_checkbox_status_1, step5_checkbox_status_2=:step5_checkbox_status_2, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":step5_checkbox_status_1" => $v['step5_field1_fieldset']['step5_checkbox_status_1'],
			":step5_checkbox_status_2" => $v['step5_field1_fieldset']['step5_checkbox_status_2'],
			":data_relation" => "step5"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
	} //$form_step5_data
	else
	{
		$query = "INSERT INTO {esim_workshop_2019_form_step5_feedback} (uid, step5_checkbox_status_1, step5_checkbox_status_2, data_relation,inserted_on) VALUES (:uid,:step5_checkbox_status_1, :step5_checkbox_status_2, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":step5_checkbox_status_1" => $v['step5_field1_fieldset']['step5_checkbox_status_1'],
			":step5_checkbox_status_2" => $v['step5_field1_fieldset']['step5_checkbox_status_2'],
			":data_relation" => "step5",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
	}
	$form_state['redirect'] = 'esim-workshop/form/step4/' . $user->uid;
}
function esim_workshop_step5_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$query = db_select('esim_workshop_2019_form_step5_feedback');
	$query->fields('esim_workshop_2019_form_step5_feedback');
	$query->condition('uid', $user->uid);
	$form_step5_data = $query->execute()->fetchObject();
	if ($form_step5_data)
	{
		$query = "UPDATE esim_workshop_2019_form_step5_feedback SET step5_checkbox_status_1=:step5_checkbox_status_1,step5_checkbox_status_2=:step5_checkbox_status_2, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":step5_checkbox_status_1" => $v['step5_field1_fieldset']['step5_checkbox_status_1'],
			":step5_checkbox_status_2" => $v['step5_field1_fieldset']['step5_checkbox_status_2'],
			":data_relation" => "step5"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
	} //$form_step5_data
	else
	{
		$query = "INSERT INTO {esim_workshop_2019_form_step5_feedback} (uid, step5_checkbox_status_1,step5_checkbox_status_2, data_relation,inserted_on) VALUES (:uid,:step5_checkbox_status_1,:step5_checkbox_status_2 ,:data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":step5_checkbox_status_1" => $v['step5_field1_fieldset']['step5_checkbox_status_1'],
			":step5_checkbox_status_2" => $v['step5_field1_fieldset']['step5_checkbox_status_2'],
			":data_relation" => "step5",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
	}
	$form_state['redirect'] = 'esim-workshop/form/step6/' . $user->uid;
}
/*************************************** step 5 form ends ****************************/

/***** step 6 form starts - PCB designing of ‘Astable Multivibrator’ circuit  *********/
function esim_workshop_step6_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	$img_path = $base_url . '/' . drupal_get_path("module", "esim_workshop_form") . '/images';
	$form = array();
	$form['step6_field2_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step6-field2-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$form['step6_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step6-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	
	$query = db_select('esim_workshop_2019_form_step6_feedback');
	$query->fields('esim_workshop_2019_form_step6_feedback');
	$query->condition('uid', $user->uid);
	$form_step6_data = $query->execute();
	if ($form_step6_data)
	{
		if ($step6_data = $form_step6_data->fetchObject())
		{
			$project_file = 	$step6_data->project_file;
			$project_file_id = $step6_data->id;
			$form['step6_field1_fieldset']['step6_field1_question1'] = array(
				'#markup' => '<h4><strong>Task 6: Documentation of your work</strong></h4>
<span style="font-weight: 400;">You need to answer the following questions. They will help you think more about circuit designing.</span>',
				'#type' => 'item',
			);
			$form['step6_field1_fieldset']['step6_field1_question2'] = array(
				'#type' => 'radios',
				'#title' => t('1. Is your design DRC compliant?'),
				'#options' => array(
					'Yes' => 'Yes',
					'No' => 'No'
				),
				'#default_value' => $step6_data->step6_field1_question2,
				'#required' => TRUE,
			);
			$form['step6_field1_fieldset']['step6_field1_question3'] = array(
				'#type' => 'radios',
				'#title' => t('2. Have you used THT or SMD footprints for the component in your design?'),
				'#options' => array(
					'THT' => 'THT',
					'SMD' => 'SMD'
				),
				'#default_value' => $step6_data->step6_field1_question3,
				'#required' => TRUE
			);
			$form['step6_field1_fieldset']['step6_field1_question3_justify'] = array(
				'#type' => 'textarea',
				'#title' => t('Justify your choice'),
				'#default_value' => $step6_data->step6_field1_question3_justify,
				'#required' => TRUE
			);
			$form['step6_field1_fieldset']['step6_field1_question4'] = array(
				'#type' => 'radios',
				'#title' => t('3. Have you used a Ground plane for connecting the GND terminals?'),
				'#options' => array(
					'Yes' => 'Yes',
					'No' => 'No'
				),
				'#default_value' => $step6_data->step6_field1_question4,
				'#required' => TRUE
			);
			$form['step6_field1_fieldset']['step6_field1_question5'] = array(
				'#type' => 'radios',
				'#title' => t('4. Are the components in your design placed symmetrically?'),
				'#options' => array(
					'Yes' => 'Yes',
					'No' => 'No'
				),
				'#default_value' => $step6_data->step6_field1_question5,
				'#required' => TRUE
			);
			$form['step6_field1_fieldset']['step6_field1_question6'] = array(
				'#type' => 'radios',
				'#title' => t('5. How many layers of PCB you have used while designing?'),
				'#options' => array(
					'1' => '1',
					'2' => '2'
				),
				'#default_value' => $step6_data->step6_field1_question6,
				'#required' => TRUE
			);
			$form['step6_field1_fieldset']['step6_field1_question7'] = array(
				'#type' => 'radios',
				'#title' => t('6. Have you made use of silkscreens to place text, dimensions and additional information?'),
				'#options' => array(
					'Yes' => 'Yes',
					'No' => 'No'
				),
				'#default_value' => $step6_data->step6_field1_question7,
				'#required' => TRUE
			);
			$form['step6_field1_fieldset']['step6_field1_question8'] = array(
				'#type' => 'textfield',
				'#title' => t('7. What was the board dimension (in millimeters) after placing and routing the components'),
				'#description' => t('<span style="color:red;">Input answer as width X height. For example 15mm x 15mm</span>'),
				'#default_value' => $step6_data->step6_field1_question8,
				'#required' => TRUE
			);
			$form['step6_field1_fieldset']['step6_field1_question9'] = array(
				'#type' => 'textarea',
				'#title' => t('8. Compare and note down the design differences such as orientation and placement of components, track width etc. between the PCB that you have designed and the one that was provided to you'),
				'#default_value' => $step6_data->step6_field1_question9,
				'#required' => TRUE
			);
			$form['step6_field1_fieldset']['step6_field1_question10'] = array(
				'#type' => 'radios',
				'#title' => t('9. Were you able to understand the importance of Gerber file and its generation?'),
				'#options' => array(
					'Yes' => 'Yes',
					'No' => 'No'
				),
				'#default_value' => $step6_data->step6_field1_question10,
				'#required' => TRUE
			);
			$form['step6_field1_fieldset']['step6_field1_question11'] = array(
				'#type' => 'textarea',
				'#title' => t('10. Do you think your design was better in certain ways than the PCB that was provided to you?'),
				'#default_value' => $step6_data->step6_field1_question11,
				'#required' => TRUE
			);	
			if ($project_file)
		    {
		    	$form['step6_field2_fieldset']['intro_para'] = array(
		    		'#type'=> 'item',
		    		'#markup' => t('<h4><strong>Task 5: Uploading your project files</strong></h4>Compress your project folder in ZIP format. Rename the compressed zip file to your RC number. For example, 2345.zip. Upload the zip file by clicking on the upload button below.'),
		    	);
		        $form['step6_field2_fieldset']['project_file']['existing'] = array(
		            '#type' => 'item',
		            '#title' => t('Existing Project file'),
		            '#markup' => l($step6_data->project_file, 'esim-workshop/download/project-file/' . $step6_data->uid),
		            '#suffix' => '<p>Click on the link to download the project file</p>'
		        );
		        $form['step6_field2_fieldset']['project_file']['upload_project_file'] = array(
		            '#type' => 'file',
		            '#title' => t('Upload your project file'),
		            '#size' => 48,
		            '#description' => t("Upload a new project file above if you want to replace the existing file. Leave blank if you want to keep using the existing file. <br />") . t('<span style="color:red">Allowed file extensions : ') . variable_get('workshop_project_file_extensions', '') . t('</span><br><span style="color:red">Kindly click on the upload your project file button below to save your file.</span>')
		        );
		        $form['step6_field2_fieldset']['project_file']['project_file_id'] = array(
		            '#type' => 'hidden',
		            '#default_value' => $project_file_id
		        );
		        $form['step6_field2_fieldset']['save_project_file'] = array(
				'#type' => 'submit',
				'#value' => 'Upload your project file',
				'#submit' => array(
					'esim_workshop_step6_form_upload_project_file_submit'
				),
				'#limit_validation_errors' => array()
				);
		      }
		    else
		      {
		      	$form['step6_field2_fieldset']['intro_para'] = array(
		    		'#type'=> 'item',
		    		'#markup' => t('<h4><strong>Task 5: Uploading your project files</strong></h4>Compress your project folder in ZIP format. Rename the compressed zip file to your RC number. For example, 2345.zip. Upload the zip file by clicking on the upload button below.'),
		    	);
		        $form['step6_field2_fieldset']['project_file']['upload_project_file'] = array(
		            '#type' => 'file',
		            '#title' => t('Upload your project file'),
		            '#size' => 48,
		            '#required' => TRUE,
		            '#description' => t("Upload a new project file above if you want to replace the existing file. Leave blank if you want to keep using the existing file. <br />") . t('<span style="color:red">Allowed file extensions : ') . variable_get('workshop_project_file_extensions', '') . t('</span><br><span style="color:red">Kindly click on the upload your project file button below to save your file.</span>')
		        );
		        $form['step6_field2_fieldset']['save_project_file'] = array(
				'#type' => 'submit',
				'#value' => 'Upload your project file',
				'#submit' => array(
					'esim_workshop_step6_form_upload_project_file_submit'
				),
				'#limit_validation_errors' => array()
			);
		    }
  			$form["step6_page_id"] = array(
				"#type" => "hidden",
				"#value" => "step6"
			);
			$form['previous1'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'esim_workshop_step6_form_previous_submit'
				)
			);
			$form['step6_field1_fieldset']['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save',
				'#submit' => array(
					'esim_workshop_step6_form_next_submit'
				)
			);
			$form['submit_and_finish'] = array(
				'#type' => 'submit',
				'#value' => 'Submit and fInish',
				'#submit' => array(
					'esim_workshop_step6_form_submit_finally'
				)
			);
		} //$step6_data = $form_step6_data->fetchObject()
		else
		{
			$form['step6_field1_fieldset']['step6_field1_question1'] = array(
				'#markup' => '<h4><strong>Task 6: Documentation of your work</strong></h4>
<span style="font-weight: 400;">You need to answer the following questions. They will help you think more about circuit designing.</span>',
				'#type' => 'item',
			);
			$form['step6_field1_fieldset']['step6_field1_question2'] = array(
				'#type' => 'radios',
				'#title' => t('1. Is your design DRC compliant?'),
				'#options' => array(
					'Yes' => 'Yes',
					'No' => 'No'
				),
				'#required' => TRUE
			);
			$form['step6_field1_fieldset']['step6_field1_question3'] = array(
				'#type' => 'radios',
				'#title' => t('2. Have you used THT or SMD footprints for the component in your design?'),
				'#options' => array(
					'THT' => 'THT',
					'SMD' => 'SMD'
				),
				'#required' => TRUE
			);
			$form['step6_field1_fieldset']['step6_field1_question3_justify'] = array(
				'#type' => 'textarea',
				'#title' => t('Justify your choice'),
				'#required' => TRUE
			);
			$form['step6_field1_fieldset']['step6_field1_question4'] = array(
				'#type' => 'radios',
				'#title' => t('3. Have you used a Ground plane for connecting the GND terminals?'),
				'#options' => array(
					'Yes' => 'Yes',
					'No' => 'No'
				),
				'#required' => TRUE
			);
			$form['step6_field1_fieldset']['step6_field1_question5'] = array(
				'#type' => 'radios',
				'#title' => t('4. Are the components in your design placed symmetrically?'),
				'#options' => array(
					'Yes' => 'Yes',
					'No' => 'No'
				),
				'#required' => TRUE
			);
			$form['step6_field1_fieldset']['step6_field1_question6'] = array(
				'#type' => 'radios',
				'#title' => t('5. How many layers of PCB you have used while designing?'),
				'#options' => array(
					'1' => '1',
					'2' => '2'
				),
				'#required' => TRUE
			);
			$form['step6_field1_fieldset']['step6_field1_question7'] = array(
				'#type' => 'radios',
				'#title' => t('6. Have you made use of silkscreens to place text, dimensions and additional information?'),
				'#options' => array(
					'Yes' => 'Yes',
					'No' => 'No'
				),
				'#required' => TRUE
			);
			$form['step6_field1_fieldset']['step6_field1_question8'] = array(
				'#type' => 'textfield',
				'#title' => t('7. What was the board dimension (in millimeters) after placing and routing the components'),
				'#description' => t('<span style="color:red;">Input answer as width X height. For example 15mm x 15mm</span>'),
				'#required' => TRUE
			);
			$form['step6_field1_fieldset']['step6_field1_question9'] = array(
				'#type' => 'textarea',
				'#title' => t('8. Compare and note down the design differences such as orientation and placement of components, track width etc. between the PCB that you have designed and the one that was provided to you'),
				'#required' => TRUE
			);
			$form['step6_field1_fieldset']['step6_field1_question10'] = array(
				'#type' => 'radios',
				'#title' => t('9. Were you able to understand the importance of Gerber file and its generation?'),
				'#options' => array(
					'Yes' => 'Yes',
					'No' => 'No'
				),
				'#required' => TRUE
			);
			$form['step6_field1_fieldset']['step6_field1_question11'] = array(
				'#type' => 'textarea',
				'#title' => t('10. Do you think your design was better in certain ways than the PCB that was provided to you?'),
				'#required' => TRUE
			);
			if ($project_file)
		    {
		    	$form['step6_field2_fieldset']['intro_para'] = array(
		    		'#type'=> 'item',
		    		'#markup' => t('<h4><strong>Task 5: Uploading your project files</strong></h4>Compress your project folder in ZIP format. Rename the compressed zip file to your RC number. For example, 2345.zip. Upload the zip file by clicking on the upload button below.'),
		    	);
		        $form['step6_field2_fieldset']['project_file']['existing'] = array(
		            '#type' => 'item',
		            '#title' => t('Existing Project file'),
		            '#markup' => l($step6_data->project_file, 'esim-workshop/download/project-file/' . $step6_data->uid),
		             '#suffix' => '<p>Click on the link to download the project file</p>'
		        );
		        $form['step6_field2_fieldset']['project_file']['upload_project_file'] = array(
		            '#type' => 'file',
		            '#title' => t('Upload your project file'),
		            '#size' => 48,
		           '#description' => t("Upload a new project file above if you want to replace the existing file. Leave blank if you want to keep using the existing file. <br />") . t('<span style="color:red">Allowed file extensions : ') . variable_get('workshop_project_file_extensions', '') . t('</span><br><span style="color:red">Kindly click on the upload your project file button below to save your file.</span>')
		        );
		        $form['step6_field2_fieldset']['project_file']['project_file_id'] = array(
		            '#type' => 'hidden',
		            '#default_value' => $project_file_id
		        );
		        $form['step6_field2_fieldset']['save_project_file'] = array(
				'#type' => 'submit',
				'#value' => 'Upload your project file',
				'#submit' => array(
					'esim_workshop_step6_form_upload_project_file_submit'
				),
				'#limit_validation_errors' => array()
				);
		      }
		    else
		      {
		      	$form['step6_field2_fieldset']['intro_para'] = array(
		    		'#type'=> 'item',
		    		'#markup' => t('<h4><strong>Task 5: Uploading your project files</strong></h4>Compress your project folder in ZIP format. Rename the compressed zip file to your RC number. For example, 2345.zip. Upload the zip file by clicking on the upload button below.'),
		    	);
		        $form['step6_field2_fieldset']['project_file']['upload_project_file'] = array(
		            '#type' => 'file',
		            '#title' => t('Upload your project file'),
		            '#size' => 48,
		            '#required' => TRUE,
		            '#description' => t("Upload a new project file above if you want to replace the existing file. Leave blank if you want to keep using the existing file. <br />") . t('<span style="color:red">Allowed file extensions : ') . variable_get('workshop_project_file_extensions', '') . t('</span><br><span style="color:red">Kindly click on the upload your project file button below to save your file.</span>')
		        );
		        $form['step6_field2_fieldset']['save_project_file'] = array(
				'#type' => 'submit',
				'#value' => 'Upload your project file',
				'#submit' => array(
					'esim_workshop_step6_form_upload_project_file_submit'
				),
				'#limit_validation_errors' => array()
			);
		    }
			$form["step6_page_id"] = array(
				"#type" => "hidden",
				"#value" => "step6"
			);
			$form['previous1'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'esim_workshop_step6_form_previous_submit'
				)
			);
			$form['step6_field1_fieldset']['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save',
				'#submit' => array(
					'esim_workshop_step6_form_next_submit'
				)
			);
			$form['submit_and_finish'] = array(
				'#type' => 'submit',
				'#value' => 'Submit and fInish',
				'#submit' => array(
					'esim_workshop_step6_form_submit_finally'
				)
			);
		} //$step6_data = $form_step6_data->fetchObject()
	} //$form_step6_data
	return $form;
}
function esim_workshop_step6_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$query = db_select('esim_workshop_2019_form_step6_feedback');
	$query->fields('esim_workshop_2019_form_step6_feedback');
	$query->condition('uid', $user->uid);
	$form_step6_data = $query->execute()->fetchObject();
	if ($form_step6_data)
	{
		$query = "UPDATE esim_workshop_2019_form_step6_feedback SET step6_field1_question2=:step6_field1_question2,step6_field1_question3 = :step6_field1_question3, step6_field1_question3_justify=:step6_field1_question3_justify,step6_field1_question4 = :step6_field1_question4,step6_field1_question5= :step6_field1_question5,step6_field1_question6 = :step6_field1_question6,
			step6_field1_question7=:step6_field1_question7,step6_field1_question8 = :step6_field1_question8, step6_field1_question9=:step6_field1_question9,step6_field1_question10 = :step6_field1_question10, step6_field1_question11 = :step6_field1_question11 WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":step6_field1_question2" => $v['step6_field1_fieldset']['step6_field1_question2'],
			":step6_field1_question3" => $v['step6_field1_fieldset']['step6_field1_question3'],
			":step6_field1_question3_justify" => $v['step6_field1_fieldset']['step6_field1_question3_justify'],
			":step6_field1_question4" => $v['step6_field1_fieldset']['step6_field1_question4'],
			":step6_field1_question5" => $v['step6_field1_fieldset']['step6_field1_question5'],
			":step6_field1_question6" => $v['step6_field1_fieldset']['step6_field1_question6'],
			":step6_field1_question7" => $v['step6_field1_fieldset']['step6_field1_question7'],
			":step6_field1_question8" => $v['step6_field1_fieldset']['step6_field1_question8'],
			":step6_field1_question9" => $v['step6_field1_fieldset']['step6_field1_question9'],
			":step6_field1_question10" => $v['step6_field1_fieldset']['step6_field1_question10'],
			":step6_field1_question11" => $v['step6_field1_fieldset']['step6_field1_question11'],
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
	} //$form_step6_data
	else
	{
		$query = "INSERT INTO {esim_workshop_2019_form_step6_feedback} 
		(uid, step6_field1_question2,step6_field1_question3, step6_field1_question3_justify,step6_field1_question4,step6_field1_question5,
		step6_field1_question6,step6_field1_question7,step6_field1_question8,step6_field1_question9,step6_field1_question10,
		step6_field1_question11, data_relation,inserted_on) 
		VALUES 
		(:uid,:step6_field1_question2,:step6_field1_question3,:step6_field1_question3_justify,:step6_field1_question4,:step6_field1_question5
		,:step6_field1_question6,:step6_field1_question7,:step6_field1_question8,:step6_field1_question9
		,:step6_field1_question10,:step6_field1_question11, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":step6_field1_question2" => $v['step6_field1_fieldset']['step6_field1_question2'],
			":step6_field1_question3" => $v['step6_field1_fieldset']['step6_field1_question3'],
			":step6_field1_question3_justify" => $v['step6_field1_fieldset']['step6_field1_question3_justify'],
			":step6_field1_question4" => $v['step6_field1_fieldset']['step6_field1_question4'],
			":step6_field1_question5" => $v['step6_field1_fieldset']['step6_field1_question5'],
			":step6_field1_question6" => $v['step6_field1_fieldset']['step6_field1_question6'],
			":step6_field1_question7" => $v['step6_field1_fieldset']['step6_field1_question7'],
			":step6_field1_question8" => $v['step6_field1_fieldset']['step6_field1_question8'],
			":step6_field1_question9" => $v['step6_field1_fieldset']['step6_field1_question9'],
			":step6_field1_question10" => $v['step6_field1_fieldset']['step6_field1_question10'],
			":step6_field1_question11" => $v['step6_field1_fieldset']['step6_field1_question11'],
			":data_relation" => "step6",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
	}
	$form_state['redirect'] = 'esim-workshop/form/step5/' . $user->uid;
}
function esim_workshop_step6_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$query = db_select('esim_workshop_2019_form_step6_feedback');
	$query->fields('esim_workshop_2019_form_step6_feedback');
	$query->condition('uid', $user->uid);
	$form_step6_data = $query->execute()->fetchObject();
	if ($form_step6_data)
	{
		$query = "UPDATE esim_workshop_2019_form_step6_feedback SET step6_field1_question2=:step6_field1_question2,step6_field1_question3 = :step6_field1_question3, step6_field1_question3_justify=:step6_field1_question3_justify,step6_field1_question4 = :step6_field1_question4,step6_field1_question5= :step6_field1_question5,step6_field1_question6 = :step6_field1_question6,
			step6_field1_question7=:step6_field1_question7,step6_field1_question8 = :step6_field1_question8, step6_field1_question9=:step6_field1_question9,step6_field1_question10 = :step6_field1_question10, step6_field1_question11 = :step6_field1_question11 WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":step6_field1_question2" => $v['step6_field1_fieldset']['step6_field1_question2'],
			":step6_field1_question3" => $v['step6_field1_fieldset']['step6_field1_question3'],
			":step6_field1_question3_justify" => $v['step6_field1_fieldset']['step6_field1_question3_justify'],
			":step6_field1_question4" => $v['step6_field1_fieldset']['step6_field1_question4'],
			":step6_field1_question5" => $v['step6_field1_fieldset']['step6_field1_question5'],
			":step6_field1_question6" => $v['step6_field1_fieldset']['step6_field1_question6'],
			":step6_field1_question7" => $v['step6_field1_fieldset']['step6_field1_question7'],
			":step6_field1_question8" => $v['step6_field1_fieldset']['step6_field1_question8'],
			":step6_field1_question9" => $v['step6_field1_fieldset']['step6_field1_question9'],
			":step6_field1_question10" => $v['step6_field1_fieldset']['step6_field1_question10'],
			":step6_field1_question11" => $v['step6_field1_fieldset']['step6_field1_question11'],
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
	} //$form_step6_data
	else
	{
		$query = "INSERT INTO {esim_workshop_2019_form_step6_feedback} 
		(uid, step6_field1_question2,step6_field1_question3, step6_field1_question3_justify,step6_field1_question4,step6_field1_question5,
		step6_field1_question6,step6_field1_question7,step6_field1_question8,step6_field1_question9,step6_field1_question10,
		step6_field1_question11, data_relation,inserted_on) 
		VALUES 
		(:uid,:step6_field1_question2,:step6_field1_question3,:step6_field1_question3_justify,:step6_field1_question4,:step6_field1_question5
		,:step6_field1_question6,:step6_field1_question7,:step6_field1_question8,:step6_field1_question9
		,:step6_field1_question10,:step6_field1_question11, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":step6_field1_question2" => $v['step6_field1_fieldset']['step6_field1_question2'],
			":step6_field1_question3" => $v['step6_field1_fieldset']['step6_field1_question3'],
			":step6_field1_question3_justify" => $v['step6_field1_fieldset']['step6_field1_question3_justify'],
			":step6_field1_question4" => $v['step6_field1_fieldset']['step6_field1_question4'],
			":step6_field1_question5" => $v['step6_field1_fieldset']['step6_field1_question5'],
			":step6_field1_question6" => $v['step6_field1_fieldset']['step6_field1_question6'],
			":step6_field1_question7" => $v['step6_field1_fieldset']['step6_field1_question7'],
			":step6_field1_question8" => $v['step6_field1_fieldset']['step6_field1_question8'],
			":step6_field1_question9" => $v['step6_field1_fieldset']['step6_field1_question9'],
			":step6_field1_question10" => $v['step6_field1_fieldset']['step6_field1_question10'],
			":step6_field1_question11" => $v['step6_field1_fieldset']['step6_field1_question11'],
			":data_relation" => "step6",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
	}
	drupal_set_message(t('You have successfully saved your documentation form. You can edit your documentation form at anytime.'), 'status');
	$form_state['redirect'] = 'esim-workshop/form/step6/' . $user->uid;
}

function esim_workshop_step6_form_upload_project_file_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$query = "SELECT * FROM esim_workshop_2019_form_step6_feedback WHERE uid=:uid";
	$args = array(
		":uid" => $user->uid
	);
	 if (isset($_FILES['files']))
	{
		/* check if atleast one source or result file is uploaded 
		if (!($_FILES['files']['name']['upload_project_file']))
			form_set_error('samplefilepath', t('Please upload file with circuit diagram.'));
		/* check for valid filename extensions */
		foreach ($_FILES['files']['name'] as $file_form_name => $file_name)
		{
			if ($file_name)
			{
				
				if ($_FILES['files']['size'][$file_form_name] <= 0)
					form_set_error('project_file', t('File size cannot be zero.'));
			} //$file_name
		} //$_FILES['files']['name'] as $file_form_name => $file_name
	}
	$form_step6_data = db_query($query, $args);
	$form_step6_data_count = $form_step6_data->rowCount();
	$form_step6_data = $form_step6_data->fetchObject();
	$existing_filepath = $form_step6_data->filepath;
	//var_dump($existing_filepath);die;
	if ($form_step6_data_count != 0)
	{
		$root_path = esim_workshop_file_path();
		$dest_path = $user->uid;
		if (!is_dir($root_path . $dest_path))
			mkdir($root_path . $dest_path);
	/* uploading files */
		foreach ($_FILES['files']['name'] as $file_form_name => $file_name)
		{
		if ($file_name)
		{
			/* checking file type */
			//$file_type = 'S';
			$allowed_extensions_str = variable_get('workshop_project_file_extensions', '');
			$allowed_extensions = explode(',', $allowed_extensions_str);
			$fnames = explode('.', strtolower($_FILES['files']['name'][$file_form_name]));
			$temp_extension = end($fnames);
			if (!in_array($temp_extension, $allowed_extensions))
			{
				form_set_error('project_file', t('Only file with ' . $allowed_extensions_str . ' extensions can be uploaded.'));
			}
			elseif (!workshop_project_file_check_valid_filename($_FILES['files']['name'][$file_form_name])) {
					form_set_error('project_file', t('Invalid file name specified. The file name should be your RC number. For example, 2345.zip.'));
			}
			else
			{
			if (file_exists($root_path . $dest_path . '/' . $_FILES['files']['name'][$file_form_name]))
			{
				move_uploaded_file($_FILES['files']['tmp_name'][$file_form_name], $root_path . $dest_path . '/' . $_FILES['files']['name'][$file_form_name]);
				 drupal_set_message(t("File replaced", array('!filename' => $_FILES['files']['name'][$file_form_name])), 'status');
				 drupal_goto('esim-workshop/form/step6/' . $user->uid);
				//unlink($root_path . $dest_path . $_FILES['files']['name'][$file_form_name]);
			} //file_exists($root_path . $dest_path . $_FILES['files']['name'][$file_form_name])/* uploading file */
			else if (move_uploaded_file($_FILES['files']['tmp_name'][$file_form_name], $root_path . $dest_path . '/' . $_FILES['files']['name'][$file_form_name]))
			{
				//var_dump($root_path . $dest_path . '/' .  $form_step6_data->project_file);die;
				unlink($root_path . $dest_path . '/' .  $form_step6_data->project_file);
				$query = "UPDATE {esim_workshop_2019_form_step6_feedback} SET project_file = :project_file, filepath = :filepath,filemime = :filemime, filesize = :filesize, filetype = :filesize, file_timestamp = :file_timestamp WHERE uid = :uid";
				$args = array(
					":project_file" => $_FILES['files']['name'][$file_form_name],
                    ":filepath" => $dest_path . '/' . $_FILES['files']['name'][$file_form_name],
                    ":filemime" => 'application/zip',
                    ":filesize" => $_FILES['files']['size'][$file_form_name],
                    ":filetype" => 'project file',
                    ":file_timestamp" => time(),
                    ":uid" => $user->uid
				);
				
				$updateresult = db_query($query, $args);
				//var_dump($args);die;
				drupal_set_message($file_name . ' uploaded successfully. You have successfully saved your project file. You can resubmit/modify your project file at anytime.', 'status');
				drupal_goto('esim-workshop/form/step6/' . $user->uid);
			} //move_uploaded_file($_FILES['files']['tmp_name'][$file_form_name], $root_path . $dest_path . $_FILES['files']['name'][$file_form_name])
			else
			{
				drupal_set_message('Error uploading file : ' . $dest_path . '/' . $file_name, 'error');
			}
		}
		} //$file_name
	}

	} //$form_step6_data
	else
	{
		$root_path = esim_workshop_file_path();
		$dest_path = $user->uid;
    	if (!is_dir($root_path . $dest_path))
        	mkdir($root_path . $dest_path);
       //var_dump($root_path . $dest_path);die;
        foreach ($_FILES['files']['name'] as $file_form_name => $file_name)
      {
        if ($file_name)
          {
          	$allowed_extensions_str = variable_get('workshop_project_file_extensions', '');
			$allowed_extensions = explode(',', $allowed_extensions_str);
			$fnames = explode('.', strtolower($_FILES['files']['name'][$file_form_name]));
			$temp_extension = end($fnames);
			if (!in_array($temp_extension, $allowed_extensions))
			{
				form_set_error('project_file', t('Only file with ' . $allowed_extensions_str . ' extensions can be uploaded.'));
			}
			elseif (!workshop_project_file_check_valid_filename($_FILES['files']['name'][$file_form_name])) {
					form_set_error('project_file', t('Invalid file name specified. The file name should be your RC number. For example, 2345.zip.'));
			}
			else{
		if (file_exists($root_path . $dest_path . $_FILES['files']['name'][$file_form_name]))
            {
            	move_uploaded_file($_FILES['files']['tmp_name'][$file_form_name], $root_path . $dest_path . '/' . $_FILES['files']['name'][$file_form_name]);
                drupal_set_message(t("Error uploading file. File !filename already exists.", array(
                            '!filename' => $_FILES['files']['name'][$file_form_name]
                )), 'error');
                return;
            }
            //var_dump($root_path . $dest_path . '/' . $_FILES['files']['name'][$file_form_name]);die;
                    /* uploading file */
        else if (move_uploaded_file($_FILES['files']['tmp_name'][$file_form_name], $root_path . $dest_path . '/' . $_FILES['files']['name'][$file_form_name]))
            {
                $query = "INSERT INTO {esim_workshop_2019_form_step6_feedback} ( uid,project_file, filepath,filemime, filesize, filetype, file_timestamp,data_relation)
                  VALUES (:uid,:project_file ,:filepath,:filemime, :filesize, :filetype, :file_timestamp, :data_relation)";
                $args = array(
                	":uid" => $user->uid,
                    ":project_file" => $_FILES['files']['name'][$file_form_name],
                    ":filepath" => $dest_path . '/' . $_FILES['files']['name'][$file_form_name],
                    ":filemime" => 'application/zip',
                    ":filesize" => $_FILES['files']['size'][$file_form_name],
                    ":filetype" => 'project file',
                    ":file_timestamp" => time(),
                    ":data_relation" => "step6"
                );
                $result = db_query($query, $args, array(
                            'return' => Database::RETURN_INSERT_ID
                    ));
                drupal_set_message($file_name . ' uploaded successfully.', 'status');
        	}
        else
            {
            	drupal_set_message('Error uploading file : ' . $dest_path . '/' . $file_name, 'error');
            }
        
	drupal_set_message(t('You have successfully saved your project file. You can resubmit/modify your project file at anytime.'), 'status');
	drupal_goto('esim-workshop/form/step6/' . $user->uid);
	}
}
}
	}
}
function esim_workshop_step6_form_submit_finally($form, &$form_state, $no_js_use = FALSE)
{
	drupal_set_message(t('You have successfully completed the form.'), 'status');
	drupal_goto('esim-workshop/form');
}
/*************************************** step 6 form ends ****************************/