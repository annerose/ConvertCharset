# ConvertCharset
 

##  Program Function

Convert source file's GBK / Unicode16 charset to UTF8(BOM),  and convert the include filenames to lowercase.


##  Program Flow

only test on Windows, dev by VS2005
 
 

1. Traverse the source files(.cpp|.h|.cxx|.c|.java) in the current directory

2. Convert charset to utf-8(bom) (available for VC & GCC)



- GBK -> Unicode16 -> UTF8
- Unicode16 ->  UTF8
- UTF8 (no bom)->  UTF8

 
3. Add command param '/a'  to convert the include filenames to lowercase


Note: avoid freetype, because of the headfiles are macro


## Usage

- Copy ConvertingCharset.exe to src file dir
- Run ConvertingCharset.exe to convert src files's charset to utf8(bom)
- Run 'ConvertingCharset.exe /a' to convert charset and lower include file

2017-6-5




 

