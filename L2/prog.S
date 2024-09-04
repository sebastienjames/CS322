(@printArray
	1 1
	%check <- rdi
	%check &= 1
	cjump 0 = 0 :isarray
	goto :notarray
	:notarray
	rax <- 0
	return
	:isarray
	%S0 <- 0
	mem rsp 0 <- %S0
	%arr <- rdi
	%arrlength <- mem rdi 0
	:forloop
	%S1 <- mem rsp 0
	cjump %S1 < %arrlength :continue
	goto :done
	:continue
	rdi <- %arr
	%S2 <- mem rsp 0
	rsi <- %S2
	mem rsp -8 <- :ret
	call @printArrayElement 2
	:ret
	%S3 <- mem rsp 0
	%S3++
	mem rsp 0 <- %S3
	goto :forloop
	:done
	rax <- 1
	return
)(@printArray
	1 1
)(@printArray
	1 1
)