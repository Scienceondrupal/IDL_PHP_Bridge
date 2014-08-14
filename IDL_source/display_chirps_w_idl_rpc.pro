;+
; :Description:
;    Display the CHIRPS data set via IDL Remote Procedure Call (RPC) from the
;    Drupal IDL_PHP module.
;
;    The DISPLAY_CHIRPS_W_IDL_RPC proceedure generates images of the CHIRPS 
;    precipitation data set and returns a URL to the image.
;
;  :Site specific parameters:
;    data_dir:    path to the data directory
;    url_path:    URL path to the ESE jobs directory
;    drupal_dir:  local path to the ESE jobs directory
;
; :Keywords:
;    year:      data year
;    month:     data month
;    clut:      color look up table
;    mode:      mode to operate in, monthly, average or anomaly
;
; :History:
;   2014/01/20: Created by Martin Landsfeld, The New Media Studio
;
; :Todo:
;
;-


; IDL_PHP_Bridge Copyright (C) 2014  New Media Studio, Inc.
;
; This program is free software: you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation, either version 3 of the License, or
; (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program.  If not, see <http://www.gnu.org/licenses/>.


FUNCTION display_chirps_w_idl_rpc, year, month, clut, mode
  
  ;------ Site specific paths -----------------------------------------------
  data_dir = '/Users/Tnms/Projects/NSODC/data/chirps_subset/'
  means_dir = '/Users/Tnms/Projects/NSODC/data/chirps_subset/means/'
  drupal_dir = '/Applications/MAMP/htdocs/TNMS/sites/default/files/
  url_path = 'http://localhost:8888/TNMS/sites/default/files/'
  ;-----------------------------------------------------------------------------------

  xsize = 720
  ysize = 400
  precp_min = 0
  precp_max = 600
  bg_color = [200,200,200]
  clut_orig = clut
  clut_lut = [0, 1, 49, 3, 53, 13, 38, 70]
  
  
  ;restore mask
  restore, data_dir + 'chirps_sub_landmask.50N50S.p05Deg.tif.sav'
  print, ''
  print, 'Reading mask: global_landmask.50N50S.tif'
  print, ''

  tick_vals = [0, 200, 400, 600]
  months = ['January', 'February', 'March', 'April', 'May', 'June', 'July', 'August', 'September', 'October', 'November', 'December']
  
  if mode eq 0 or mode eq 2 then begin
    file_name = string('chirps.', year, '.', month, '.tif', form='(a, i4, a, i02, a)')
    
    data_file = data_dir+file_name+'.sav'
    print, 'Restoring data file: ', data_file
    print, ''
    ;restore chirps data file
    restore, data_file
    date = string(year, month, form='(i0, "/", i02)')
    title = 'CHIRPS Precipitation, '+date
  endif
  
  if mode eq 1 then begin
    print, 'Restoring mean...'
    ;restore monthly mean
    mean_path = string(means_dir, 'chirps.', month, '.tif.sav', form='(2a, i02, a)')
    restore, mean_path
    chirps = mean
    title = string('CHIRPS Mean, ', months[month-1], f='(2a)')
    print, 'Restored mean...'
    help, chirps
  endif
    
  if mode eq 2 then begin
    ;restore monthly mean
    mean_path = string(means_dir, 'chirps.', month, '.tif.sav', form='(2a, i02, a)')
    restore, mean_path
    chirps -= mean
    clut = 72
    precp_max = 100
    precp_min = -100
  endif
    
  img = image(chirps, min_value=precp_min, max_value=precp_max, rgb_table=clut, dimensions=[xsize,ysize], $
  margin=0, grid_units='deg', image_location=[-127.5, -50.0], image_dimensions=[180.0, 100.0], $
  map_projection='Geographic', mapgrid=grid, /buffer)
    
  grid = img.mapgrid
  grid.hide = 1
  
  print, 'mapping boundaries...'
  map = map('Geographic', limit=[-50, -127.5, 50, 52.5], margin=0, /overplot)
  m1 = mapcontinents(/countries, color = [150,150,150])
  
  t = text(.46, .8, title, /current, color=!color.black, font_style='Bold', alignment=0.5)
  
  print, 'building colorbar...'
  ;cb = colorbar(target=img, position=[.5, .1, .7, .15], rgb_table=rgb, title='(mm)', $
  cb = colorbar(target=img, position=[.05, .2, .25, .25], rgb_table=clut, title='(mm)', $
    tickvalues=tick_vals, tickdir=0, textpos=1, subticklen=0.0)
  cb.TEXTPOS = 0
  cb.TICKDIR = 1
  cb.BORDER_ON = 1
  cb.COLOR = 'black'
  
  print, 'copying buffer...'
  buf = img.CopyWindow(border=0)
  
  print, 'reforming buffer...'
  ii = where(mask eq 0)
  for i=0, 2 do begin
    b = reform(buf[i, *, *])
    b[ii] = bg_color[i]
    buf[i,*,*] = b
  endfor
  
  print, 'building image again...'
  img = image(buf, dimensions=[xsize, ysize], margin=0, /current)
  print, 'copying buffer again...'
  buf = img.CopyWindow(border=0)
 
  file_name = string('chirps.', year, '.', month, '.', clut_orig, '.', mode, '.png', form='(a, i4, a, i0, a, i0, a, i0, a)')
  
  img_path = drupal_dir + file_name
  print, 'writing to: ', img_path
  print, ''
  write_png, img_path, buf
  
  img_url = url_path + file_name  
  
  print, 'Returning img_url:' + img_url
  print, ''
  print, 'fini!'

  return, img_url
  
END
