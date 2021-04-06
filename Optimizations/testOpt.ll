; ModuleID = '../FrontEnd/resultingIR.ll'
source_filename = "../FrontEnd/resultingIR.ll"

declare i32 @read()

declare void @print(i32)

define i32 @f(i32) {
  %a = alloca i32, align 4
  store i32 %0, i32* %a
  %i = alloca i32, align 4
  store i32 0, i32* %i
  br label %2

; <label>:2:                                      ; preds = %11, %1
  %3 = load i32, i32* %i
  %4 = icmp slt i32 %3, 10
  br i1 %4, label %5, label %8

; <label>:5:                                      ; preds = %2
  %6 = load i32, i32* %i
  %7 = icmp slt i32 %6, 5
  br i1 %7, label %9, label %11

; <label>:8:                                      ; preds = %2
  ret i32 1

; <label>:9:                                      ; preds = %5
  %10 = load i32, i32* %i
  call void @print(i32 %10)
  br label %11

; <label>:11:                                     ; preds = %9, %5
  %12 = load i32, i32* %i
  %13 = add i32 %12, 1
  store i32 %13, i32* %i
  br label %2
}
