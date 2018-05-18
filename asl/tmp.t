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

     %1 = 9
     %2 = 1
     %3 = %2 * %1
     %4 = 67
     %5 = a + %4
     z[%3] = %5
     %6 = 34
     x = %6
     %7 = 3
     %8 = 1
     %9 = %8 * %7
     %10 = 56
     %11 = 9
     %12 = 1
     %13 = %12 * %11
     %14 = z[%13]
     %15 = %10 + %14
     z[%9] = %15
     %16 = 3
     %17 = 1
     %18 = %17 * %16
     %19 = z[%18]
     %20 = x < %19
     ifFalse %20 goto then1
     %21 = 78
     x = %21
     writef b
     writeln
     goto endif1
  label then1 :
     %23 = 99
     x = %23
  label endif1 :
     %24 = 3
     %25 = 1
     %26 = %25 * %24
     %27 = z[%26]
     writei %27
     writeln
     %29 = 1
     _result = %29
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


