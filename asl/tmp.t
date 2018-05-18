function f
  params
    _result
    a
    b
  endparams

  vars
    x 1
    y 1
    z 10
  endvars

     %2 = 67
     %3 = a + %2
     z = %3
     %4 = 34
     x = %4
     %6 = 56
     %8 = %6 + 
     z = %8
     %10 = x < 
     ifFalse %10 goto then1
     %11 = 78
     x = %11
     writef b
     writeln
     goto endif1
  label then1 :
     %13 = 99
     x = %13
  label endif1 :
     writei 
     writeln
     %16 = 1
     _result = %16
     return
endfunction

function fz
  params
    r
  endparams

     goto while1
  label do1 :
     %3 = 1
     %4 = r - %3
     r = %4
  label while1 :
     %1 = 0
     %2 = %1 < r
     %5 = not %2
     ifFalse %5 goto do1
     return
endfunction

function main
  vars
    a 1
  endvars

     pushparam 
     %1 = 3
     pushparam %1
     %2 = 2
     pushparam %2
     call f
     popparam 
     popparam 
     popparam %3
     ifFalse %3 goto endif1
     %4 = 3.7
     %6 = float a
     %5 = %6 +. %4
     %7 = 4
     %9 = float %7
     %8 = %5 +. %9
     writef %8
     writeln
  label endif1 :
     return
endfunction


