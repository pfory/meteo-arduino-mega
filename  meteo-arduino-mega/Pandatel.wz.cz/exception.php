<?php 


/**
 * Xively Exception
 * @author Daniel Boorn - daniel.boorn@gmail.com
 * @copyright LogMeIn, Inc.
 * @license BSD 3-Clause, http://opensource.org/licenses/BSD-3-Clause
 * @namespace Xively
 * @note Class uses a JSON api_path.js file that defines the API resources (endpoints and paths). 
*/


namespace Xively;

class Exception extends \Exception{
	
	public $response;
	public $extra;
		
	public function __construct($message, $code = 0, $response=null, $extra=null, $previous = null) {
		parent::__construct($message, $code, $previous);
		$this->response = $response;
		$this->extra = $extra;
	}
	
	public function getResponse(){
		return $this->response;
	}
	
	public function getExtra(){
		return $this->extra;
	}
	
}

