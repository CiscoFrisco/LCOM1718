#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include "vg_macros.h"
#include "vbe.h"
#include "lmlib.h"
#include "video_test.h"

#define LINEAR_MODEL_BIT 14

#define PB2BASE(x) (((x) >> 4) & 0x0F000)
#define PB2OFF(x) ((x) & 0x0FFFF)
#define MIB_SIZE      256
#define VIB_SIZE      512

int vbe_get_mode_info(unsigned short mode, vbe_mode_info_t *vmi_p) {

	struct reg86u reg86;
	mmap_t m;

	lm_init();

	lm_alloc(MIB_SIZE,&m);

	reg86.u.b.intno = VG_SERVICE;
	reg86.u.w.ax = VBE_01;
	reg86.u.w.cx = mode;
	reg86.u.w.es = PB2BASE(m.phys);
	reg86.u.w.di = PB2OFF(m.phys);


	if( sys_int86(&reg86) != OK ) {
		printf("\tvg_get_mode_info(): sys_int86() failed \n");
		return 1;
	}

	memcpy(vmi_p, m.virtual, MIB_SIZE);

	lm_free(&m);

	return 0;
}

int vbe_get_controller_info(vbe_controller_info_t *vbi_p)
{
	struct reg86u reg86;
	mmap_t m;

	lm_init();

	lm_alloc(VIB_SIZE,&m);

	reg86.u.b.intno = VG_SERVICE;
	reg86.u.w.ax = VBE_00;
	reg86.u.w.es = PB2BASE(m.phys);
	reg86.u.w.di = PB2OFF(m.phys);


	if( sys_int86(&reg86) != OK ) {
		printf("\tvg_get_controller_info(): sys_int86() failed \n");
		return 1;
	}

	memcpy(vbi_p, m.virtual, VIB_SIZE);

	lm_free(&m);

	return 0;
}


