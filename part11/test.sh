nasm listing_0037_single_register_mov.asm
nasm listing_0038_many_register_mov.asm

sha256sum listing_0037_single_register_mov > binary.sha256
sha256sum listing_0038_many_register_mov >> binary.sha256
sha256sum -c binary.sha256