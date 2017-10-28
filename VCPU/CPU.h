//
// Created by Tom on 28.10.2017.
//

#ifndef VCPU_CPU_H
#define VCPU_CPU_H

struct VCPU {

	Stack<double> *values;
	Stack<int> *refunds;

	double ax, bx, cx, dx;
	double *ram;
	double *commands;

	size_t SzOfCode;

};

void Construct_Cpu (size_t sizeOfCmdArr, VCPU *Cpu, Stack<double> *_lValues, Stack<int> *_lRefunds, double *_lcommands);

void Destruct_Cpu (VCPU *Cpu);


void controlVerSig (char **code);

void *createCommandArr (char *const code, double **const arrayPtrCmd, size_t *sizePtrArr);

void executePrg (VCPU *CPU);

void dumpCPU (VCPU CPU);

#endif
