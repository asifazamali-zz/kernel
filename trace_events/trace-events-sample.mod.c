#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xe37a5a9c, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0xea651a6e, __VMLINUX_SYMBOL_STR(trace_event_raw_init) },
	{ 0x1ef08eff, __VMLINUX_SYMBOL_STR(ftrace_event_reg) },
	{ 0x9160556f, __VMLINUX_SYMBOL_STR(kthread_stop) },
	{ 0x3396d637, __VMLINUX_SYMBOL_STR(wake_up_process) },
	{ 0x8210fdf9, __VMLINUX_SYMBOL_STR(kthread_create_on_node) },
	{ 0xb3f7646e, __VMLINUX_SYMBOL_STR(kthread_should_stop) },
	{        0, __VMLINUX_SYMBOL_STR(schedule_timeout) },
	{ 0xe13ae1fb, __VMLINUX_SYMBOL_STR(current_task) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
	{ 0x91ccb71e, __VMLINUX_SYMBOL_STR(trace_define_field) },
	{ 0x1e3a88fb, __VMLINUX_SYMBOL_STR(trace_seq_printf) },
	{ 0x9f124ec3, __VMLINUX_SYMBOL_STR(ftrace_raw_output_prep) },
	{ 0x498d293a, __VMLINUX_SYMBOL_STR(trace_buffer_unlock_commit) },
	{ 0xd75921d5, __VMLINUX_SYMBOL_STR(filter_check_discard) },
	{ 0x9621849f, __VMLINUX_SYMBOL_STR(ring_buffer_event_data) },
	{ 0x30aa2bb5, __VMLINUX_SYMBOL_STR(trace_event_buffer_lock_reserve) },
	{ 0x4629334c, __VMLINUX_SYMBOL_STR(__preempt_count) },
	{ 0x78764f4e, __VMLINUX_SYMBOL_STR(pv_irq_ops) },
	{ 0xf33cdfab, __VMLINUX_SYMBOL_STR(perf_tp_event) },
	{ 0x5792f848, __VMLINUX_SYMBOL_STR(strlcpy) },
	{ 0xe6602e9f, __VMLINUX_SYMBOL_STR(perf_trace_buf_prepare) },
	{ 0x7628f3c7, __VMLINUX_SYMBOL_STR(this_cpu_off) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "11B35C61EA12F445679E6DC");
