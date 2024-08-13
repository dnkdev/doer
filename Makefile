preq := src/main.c src/lexer.c src/parser.c src/ast.c src/interpret.c
exe := do
cc := clang

run: build
	./${exe}

build:
	${cc} ${preq} -o ${exe} -g
	
gcc:
	gcc --std=c11 ${preq} -o ${exe} -g