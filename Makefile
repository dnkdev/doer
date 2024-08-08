preq := main.c lexer.c
exe := do
cc := clang

build:
	${cc} ${preq} -o ${exe}

run:
	./${exe}