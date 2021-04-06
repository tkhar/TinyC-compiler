
declare i32 @read()

declare void @print(i32)

define i32 @function() {
  %a = alloca i32, align 4
  store i32 3, i32* %a
  %1 = load i32, i32* %a
  %2 = icmp sgt i32 %1, 2
  br i1 %2, label %3, label %8

3:                                                ; preds = %0
  %4 = load i32, i32* %a
  %5 = add i32 %4, 1
  store i32 %5, i32* %a
  br label %6

6:                                                ; preds = %8, %3
  %7 = load i32, i32* %a
  ret i32 %7

8:                                                ; preds = %0
  store i32 0, i32* %a
  br label %6
}
