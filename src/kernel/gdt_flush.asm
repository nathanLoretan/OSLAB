.global gdt_flush

.set CODE_SEG_OFFSET, 0x08
.set DATA_SEG_OFFSET, 0x10

.section .text
gdt_flush:

	# Get pointer given in parameter and load it in LGDT
	movl 4(%esp), %eax
	lgdt (%eax)

	# Enable protected mode
	movl %cr0, %eax
	orl  $0x00000001, %eax
	movl %eax, %cr0

    # Load Data Segment
	movw	$DATA_SEG_OFFSET, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw 	%ax, %fs
	movw	%ax, %gs
	movw 	%ax, %ss

	# Need to do a far jump to specify the new code segment. To add a
	# segment-offset pointer, the jump need to be specify like:
	# jmp $segment, $offset
    jmp	$CODE_SEG_OFFSET, $complete_flush

complete_flush:
	ret
