/**
 * \file init.c
 *
 * \date May 21, 2009
 * \author anton
 */

#include "types.h"
#include "common.h"
#include "conio.h"
#include "kernel/irq.h"
#include "kernel/uart.h"
#include "kernel/timers.h"
#include "net/net.h"
#include "fs/rootfs.h"
#include "asm/cache.h"
#include "kernel/module.h"

//TODO this code must move to boot.S file
void copy_data_section() {
	extern char _endtext, _data_start, _data_end, _bstart, _bend;

	char *src = &_endtext;

	char *dst = &_data_start;

	if (0x40000000 >= (unsigned int) &_endtext) {
		/* ROM has data at end of text; copy it. */
		while (dst < &_data_end) {
			*dst++ = *src++;
		}
	}

	/* Zero bss */
	for (dst = &_bstart; dst < &_bend; dst++)
		*dst = 0;
}



static int init_modules() {
	extern module_descriptor_t *__modules_handlers_start, *__modules_handlers_end;
	module_descriptor_t ** p_module = &__modules_handlers_start;
	int i, total = (int) (&__modules_handlers_end - &__modules_handlers_start);

	TRACE("\nInitializing modules (total: %d)\n\n", total);

	for (i = 0; i < total; i++, p_module++) {
		if (NULL == (*p_module)) {
			LOG_ERROR("Missing module descriptor\n");
			continue;
		}
		if (NULL == ((*p_module)->name)) {
			LOG_ERROR("Broken module descriptor. Can't find module name\n");
			continue;
		}
		if (NULL == ((*p_module)->init)) {
			LOG_ERROR("Broken module descriptor. Can't find init function for module %s\n", (*p_module)->name);
			continue;
		}

		TRACE("Initializing %s ... ", (*p_module)->name);
		if (-1 != (*p_module)->init()) {
			TRACE("DONE\n");
		} else {
			TRACE("FAILED\n");
		}

	}

	TRACE("\n");

	return 0;
}
static int init_others() {
	extern init_descriptor_t *__init_handlers_start, *__init_handlers_end;
	init_descriptor_t ** p_init_desc = &__init_handlers_start;
	int i, total = (int) (&__init_handlers_end - &__init_handlers_start);

	TRACE("\nInitializing others (total: %d)\n\n", total);

	for (i = 0; i < total; i++, p_init_desc++) {
		if (NULL == (*p_init_desc)) {
			LOG_ERROR("Missing init descriptor\n");
			continue;
		}
		if (NULL == ((*p_init_desc)->init)) {
			LOG_ERROR("Broken init handler descriptor.");
			continue;
		}

		if (-1 != (*p_init_desc)->init()) {
			TRACE("DONE\n");
		} else {
			TRACE("FAILED\n");
		}

	}

	TRACE("\n");

	return 0;
}
int hardware_init_hook() {
	//TODO during too long time for simulation:(
	copy_data_section();

	cache_data_enable();
	cache_instr_enable();

	irq_init_handlers();
	uart_init();
	timers_init();

	init_modules();

	init_others();

#ifdef MONITOR_TESTS
	if (0 != express_tests_execute()) {
		TRACE("express tests fault\n halt system\n");
		sys_halt();
		while (1)
			;
	}
#endif //MONITOR_TESTS

	return 0;
}
