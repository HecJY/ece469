#ifndef __USERPROG__
#define __USERPROG__


typedef struct molecules
{
    /* data */
    mbox_t s2_mbox;
    mbox_t s_mbox;
    mbox_t co_mbox;
    mbox_t o2_mbox;
    mbox_t c2_mbox;
    mbox_t so4_mbox;

}molecules;




#define S2INJECTION "s2_injection.dlx.obj"
#define S2BREAK "s2_break.dlx.obj"
#define SO4FORM "so4_form.dlx.obj"
#define COBREAK "co_break.dlx.obj"
#define COINJECTION "co_injection.dlx.obj"

#ifndef NULL
#define NULL (void *)0x0

#endif
#endif
