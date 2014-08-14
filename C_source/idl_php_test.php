<?php
/*
 * IDL_PHP_Bridge Copyright (C) 2014  New Media Studio, Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY * without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
  echo "begin!\n";
  
  echo "\nTesting idlphp_setmainstringvar...\n";
  $res = idlphp_setmainstringvar("my_string_var","my test string.");
  var_dump( "res = " . $res );
  $res = idlphp_executestr("help, my_string_var");  
  $var = idlphp_getmainvariable("my_string_var");  
  var_dump( "var = " . $var );
  
  echo "\nTesting idlphp_setmainlongvar...\n";
  $res = idlphp_setmainlongvar("my_long_var", 999);
  var_dump( "res = " . $res );
  $res = idlphp_executestr("help, my_long_var");  
  $var = idlphp_getmainvariable("my_long_var");  
  var_dump( "var = " . $var );
  
  echo "\nTesting idlphp_setmaindoublevar...\n";
  $res = idlphp_setmaindoublevar("my_double_var", 999.0);
  var_dump( "res = " . $res );
  $res = idlphp_executestr("help, my_double_var");  
  $var = idlphp_getmainvariable("my_double_var");  
  var_dump( "var = " . $var );
  

  echo "\nTesting idlphp_setmainstringarray...\n";
  $res = idlphp_executestr("my_string_array=['Hello','IDL','PHP','!']");
  //$arr = array("Hello", "IDL", "PHP!");
  //$res = idlphp_setmainstringarray("my_string_array", $arr);
  var_dump( "res = " . $res );
  $res = idlphp_executestr("help, my_string_array");  
  $var = idlphp_getmainvariable("my_string_array");  
  var_dump( "var = " . $var );

  echo "\nTesting idlphp_setmainlongarray...\n";
  //$arr = array(2, 6, 11, 33, 1, 7);
  //$res = idlphp_setmainlongarray("my_long_array", $arr);
  $res = idlphp_executestr("my_long_array=[2, 6, 11, 33, 1, 7]");
  var_dump( "res = " . $res );
  $res = idlphp_executestr("help, my_long_array");  
  $var = idlphp_getmainvariable("my_long_array");  
  var_dump( "var = " . $var );
  
  echo "\nTesting idlphp_setmaindoublearray...\n";
  $arr = array(222.2, 6.66, 11, 33.33, 1.5, 7.7);
  $res = idlphp_setmaindoublearray("my_dbl_array", $arr);
  var_dump( "res = " . $res );
  $res = idlphp_executestr("help, my_dbl_array");  
  $var = idlphp_getmainvariable("my_dbl_array");  
  var_dump( "var = " . $var );
  
  echo "\nfini!";

?>
