.section .multiboot, "a"
.align 8

/* Multiboot2 header:
   magic: 0xE85250D6
   architecture: 0 (i386)
   header_length: 24 (bytes total header including the end tag)
   checksum: such that the sum of the four values equals 0 modulo 2^32.
*/
multiboot_header:
    .long 0xE85250D6         /* Multiboot2 magic number */
    .long 0                  /* Architecture (0 = i386) */
    .long 24                 /* Total header length (in bytes) */
    .long 0x17ADAF12         /* Checksum: 0xE85250D6 + 0 + 24 + 0x17ADAF12 = 0 mod 2^32 */
    /* End tag for the multiboot header */
    .long 0                  /* Tag type 0 indicates the end */
    .long 8                  /* Tag size is 8 bytes */

.text
.global _start
_start:
    call kernel_main
hang:
    jmp hang
