preq := main.c lexer.c
exe := do
cc := clang

run: build
	./${exe}

build:
	${cc} ${preq} -o ${exe}
	
gcc:
	gcc --std=c11 ${preq} -o ${exe} -g