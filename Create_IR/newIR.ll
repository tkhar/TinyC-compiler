
define i32 @test(i32, i32) {
  %m = alloca i32, align 4
  %n = alloca i32, align 4
  %a = alloca i32, align 4
  store i32 %0, i32* %m
  store i32 %1, i32* %n
  store i32 0, i32* %a
  %3 = load i32, i32* %m
  %4 = load i32, i32* %n
  %5 = icmp slt i32 %3, %4
  br i1 %5, label %6, label %8

; <label>:6:                                      ; preds = %2
  %7 = load i32, i32* %m
  store i32 %7, i32* %a
  br label %10

; <label>:8:                                      ; preds = %2
  %9 = load i32, i32* %n
  store i32 %9, i32* %a
  br label %10

; <label>:10:                                     ; preds = %8, %6
  %11 = load i32, i32* %a
  ret i32 %11
}
