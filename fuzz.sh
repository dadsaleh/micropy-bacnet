#!/bin/bash
cd "$(dirname "$0")" || exit 1

FUZZ_TIME="${FUZZ_TIME:-60}"
OUT=build/fuzz

shopt -s nullglob
files=(test/fuzz/fuzz_*.c)
if [ ${#files[@]} -eq 0 ]; then
    echo "No fuzz_*.c files in test/fuzz" >&2
    exit 1
fi

options=("All" "${files[@]}")
selected=0

menu() {
    local count=${#options[@]} key rest i
    tput civis
    trap 'tput cnorm' EXIT
    while true; do
        for i in "${!options[@]}"; do
            if [ "$i" -eq "$selected" ]; then
                printf '\e[7m> %s\e[0m\n' "${options[$i]}"
            else
                printf '  %s\n' "${options[$i]}"
            fi
        done
        IFS= read -rsn1 key || exit 1
        if [ "$key" = $'\e' ]; then
            read -rsn2 -t 0.1 rest
            key+=$rest
        fi
        case "$key" in
            $'\e[A') selected=$(((selected + count - 1) % count)) ;;
            $'\e[B') selected=$(((selected + 1) % count)) ;;
            '') return ;;
            q) exit 0 ;;
        esac
        printf '\e[%dA' "$count"
    done
}

echo "Select a fuzz target (arrows, enter, q to quit):"
menu

if [ "$selected" -eq 0 ]; then
    targets=("${files[@]}")
else
    targets=("${options[$selected]}")
fi

mkdir -p "$OUT"
for src in "${targets[@]}"; do
    name=$(basename "$src" .c)
    echo "Building $name"
    clang -g -O1 -std=c23 -fsanitize=fuzzer,address,undefined \
        -Ipdu_decoder -Ipdu_decoder/core \
        "$src" $(find pdu_decoder -name '*.c') -o "$OUT/$name" || exit 1
done

for src in "${targets[@]}"; do
    name=$(basename "$src" .c)
    mkdir -p "$OUT/corpus/$name" "$OUT/crashes/$name"
    kitty --title "$name" bash -c "./$OUT/$name -max_total_time=$FUZZ_TIME -artifact_prefix=$OUT/crashes/$name/ $OUT/corpus/$name; read -rp 'Press enter to close'" >/dev/null 2>&1 &
    disown
done
