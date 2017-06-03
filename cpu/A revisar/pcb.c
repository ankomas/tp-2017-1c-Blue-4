#include <stdio.h>
#include <parser/metadata_program.h>

typedef struct{
	uint32_t pag;
	uint32_t off;
	uint32_t size;
}t_pos;

typedef struct{
	char ID;
	t_pos pos;
}t_var;

typedef struct{
	uint32_t argsSize;
	t_var* args;
	uint32_t varsSize;
	t_var* vars;
	uint32_t retPos;
	t_pos retVar;
}t_stack;

typedef struct{
	uint32_t pid;
	uint32_t cantPagCod;
	uint32_t indiceCodigoSize;
	t_instructions* indiceCodigo;
	uint32_t indiceEtiquetasSize;
	char* indiceEtiquetas;
	uint32_t indiceStackSize;
	t_stack* indiceStack;
	uint32_t exitCode;
}t_pcb;

int compararPos(t_pos pos1,t_pos pos2){
	if(pos1.pag!=pos2.pag){
		return 0;
	}
	if(pos1.off!=pos2.off){
		return 0;
	}
	if(pos1.size!=pos2.size){
		return 0;
	}
	return 1;
}

size_t tamanioEntradaStack(t_stack entradaStack){
	return sizeof(uint32_t)+
	sizeof(t_var)*entradaStack.argsSize+
	sizeof(uint32_t)+sizeof(t_var)*entradaStack.varsSize+
	sizeof(uint32_t)+sizeof(t_pos);
}

t_pos avanzarPos(t_pos posActual,uint32_t tamPag){
	if(posActual.off==tamPag-4){
		posActual.pag++;
		posActual.off=0;
	}
	else{
		posActual.off+=4;
	}

	return posActual;
}

uint32_t buscarPosOcupadas(t_pos** posOcupadas,uint32_t stackSize, t_stack* indiceStack){
	int i,j,k,cantPosOcupadas;
	t_pos posActual;
	
	posActual.pag=0;
	posActual.off=0;
	posActual.size=4;

	cantPosOcupadas=0;

	for(i=0;i<stackSize;i++){
		if(indiceStack[i].args)
		{
			for(j=0;j<indiceStack.argsSize;j++){
				cantPosOcupadas++;
				posOcupadas=realloc(posOcupadas,sizeof(t_pos)*cantPosOcupadas);
				memcpy(posOcupadas,indiceStack[i].args[j],sizeof(t_pos)*(cantPosOcupadas-1));
			}
		}
		if(indiceStack[i].vars)
		{
			for(k=0;k<indiceStack.varsSize;k++){
				cantPosOcupadas++;
				posOcupadas=realloc(posOcupadas,sizeof(t_pos)*cantPosOcupadas);
				memcpy(posOcupadas,indiceStack[i].vars[k],sizeof(t_pos)*(cantPosOcupadas-1));
			}
		}
	}

	return cantPosOcupadas;
}

t_pos buscarPosLibre(uint32_t stackSize,t_stack* indiceStack,uint32_t tamPag){
	int i;
	t_pos posActual;
	t_pos* posOcupadas;
	uint32_t cantPosOcupadas;

	posActual.pag=0;
	posActual.off=0;
	posActual.size=4;

	cantPosOcupadas=buscarPosOcupadas(&posOcupadas,stackSize,indiceStack,tamPag);

	if(cantPosOcupadas==0)
		return posActual;

	i=0;

	while(compararPos(posActual,posOcupadas[i])&&i<cantPosOcupadas){
		posActual=avanzarPos(posAcutal,tamPag);
		i++;
	}

	return posActual;
}

t_puntero posAPuntero(t_pos pos,uint32_t tamPag){
	return pos.pag*tamPag+pos.offset;
}

t_puntero nuevaVar(t_pcb pcb,t_nombre_variable variable,uint32_t tamPag){
	t_var nuevaVar
	t_pos nuevaPos;

	nuevaPos=buscarPosLibre(pcb.indiceStackSize,pcb.indiceStack,tamPag);

	nuevaVar.ID=variable;
	nuevaVar.pos=nuevaPos;

	pcb.indiceStack[pcb.indiceStackSize-1]=nuevaVar;

	return posAPuntero(nuevaPos.tamPag);
}