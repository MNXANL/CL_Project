function f1
  params
    a
    b
  endparams

  vars
    c 1
    d 1
    found 1
  endvars

     %1 = 0
     found = %1
     %2 = 0.7
     %4 = float a
     %3 = %4 +. %2
     c = %3
     %5 = 0
     d = %5
     %6 = a + d
     %8 = float %6
     %7 = c <. %8
     %9 = not found
     %10 = %7 or %9
     ifFalse %10 goto endif1
     goto while1
  label do1 :
     %13 = 1
     %14 = b - %13
     b = %14
     %15 = 1
     found = %15
  label while1 :
     %11 = 0
     %12 = %11 < b
     %16 = not %12
     ifFalse %16 goto do1
  label endif1 :
     %17 = 11
     %18 = b <= %17
     ifFalse %18 goto endif2
     writeln
     writeln
     writef c
     writeln
     writeln
     
     %19 = 2
     %21 = float %19
     %20 = %21 *. c
     %22 = 1
     %24 = float %22
     %23 = %20 +. %24
     c = %23
  label endif2 :
     writef c
     writeln
     return
endfunction

function main
  vars
    a 1
    b 1
  endvars

   %1 = 4
   a = %1
   %2 = 2
   %3 = %2 * a
   %4 = 1
   %5 = %3 + %4
   b = %5
   call f1
   return
endfunction


