//
// Created by stanissslaf on 5/1/18.
//

#ifndef LANGUAGE_OPCODES_H
#define LANGUAGE_OPCODES_H


// NASM opcodes

long long enum opcodes
{

    // 1 byte commands

    Push_rax =      /*  push rax      */ 0x50,
    Push_rbp =      /*  push rbp      */ 0x55,
    Pop_rbp =       /*  pop rbp       */ 0x5d,
    Pop_rax =       /*  pop rax       */ 0x58,
    Ret =           /*  ret           */ 0xc3,
    SAHF =          /*  sahf          */ 0x9e,
    CALL =          /*  call          */ 0xe8,

    jmp =           /* jmp Label      */ 0xe9,


    // 2 byte commands

    mov_r14 =       /*  mov r14,      */ 0xbe49,
    Push_r14 =      /*  push r14      */ 0x5641,
    FSQRT =         /*  fsqrt         */ 0xfad9,
    FCOMMP =        /*  fcommp        */ 0xd9de,
    FNSTSW_AX =     /*  fnstsw, ax    */ 0xe0df,
    FADDP =         /*  faddp         */ 0xc1de,
    FMULP =         /*  fmulp         */ 0xc9de,
    FDIVP =         /*  fdivp         */ 0xf9de,
    FSUBP =         /*  fsubp         */ 0xe9de,
    Syscall =       /*  syscll        */ 0x050f,

    je = 0x840f,
    jne = 0x850f,
    jb = 0x820f,
    jbe = 0x860f,
    jae = 0x830f,
    ja = 0x870f,


    Push_Q_Rbp_Sub_Val_short =  /* push qword [rbp - value] */ 0x75ff,      //if value <= 255 sizeof(value) == 1
    Push_Q_Rbp_Sub_Val_long =   /* push qword [rbp - value] */ 0xb5ff,      //else sizeof(value) = 2


    // 3 byte commands


    Mov_rbp_rsp =                   /*  mov rbp rsp     */       0xe58948,

    Sub_Rsp_value_long =            /* sub rsp, value   */       0xec8148,  // if value <= 255 sizeof(value) == 1
    Add_Rsp_value_long =            /*                  */       0xc48148,  // if value > 255  sizeof(value) == 3

    Sub_Rsp_value_short =           /* add rsp, value   */       0xec8348,  // if value <= 255 sizeof(value) == 1
    Add_Rsp_value_short =           /*                  */       0xc48348,  // if value > 255  sizeof(value) == 3

    Push_Q_Word_Rbp =               /* push qword [rbp] */       0x0075ff,
    Fld_Q_Word_First_arg =          /* fld qword [rsp]  */       0x2404dd,
    FSTP_Q_Word_RSP =               /* fstp qword [rsp] */       0x241cdd,
    Xor_rdi_rdi =                   /* xor rdi, rdi     */       0xff3148,
    Mov_Q_Rbp_Sub_value_Rax_short = /* mov [rbp - value], rax */ 0x458948,


    // 4 byte commands

    Mov_Q_Rsp_Sub_val_Rax_short =   /* mov [rsp - value], rax */  0x24448948, // sizeof(value) = 1
    Mov_Q_Rsp_Rax =                 /* mov [rsp], rax         */  0x24048948,

    Mov_Q_Word_Rbp_Rax =            /* mov [rbp], rax         */  0x00458948,
    Fld_Q_Word_Second_arg =         /* fld qword [rsp + 8]    */  0x082444dd,


};


#define  ADD_RSP(value)                                     \
do{                                                         \
    int temp_val = (value) ;                                \
    int temp_cmd =  Add_Rsp_value_long;                     \
                                                            \
    if (temp_val > 255){                                    \
                                                            \
        memcpy(comp->curb, &temp_cmd, 3);                   \
        comp->cur_sz += 3;                                  \
        comp->curb+=3;                                      \
        memcpy(comp->curb, &temp_val, 3);                   \
        comp->cur_sz+=3;                                    \
        comp->curb+=3;                                      \
    }                                                       \
    else {                                                  \
                                                            \
        temp_cmd =  Add_Rsp_value_short;                    \
        memcpy(comp->curb, &temp_cmd, 3);                   \
        comp->cur_sz+=3;                                    \
        comp->curb+=3;                                      \
        memcpy(comp->curb, &temp_val, 1);                   \
        comp->curb+=1;                                      \
        comp->cur_sz+=1;                                    \
    }                                                       \
} while(0)


#define  SUB_RSP(value)                                     \
do{                                                         \
    int temp_val = (value) ;                                \
    int temp_cmd =  Sub_Rsp_value_long;                     \
                                                            \
    if (temp_val > 255){                                    \
                                                            \
        memcpy(comp->curb, &temp_cmd, 3);                   \
        comp->cur_sz+=3;                                    \
        comp->curb+=3;                                      \
        memcpy(comp->curb, &temp_val, 3);                   \
        comp->cur_sz+=3;                                    \
        comp->curb+=3;                                      \
    }                                                       \
    else {                                                  \
                                                            \
        temp_cmd =  Sub_Rsp_value_short;                    \
        memcpy(comp->curb, &temp_cmd, 3);                   \
        comp->cur_sz+=3;                                    \
        comp->curb+=3;                                      \
        memcpy(comp->curb, &temp_val, 1);                   \
        comp->curb+=1;                                      \
        comp->cur_sz+=1;                                    \
    }                                                       \
} while(0)


#define MOV_RSP_SUB_RAX(value)                              \
do{                                                         \
if((value) != 0) {                                          \
        int temp_val = 256 - (value) ;                      \
        int temp_cmd = Mov_Q_Rsp_Sub_val_Rax_short;         \
                                                            \
        memcpy(comp->curb, &temp_cmd, 4);                   \
        comp->cur_sz+=4;                                    \
        comp->curb+=4;                                      \
        memcpy(comp->curb, &temp_val, 1);                   \
        comp->curb+=1;                                      \
        comp->cur_sz+=1;                                    \
    }                                                       \
    else                                                    \
    {                                                       \
        int temp_cmd = Mov_Q_Rsp_Rax;                       \
        memcpy(comp->curb, &temp_cmd, 4);                   \
        comp->curb+=4;                                      \
        comp->cur_sz+=4;                                    \
    }                                                       \
} while(0)


#define  MOV_RBP_SUB_RAX(value)                             \
do{                                                         \
if((value) != 0) {                                          \
        int temp_val = 256 - (value) ;                      \
        int temp_cmd = Mov_Q_Rbp_Sub_value_Rax_short;       \
                                                            \
        memcpy(comp->curb, &temp_cmd, 3);                   \
        comp->cur_sz+=3;                                    \
        comp->curb += 3;                                    \
        memcpy(comp->curb, &temp_val, 1);                   \
        comp->curb+=1;                                      \
        comp->cur_sz+=1;                                    \
    }                                                       \
    else                                                    \
    {                                                       \
        int temp_cmd = Mov_Q_Word_Rbp_Rax;                  \
        memcpy(comp->curb, &temp_cmd, 4);                   \
        comp->curb+=4;                                      \
        comp->cur_sz+=4;                                    \
    }                                                       \
} while(0)


#define PUSH_Q_RBP(value)                                   \
do{                                                         \
if((value) != 0) {                                          \
        unsigned int temp_val = 256 - (value) ;             \
        int temp_cmd =  Push_Q_Rbp_Sub_Val_short;           \
                                                            \
        if((value) <= 128) {                                \
            memcpy(comp->curb, &temp_cmd, 2);               \
            comp->curb+=2;                                  \
            comp->cur_sz+=2;                                \
            memcpy(comp->curb, &temp_val, 1);               \
            comp->curb+=1;                                  \
            comp->cur_sz+=1;                                \
        }                                                   \
        else {                                              \
            temp_cmd = Push_Q_Rbp_Sub_Val_long;             \
            temp_val = 0x100000000 - (value);               \
            memcpy(comp->curb, &temp_cmd, 2);               \
            comp->curb+=2;                                  \
            comp->cur_sz+=2;                                \
            memcpy(comp->curb, &temp_val, 4);               \
            comp->curb+=4;                                  \
            comp->cur_sz+=4;                                \
        }                                                   \
    }                                                       \
    else                                                    \
    {                                                       \
        int temp_cmd = Push_Q_Word_Rbp;                     \
        memcpy(comp->curb, &temp_cmd, 3);                   \
        comp->curb+=3;                                      \
        comp->cur_sz+=3;                                    \
    }                                                       \
}while(0)                                                   \


#define push_bytes(value, size)         \
do{                                     \
    size_t tempp = value;               \
    memcpy(comp->curb, &tempp, size);   \
    comp -> curb +=(size);              \
    comp->cur_sz+= (size);              \
}while(0)

#endif //LANGUAGE_OPCODES_H
