#!/usr/bin/env bash
set -euo pipefail

RED='\033[0;31m'; GREEN='\033[0;32m'; YELLOW='\033[1;33m'
CYAN='\033[0;36m'; BOLD='\033[1m'; NC='\033[0m'

info() { echo -e "${CYAN}[INFO]${NC}  $*"; }
ok() { echo -e "${GREEN}[OK]${NC}    $*"; }
warn() { echo -e "${YELLOW}[WARN]${NC}  $*"; }
fail() { echo -e "${RED}[FAIL]${NC}  $*"; exit 1; }
section() {
    echo -e "\n${BOLD}════════════════════════════════════════${NC}"
    echo -e "${BOLD}  $*${NC}"
    echo -e "${BOLD}════════════════════════════════════════${NC}"
}

[[ $# -eq 2 ]] || fail "Usage: $0 <file1> <file2>  (e.g. ./build.sh server.c client.c)"
FILE1="$1"
FILE2="$2"
[[ -f "$FILE1" ]] || fail "File not found: $FILE1"
[[ -f "$FILE2" ]] || fail "File not found: $FILE2"

BUILD_DIR="build"
mkdir -p "${BUILD_DIR}"

detect_lang() {
    case "${1##*.}" in
        c) echo "c"    ;;
        cpp|cxx|cc|C) echo "cpp"  ;;
        go) echo "go"   ;;
        rs) echo "rust" ;;
        zig) echo "zig"  ;;
        sh|bash) echo "bash" ;;
        *) echo "unknown" ;;
    esac
}

LANG1=$(detect_lang "$FILE1")
LANG2=$(detect_lang "$FILE2")

[[ "$LANG1" != "unknown" ]] || fail "Unsupported file extension: $FILE1"
[[ "$LANG2" != "unknown" ]] || fail "Unsupported file extension: $FILE2"
[[ "$LANG1" == "$LANG2" ]]  || fail "Files are in different languages: $FILE1 ($LANG1) vs $FILE2 ($LANG2)"
LANG="$LANG1"

info "Detected language: ${BOLD}${LANG}${NC}"

C_WARN_FLAGS=(
    -Wall -Wextra -Wpedantic -Wshadow -Wformat=2
    -Wcast-align -Wcast-qual -Wconversion -Wsign-conversion
    -Wnull-dereference -Wdouble-promotion -Wundef -Wuninitialized
    -Wmissing-include-dirs -Wswitch-enum -Wstrict-overflow=5
    -Wfloat-equal -Wredundant-decls
)

HARDENING_FLAGS=(
    -D_FORTIFY_SOURCE=2 -fstack-protector-strong -fstack-clash-protection
    -fcf-protection -Wl,-z,relro -Wl,-z,now -Wl,-z,noexecstack -pie -fPIE
)

ASAN_FLAGS=(
    -fsanitize=address -fsanitize=undefined -fsanitize=leak
    -fno-omit-frame-pointer -fno-optimize-sibling-calls
)

check_tool() { command -v "$1" &>/dev/null; }

bin_name() {
    local src="$1"
    echo "${BUILD_DIR}/$(basename "${src%.*}")"
}

BIN1=$(bin_name "$FILE1")
BIN2=$(bin_name "$FILE2")

section "Checking dependencies"

case "$LANG" in
    c) TOOLS=("gcc" "cppcheck" "valgrind") ;;
    cpp) TOOLS=("${CXX:-g++}" "cppcheck" "valgrind") ;;
    go) TOOLS=("go") ;;
    rust) TOOLS=("rustc") ;;
    zig) TOOLS=("zig") ;;
    bash) TOOLS=("bash" "shellcheck") ;;
esac

for tool in "${TOOLS[@]}"; do
    if check_tool "$tool"; then
        ok "${tool}: $(command -v "${tool}")"
    else
        warn "${tool} not found - relevant steps will be skipped"
    fi
done

static_check() {
    local src="$1"
    case "$LANG" in
        c)
            if check_tool cppcheck; then
                info "cppcheck: $src"
                cppcheck --enable=all --std=c11 \
                    --suppress=missingIncludeSystem \
                    --suppress=unusedFunction \
                    --error-exitcode=1 \
                    "$src" && ok "cppcheck $src: clean" \
                           || warn "cppcheck found issues in $src"
            fi
            ;;
        cpp)
            if check_tool cppcheck; then
                info "cppcheck: $src"
                cppcheck --enable=all --std=c++17 \
                    --suppress=missingIncludeSystem \
                    --suppress=unusedFunction \
                    --error-exitcode=1 \
                    "$src" && ok "cppcheck $src: clean" \
                           || warn "cppcheck found issues in $src"
            fi
            ;;
        go)
            info "go vet: $src"
            go vet "$src" && ok "go vet $src: clean" \
                          || warn "go vet found issues in $src"
            ;;
        rust)
            info "rustc --emit=metadata (check): $src"
            rustc --edition 2021 --emit=metadata -o /dev/null "$src" \
                && ok "rustc check $src: clean" \
                || warn "rustc found issues in $src"
            ;;
        zig)
            info "zig ast-check: $src"
            zig ast-check "$src" && ok "zig ast-check $src: clean" \
                                 || warn "zig found issues in $src"
            ;;
        bash)
            info "bash -n (syntax): $src"
            bash -n "$src" && ok "bash -n $src: clean" \
                           || warn "bash syntax error in $src"
            if check_tool shellcheck; then
                info "shellcheck: $src"
                shellcheck "$src" && ok "shellcheck $src: clean" \
                                  || warn "shellcheck found issues in $src"
            fi
            ;;
    esac
}

build_release() {
    local src="$1" out="$2"
    info "Building: $src -> $out"
    case "$LANG" in
        c)
            gcc -O2 "${C_WARN_FLAGS[@]}" "${HARDENING_FLAGS[@]}" \
                "$src" -o "$out" \
                && ok "$src -> $out" || fail "Compilation failed: $src"
            ;;
        cpp)
            "${CXX:-g++}" -std=c++17 -O2 "${C_WARN_FLAGS[@]}" "${HARDENING_FLAGS[@]}" \
                -lboost_system -lpthread \
                "$src" -o "$out" \
                && ok "$src -> $out" || fail "Compilation failed: $src"
            ;;
        go)
            go build -o "$out" "$src" \
                && ok "$src -> $out" || fail "Compilation failed: $src"
            ;;
        rust)
            rustc --edition 2021 -O "$src" -o "$out" \
                && ok "$src -> $out" || fail "Compilation failed: $src"
            ;;
        zig)
            local base
            base=$(basename "${src%.*}")
            zig build-exe "$src" --name "$base" \
                && mv "$base" "$out" \
                && ok "$src -> $out" || fail "Compilation failed: $src"
            ;;
        bash)
            ok "$src: ready (bash script, no build needed)"
            ;;
    esac
}

build_asan() {
    local src="$1" out="${2}_asan"
    local compiler std_flag
    case "$LANG" in
        c)   compiler="gcc";            std_flag="-std=c11"   ;;
        cpp) compiler="${CXX:-g++}";    std_flag="-std=c++17" ;;
        *)   return ;;
    esac
    info "Building ASan: $src -> $out"
    "${compiler}" "${std_flag}" -g3 -O1 \
        "${C_WARN_FLAGS[@]}" "${ASAN_FLAGS[@]}" \
        "$src" -o "$out" \
        && ok "$src -> $out" || warn "ASan build failed: $src"
}

run_valgrind() {
    local bin="$1" label="$2" log="${BUILD_DIR}/valgrind_${label}.log"
    info "Valgrind: $bin"
    timeout 10s valgrind \
        --tool=memcheck \
        --leak-check=full \
        --show-leak-kinds=all \
        --track-origins=yes \
        --error-exitcode=42 \
        --log-file="$log" \
        "$bin" || true
    echo ""
    info "Valgrind log ($label):"
    cat "$log"
    if grep -q "ERROR SUMMARY: 0 errors" "$log"; then
        ok "Valgrind $label: 0 memory errors"
    else
        warn "Valgrind $label: errors found — see $log"
    fi
}

section "Step 1: Static analysis"
static_check "$FILE1"
static_check "$FILE2"

section "Step 2: Release build (-O2)"
build_release "$FILE1" "$BIN1"
build_release "$FILE2" "$BIN2"

if [[ "$LANG" == "c" || "$LANG" == "cpp" ]]; then
    section "Step 3: AddressSanitizer + UBSan build"
    build_asan "$FILE1" "$BIN1"
    build_asan "$FILE2" "$BIN2"

    section "Step 4: Valgrind memcheck"
    if check_tool valgrind; then
        run_valgrind "$BIN1" "$(basename "${BIN1}")"
        run_valgrind "$BIN2" "$(basename "${BIN2}")"
    else
        warn "valgrind not found, skipping"
    fi
fi

section "Build complete"
ok "Binaries:"
if [[ "$LANG" != "bash" ]]; then
    echo "  Release:  ${BIN1}, ${BIN2}"
    if [[ -f "${BIN1}_asan" ]]; then
        echo "  ASan:     ${BIN1}_asan, ${BIN2}_asan"
    fi
fi
echo ""
info "To run:"
if [[ "$LANG" == "bash" ]]; then
    echo "  bash ${FILE1}"
    echo "  bash ${FILE2}"
else
    echo "  ./${BIN1}"
    echo "  ./${BIN2}"
fi
