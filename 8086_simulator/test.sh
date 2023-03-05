set -e

clear

nasm listing_0037_single_register_mov.asm
nasm listing_0038_many_register_mov.asm
nasm listing_0039_more_movs.asm
nasm listing_0040_challenge_movs.asm

sha256sum listing_0037_single_register_mov > binary.sha256
sha256sum listing_0038_many_register_mov >> binary.sha256
sha256sum listing_0039_more_movs >> binary.sha256
sha256sum listing_0040_challenge_movs >> binary.sha256
sha256sum -c binary.sha256

cd src
./build.sh
./sim8086 ../listing_0037_single_register_mov  > listing_0037_single_register_mov.asm
./sim8086 ../listing_0038_many_register_mov  >  listing_0038_many_register_mov.asm
./sim8086 ../listing_0039_more_movs  >  listing_0039_more_movs.asm
./sim8086 ../listing_0040_challenge_movs  >  listing_0040_challenge_movs.asm

nasm listing_0037_single_register_mov.asm
nasm listing_0038_many_register_mov.asm
nasm listing_0039_more_movs.asm
nasm listing_0040_challenge_movs.asm

cp ../binary.sha256 .
sha256sum -c --ignore-missing binary.sha256

rm listing_*
rm *.sha256