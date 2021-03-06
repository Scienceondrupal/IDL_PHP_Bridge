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


#ifndef PHP_IDLPHP_H
#define PHP_IDLPHP_H 1

#define PHP_IDLPHP_VERSION "1.0"
#define PHP_IDLPHP_EXTNAME "hello"

PHP_FUNCTION(idlphp);

extern zend_module_entry idlphp_entry;
#define phpext_hello_ptr &idlphp_module_entry

#endif
