grammar ubjson;

fragment
BYTE : ('\u0000'..'\uFFFF');

fragment
CHAR :  ('\u0000'..'\u007F');

ubjson : object ;

byte1 : BYTE ;
byte2 : byte1 byte1 ;
byte4 : byte2 byte2 ;
byte8 : byte4 byte4 ;
bytes : BYTE* ;
integertype : 'i' byte1 | 'U' byte1 | 'I' byte2 | 'l' byte4 | 'L' byte8 ;
id : integertype bytes ; 
numerictype : integertype | 'd' byte4 | 'D' byte8 | 'H' id ;
stringtype : 'S' id ;
valuetype : numerictype | 'Z' | 'N' | 'T' | 'F' | 'C' CHAR | stringtype ;

bareobject : (id type)* '}' | '#' integertype (id type)* | '$' type '#' integertype (id baretype?)* ;
barearray : type* ']' | '#' integertype type* | '$' type '#' integertype baretype* ;
baretype : id | bytes | CHAR | barearray | bareobject ;

object : '{' bareobject ;
array : '[' barearray ;
//object : '{' (id type)* '}';
//array : '[' type* ']'; 

containertype : object | array ;
type : valuetype | containertype ;