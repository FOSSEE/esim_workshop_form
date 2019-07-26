<?php
function esim_download_project_file()
{
	$uid = arg(3);
	$root_path = esim_workshop_file_path();
	$query = db_select('esim_workshop_2019_form_step6_feedback');
	$query->fields('esim_workshop_2019_form_step6_feedback');
	$query->condition('uid', $uid);
	$query->range(0, 1);
	$result = $query->execute();
	$uploaded_project_file = $result->fetchObject();
	$workshop_project_file = $uploaded_project_file->filepath;
			ob_clean();
            header("Pragma: public");
            header("Expires: 0");
            header("Cache-Control: must-revalidate, post-check=0, pre-check=0");
            header("Cache-Control: public");
            header("Content-Description: File Transfer");
            header('Content-Type: application/zip');
            header('Content-disposition: attachment; filename="' . $root_path . '/' . $workshop_project_file . '"');
            header('Content-Length: ' . filesize($root_path . '/' . $workshop_project_file));
            header("Content-Transfer-Encoding: binary");
            header('Expires: 0');
            header('Pragma: no-cache');
            readfile($root_path . '/' . $workshop_project_file);
	       // var_dump(readfile($root_path . $circuit_simulation_upload_file->directory_name . $circuit_simulation_upload_file->samplefilepath));
	        ob_end_flush();
           	ob_clean();
           	flush();
}