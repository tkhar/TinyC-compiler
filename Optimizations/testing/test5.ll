; ModuleID = 'testing/test5.c'
source_filename = "testing/test5.c"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @f() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  store i32 0, i32* %1, align 4
  store i32 100, i32* %2, align 4
  br label %3

; <label>:3:                                      ; preds = %6, %0
  %4 = load i32, i32* %1, align 4
  %5 = icmp slt i32 %4, 1000
  br i1 %5, label %6, label %9

; <label>:6:                                      ; preds = %3
  store i32 200, i32* %2, align 4
  %7 = load i32, i32* %1, align 4
  %8 = add nsw i32 %7, 1
  store i32 %8, i32* %1, align 4
  br label %3

; <label>:9:                                      ; preds = %3
  %10 = load i32, i32* %2, align 4
  ret i32 %10
}

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 8.0.0 (Fedora 8.0.0-1.fc30)"}
