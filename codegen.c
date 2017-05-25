#include  <stdio.h>
#include "codegen.h"
#include "context.h"

int nopcodes;
int opcodes[65536];


int emit(int op) {
	opcodes[nopcodes++] = op;
	return nopcodes - 1;
}

void emit_push(int val_type, int val, int size) {
	emit(OP_PUSH);
	emit(val_type);
	emit(val);
	emit(size);
}

void emit_asm_cmp(FILE* ftarget, const char* op) {
	fprintf(ftarget, "  pop eax\n");
	fprintf(ftarget, "  pop ecx\n");
	fprintf(ftarget, "  cmp ecx, eax\n");
	fprintf(ftarget, "  %s al\n", op);
	fprintf(ftarget, "  cbw\n");
	fprintf(ftarget, "  cwde\n");
	fprintf(ftarget, "  push eax\n");
}

void emit_asm_binop(FILE* ftarget, const char* op) {
	fprintf(ftarget, "  pop eax\n");
	fprintf(ftarget, "  pop ecx\n");
	fprintf(ftarget, "  %s ecx, eax\n", op);
	fprintf(ftarget, "  push ecx\n");
}

const char* get_asm_type(int size) {
	switch (size) {
	case 1:
		return "BYTE";

	case 2:
		return "WORD";

	case 4:
		return "DWORD";

	default:
		return "XXX";
	}
}

const char* get_asm_reg(int size) {
	switch (size) {
	case 1:
		return "al";

	case 2:
		return "ax";

	case 4:
		return "eax";

	default:
		return "xxx";
	}
}

void gen_move(FILE* ftarget, const char* dst, const char* src, int offset, int size) {
	const char* instr = size < 4 ? "movsx" : "mov";

	fprintf(ftarget, "  %s %s, %s PTR [%s]%+d\n", instr, dst, get_asm_type(size), src, offset);
}

void gen_code(FILE* ftarget, const char* funcname, int from, int end) {
	for (int i = from; i < end;) {
		int op = opcodes[i++];
		if (op == OP_FUNC_PROLOGUE) {
			fprintf(ftarget, "_%s proc\n", funcname);
			fprintf(ftarget, "  push ebp\n");
			fprintf(ftarget, "  mov ebp, esp\n");
			fprintf(ftarget, "  sub esp, %d\n\n", stack_size);
		}
		else if (op == OP_FUNC_EPILOGUE) {
			fprintf(ftarget, "\n__exit_%s:\n", funcname);
			fprintf(ftarget, "  mov esp, ebp\n");
			fprintf(ftarget, "  pop ebp\n");
			fprintf(ftarget, "  ret 0\n");
			fprintf(ftarget, "_%s endp\n\n", funcname);
		}
		else if (op == OP_PUSH) {
			int value_type = opcodes[i++];
			int value = opcodes[i++];
			int size = opcodes[i++];

			if (value_type == VAL_STR) {
				fprintf(ftarget, "  push OFFSET __str_%d\n", value);
			}
			else if (value_type == VAL_INT) {
				fprintf(ftarget, "  push %d\n", value);
			}
			else if (value_type == VAL_LOCAL) {
				if (size == 0) {
					size = 4;
				}

				gen_move(ftarget, "eax", "ebp", value, size);
				fprintf(ftarget, "  push eax\n", value);
			}
			else if (value_type == VAL_LOCAL_ADDR) {
				if (value >= 0) {
					fprintf(ftarget, "  lea eax, DWORD PTR [ebp]+%d \n", value);
				}
				else {
					fprintf(ftarget, "  lea eax, DWORD PTR [ebp]%d \n", value);
				}

				fprintf(ftarget, "  push eax\n", value);
			}
			else if (value_type == VAL_GLOB) {
				fprintf(ftarget, "  push DWORD PTR _%s\n", globals[value]);
			}
			else if (value_type == VAL_GLOB_ADDR) {
				fprintf(ftarget, "  push OFFSET _%s\n", globals[value]);
			}
		}
		else if (op == OP_DEREF) {
			int item_size = opcodes[i++];
			fprintf(ftarget, "  pop ecx\n");
			fprintf(ftarget, "  pop eax\n");
			fprintf(ftarget, "  imul ecx, %d\n", item_size);
			gen_move(ftarget, "eax", "eax + ecx", 0, item_size);

			fprintf(ftarget, "  push eax\n");
		}
		else if (op == OP_DEREF_ADDR) {
			int item_size = opcodes[i++];
			fprintf(ftarget, "  pop ecx\n");
			fprintf(ftarget, "  pop eax\n");
			fprintf(ftarget, "  imul ecx, %d\n", item_size);
			fprintf(ftarget, "  lea eax, DWORD PTR [eax + ecx]\n");
			fprintf(ftarget, "  push eax\n");
		}
		else if (op == OP_DUP_VALUE) {
			int size = opcodes[i++];
			fprintf(ftarget, "  mov eax, DWORD PTR [esp]\n"); // peek addr from the top of the stack
			gen_move(ftarget, "eax", "eax", 0, size); // dereference addr
			fprintf(ftarget, "  push eax\n");
		}
		else if (op == OP_SAVE) {
			int size = opcodes[i++];

			fprintf(ftarget, "  pop eax\n");
			fprintf(ftarget, "  pop ecx\n");

			const char* reg = get_asm_reg(size);
			fprintf(ftarget, "  mov %s PTR [ecx], %s\n", get_asm_type(size), reg);
			fprintf(ftarget, "  push eax\n");
		}
		else if (op == OP_CALL) {
			int header_ref = opcodes[i];
			int param_count = opcodes[header_ref];

			i = header_ref + param_count + 1;

			int paramEnd = header_ref;
			for (int param = param_count - 1; param >= 0; --param) {
				int paramOffset = opcodes[header_ref + param + 1];
				gen_code(ftarget, funcname, paramOffset, paramEnd);

				paramEnd = paramOffset;
			}

			// FIXME: support different parameter sizes
			int st = param_count * 4;
			fprintf(ftarget, "  call DWORD PTR [esp+%d]\n", st);
			fprintf(ftarget, "  add esp, %d\n", st + 4); // size of paremters + pointer to function
			fprintf(ftarget, "  push eax\n");
		}
		else if (op == OP_RETURN) {
			fprintf(ftarget, "  pop eax\n");
			fprintf(ftarget, "  jmp __exit_%s\n", funcname);
		}
		else if (op == OP_FOR) {
			// post increment instructions after body

			int increment_end = opcodes[i++];
			int body_end = opcodes[i++];
			gen_code(ftarget, funcname, increment_end, body_end);
			gen_code(ftarget, funcname, i, increment_end);

			i = body_end;
		}
		else if (op == OP_JMP) {
			int label = opcodes[i++];
			fprintf(ftarget, "  jmp __%s_%d\n", funcname, label);
		}
		else if (op == OP_JMPZ) {
			fprintf(ftarget, "  pop eax\n");
			fprintf(ftarget, "  test eax, eax\n");

			int label = opcodes[i++];
			fprintf(ftarget, "  je __%s_%d\n", funcname, label);
		}
		else if (op == OP_JMPNZ) {
			fprintf(ftarget, "  pop eax\n");
			fprintf(ftarget, "  test eax, eax\n");

			int label = opcodes[i++];
			fprintf(ftarget, "  jne __%s_%d\n", funcname, label);
		}
		else if (op == OP_CASE) {
			int var = opcodes[i++];
			int label = opcodes[i++];
			fprintf(ftarget, "  pop eax\n");
			fprintf(ftarget, "  cmp DWORD PTR [ebp]%+d, eax\n", var);
			fprintf(ftarget, "  jne __%s_%d\n", funcname, label);
		}
		else if (op == OP_LABEL) {
			int id = opcodes[i++];
			fprintf(ftarget, "\n__%s_%d:\n", funcname, id);
		}
		else if (op == OP_ADD) {
			emit_asm_binop(ftarget, "add");
		}
		else if (op == OP_SUB) {
			emit_asm_binop(ftarget, "sub");
		}
		else if (op == OP_MUL) {
			emit_asm_binop(ftarget, "imul");
		}
		else if (op == OP_DIV) {
			fprintf(ftarget, "  pop ecx\n");
			fprintf(ftarget, "  pop eax\n");
			fprintf(ftarget, "  cdq\n");
			fprintf(ftarget, "  idiv ecx\n", op);
			fprintf(ftarget, "  push eax\n");
		}
		else if (op == OP_MOD) {
			fprintf(ftarget, "  pop ecx\n");
			fprintf(ftarget, "  pop eax\n");
			fprintf(ftarget, "  cdq\n");
			fprintf(ftarget, "  idiv ecx\n", op);
			fprintf(ftarget, "  push edx\n");
		}
		else if (op == OP_INC || op == OP_DEC) {
			fprintf(ftarget, "  pop eax\n");
			fprintf(ftarget, "  %s DWORD PTR [eax], 1\n", op == OP_INC ? "add" : "sub");
			fprintf(ftarget, "  mov eax, DWORD PTR [eax]\n");
			fprintf(ftarget, "  push eax\n");
		}
		else if (op == OP_INC_POST || op == OP_DEC_POST) {
			fprintf(ftarget, "  pop ecx\n");
			fprintf(ftarget, "  mov eax, DWORD PTR [ecx]\n");
			fprintf(ftarget, "  %s DWORD PTR  [ecx], 1\n", op == OP_INC_POST ? "add" : "sub");
			fprintf(ftarget, "  push eax\n");
		}
		else if (op == OP_LESS) {
			emit_asm_cmp(ftarget, "setl");
		}
		else if (op == OP_GREATER) {
			emit_asm_cmp(ftarget, "setg");
		}
		else if (op == OP_LESS_EQ) {
			emit_asm_cmp(ftarget, "setle");
		}
		else if (op == OP_GREATER_EQ) {
			emit_asm_cmp(ftarget, "setge");
		}
		else if (op == OP_EQ) {
			emit_asm_cmp(ftarget, "sete");
		}
		else if (op == OP_NOT_EQ) {
			emit_asm_cmp(ftarget, "setne");
		}
		else if (op == OP_NOT){
			fprintf(ftarget, "  pop ecx\n");
			fprintf(ftarget, "  xor eax, eax\n");
			fprintf(ftarget, "  cmp ecx, eax\n");
			fprintf(ftarget, "  sete al\n");
			fprintf(ftarget, "  push eax\n");
		}
		else if (op == OP_NEG){
			fprintf(ftarget, "  neg DWORD PTR [esp]\n");
		}
	}
}