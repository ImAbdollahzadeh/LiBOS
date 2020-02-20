GCCPARAMS = -Iinclude -Wno-discarded-qualifiers -Wno-implicit-function-declaration -Wno-format -Wno-int-to-pointer-cast -Wno-int-conversion
objects   =                     \
LiBOSASM_CORE.o                 \
main.o                          \
LiBOSASM_STRING.o               \
LiBOSASM_MOV_INSTRUCTION.o      \
LiBOSASM_ADD_INSTRUCTION.o      \
LiBOSASM_SUB_INSTRUCTION.o      \
LiBOSASM_CMP_INSTRUCTION.o      \
LiBOSASM_JMP_INSTRUCTION.o      \
LiBOSASM_XOR_INSTRUCTION.o      \
LiBOSASM_SINGLETS_INSTRUCTION.o \
LiBOSASM_INT_INSTRUCTION.o      \
LiBOSASM_POP_INSTRUCTION.o      \
LiBOSASM_PUSH_INSTRUCTION.o 

%.o: ./src/%.c
	@gcc $(GCCPARAMS) -c -o ./$@ $<

MERGE:
	@gcc $(objects) -o ./final.o
	@./final.o
	@rm LiBOSASM_CORE.o
	@rm main.o
	@rm LiBOSASM_STRING.o
	@rm LiBOSASM_MOV_INSTRUCTION.o
	@rm LiBOSASM_SUB_INSTRUCTION.o
	@rm LiBOSASM_ADD_INSTRUCTION.o
	@rm LiBOSASM_JMP_INSTRUCTION.o
	@rm LiBOSASM_CMP_INSTRUCTION.o
	@rm LiBOSASM_XOR_INSTRUCTION.o
	@rm LiBOSASM_SINGLETS_INSTRUCTION.o
	@rm LiBOSASM_POP_INSTRUCTION.o
	@rm LiBOSASM_PUSH_INSTRUCTION.o
	@rm LiBOSASM_INT_INSTRUCTION.o
	@rm ./final.o

run:
ifeq ($(readme),1)
	@echo LiBOSASM_readme:
	@cat ./ReadMe
endif
ifeq ($(readme),true)
	@echo LiBOSASM_readme:
	@cat ./ReadMe
endif
ifeq ($(licence),1)
	@echo LiBOSASM_licence:
	@cat ./Licence
endif
ifeq ($(licence),true)
	@echo LiBOSASM_licence:
	@cat ./Licence
endif
	@echo --	code	--
	@make --no-print-directory LiBOSASM_CORE.o
	@make --no-print-directory main.o
	@make --no-print-directory LiBOSASM_STRING.o
	@make --no-print-directory LiBOSASM_MOV_INSTRUCTION.o
	@make --no-print-directory LiBOSASM_ADD_INSTRUCTION.o
	@make --no-print-directory LiBOSASM_SUB_INSTRUCTION.o
	@make --no-print-directory LiBOSASM_JMP_INSTRUCTION.o
	@make --no-print-directory LiBOSASM_CMP_INSTRUCTION.o
	@make --no-print-directory LiBOSASM_XOR_INSTRUCTION.o
	@make --no-print-directory LiBOSASM_SINGLETS_INSTRUCTION.o
	@make --no-print-directory LiBOSASM_INT_INSTRUCTION.o
	@make --no-print-directory LiBOSASM_POP_INSTRUCTION.o
	@make --no-print-directory LiBOSASM_PUSH_INSTRUCTION.o
	@make --no-print-directory MERGE
