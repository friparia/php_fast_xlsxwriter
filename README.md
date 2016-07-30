# php_fast_xlsxwriter
a php extension to write an array to excel(.xlsx file) fast

一个快速导出Excel(.xlsx)文件的PHP扩展
# Requirements
* PHP 5+ or PHP 7+
* GCC 4.4+
* Zlib 1.2.8+
* [libxlsxwriter](https://github.com/jmcnamara/libxlsxwriter)

# Installation
 ```shell
 git clone https://github.com/friparia/php_fast_xlsxwriter
 cd php_fast_xlsxwriter
 phpize
 ./configure --with-xlsxwriter --with-libxlsxwriter=/path/to/libxlsxwriter
 make && make install
 ```
 Then, add `extension=xlsxwriter.so' in your `php.ini` file
 
# Usage
  ## xlsx_write
  ```php
  $a = ['a', 'b'];
  $path = '~/test.xlsx';
  xlsx_write($a, $path);
  $b = [['a', 'b'], ['a1', 'b1']];
  xlsx_write($b, $path)
  ```
  ## xlsx_write_by_sheet
  ```php
  $c = [
  'sheet1' => [['a', 'b'], ['a1', 'b1']],
  ]
  xlsx_write_by_sheet($b, $path)
  ```

