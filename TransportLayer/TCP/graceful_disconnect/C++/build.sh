#!/usr/bin/env bash
set -euo pipefail

RED='\033[0;31m'; GREEN='\033[0;32m'; YELLOW='\033[1;33m'
CYAN='\033[0;36m'; BOLD='\033[1m'; NC='\033[0m'

info() { echo -e "${CYAN}[INFO]${NC}  $*"; }
ok() { echo -e "${GREEN}[OK]${NC}    $*"; }
warn() { echo -e "${YELLOW}[WARN]${NC}  $*"; }
fail() { echo -e "${RED}[FAIL]${NC}  $*"; exit 1; }
section() { echo -e "\n${BOLD}════════════════════════════════════════${NC}"; \
            echo -e "${BOLD}  $*${NC}"; \
            echo -e "${BOLD}════════════════════════════════════════${NC}"; }

CXX="${CXX:-g++}"
STD="-std=c++17"
BOOST_LIBS="-lboost_system -lpthread"
BUILD_DIR="build"
SERVER_SRC="server.cpp"
CLIENT_SRC="client.cpp"
SERVER_BIN="${BUILD_DIR}/server"
CLIENT_BIN="${BUILD_DIR}/client"
SERVER_ASAN_BIN="${BUILD_DIR}/server_asan"
CLIENT_ASAN_BIN="${BUILD_DIR}/client_asan"

WARN_FLAGS=(
    -Wall
    -Wextra
    -Wpedantic
    -Wshadow
    -Wformat=2
    -Wcast-align
    -Wcast-qual
    -Wconversion
    -Wsign-conversion
    -Wnull-dereference
    -Wdouble-promotion
    -Wundef
    -Wuninitialized
    -Wmissing-include-dirs
    -Wswitch-enum
    -Wstrict-overflow=5
    -Wfloat-equal
    -Wredundant-decls
    -Winline
)

HARDENING_FLAGS=(
    -D_FORTIFY_SOURCE=2
    -fstack-protector-strong
    -fstack-clash-protection
    -fcf-protection
    -Wl,-z,relro
    -Wl,-z,now
    -Wl,-z,noexecstack
    -pie
    -fPIE
)

ASAN_FLAGS=(
    -fsanitize=address
    -fsanitize=undefined
    -fsanitize=leak
    -fno-omit-frame-pointer
    -fno-optimize-sibling-calls
)

mkdir -p "${BUILD_DIR}"

section "Checking dependencies"
for tool in "${CXX}" valgrind cppcheck; do
    if command -v "${tool}" &>/dev/null; then
        ok "${tool} found: $(command -v ${tool})"
    else
        warn "${tool} not found - соответствующие шаги будут пропущены"
    fi
done

section "Step 1: Release build (-O2)"
BASE_FLAGS="${STD} -O2 ${WARN_FLAGS[*]} ${HARDENING_FLAGS[*]}"

info "Compiling server..."
${CXX} ${BASE_FLAGS} "${SERVER_SRC}" -o "${SERVER_BIN}" ${BOOST_LIBS} \
    && ok "server -> ${SERVER_BIN}" \
    || fail "server compilation failed"

info "Compiling client..."
${CXX} ${BASE_FLAGS} "${CLIENT_SRC}" -o "${CLIENT_BIN}" ${BOOST_LIBS} \
    && ok "client -> ${CLIENT_BIN}" \
    || fail "client compilation failed"

section "Step 2: AddressSanitizer + UBSan build"
if ${CXX} --version &>/dev/null; then
    ASAN_BUILD_FLAGS="${STD} -g3 -O1 ${WARN_FLAGS[*]} ${ASAN_FLAGS[*]}"

    info "Compiling server (asan)..."
    ${CXX} ${ASAN_BUILD_FLAGS} "${SERVER_SRC}" -o "${SERVER_ASAN_BIN}" ${BOOST_LIBS} \
        && ok "server_asan -> ${SERVER_ASAN_BIN}" \
        || warn "ASan server build failed (возможно линковщик не поддерживает)"

    info "Compiling client (asan)..."
    ${CXX} ${ASAN_BUILD_FLAGS} "${CLIENT_SRC}" -o "${CLIENT_ASAN_BIN}" ${BOOST_LIBS} \
        && ok "client_asan -> ${CLIENT_ASAN_BIN}" \
        || warn "ASan client build failed"
fi

section "Step 3: cppcheck static analysis"
if command -v cppcheck &>/dev/null; then
    info "Running cppcheck on server.cpp..."
    cppcheck \
        --enable=all \
        --std=c++17 \
        --suppress=missingIncludeSystem \
        --suppress=unusedFunction \
        --error-exitcode=1 \
        "${SERVER_SRC}" && ok "cppcheck server: clean"

    info "Running cppcheck on client.cpp..."
    cppcheck \
        --enable=all \
        --std=c++17 \
        --suppress=missingIncludeSystem \
        --suppress=unusedFunction \
        --error-exitcode=1 \
        "${CLIENT_SRC}" && ok "cppcheck client: clean"
else
    warn "cppcheck not found, skipping"
fi

section "Step 4: Valgrind memcheck"
if command -v valgrind &>/dev/null; then
    info "Starting server under valgrind (background)..."

    valgrind \
        --tool=memcheck \
        --leak-check=full \
        --show-leak-kinds=all \
        --track-origins=yes \
        --error-exitcode=42 \
        --log-file="${BUILD_DIR}/valgrind_server.log" \
        "${SERVER_BIN}" &
    SERVER_PID=$!
    sleep 1  

    info "Running client under valgrind..."
    valgrind \
        --tool=memcheck \
        --leak-check=full \
        --show-leak-kinds=all \
        --track-origins=yes \
        --error-exitcode=42 \
        --log-file="${BUILD_DIR}/valgrind_client.log" \
        "${CLIENT_BIN}"
    CLIENT_EXIT=$?

    sleep 1
    kill "${SERVER_PID}" 2>/dev/null || true
    wait "${SERVER_PID}" 2>/dev/null || true
    echo ""
    info "Valgrind server log:"
    cat "${BUILD_DIR}/valgrind_server.log"
    echo ""
    info "Valgrind client log:"
    cat "${BUILD_DIR}/valgrind_client.log"

    if [ "${CLIENT_EXIT}" -eq 0 ]; then
        ok "Valgrind client: no errors"
    else
        warn "Valgrind client exited with code ${CLIENT_EXIT}"
    fi

    if grep -q "ERROR SUMMARY: 0 errors" "${BUILD_DIR}/valgrind_client.log"; then
        ok "Valgrind client: 0 memory errors"
    else
        warn "Valgrind client: найдены ошибки - смотри ${BUILD_DIR}/valgrind_client.log"
    fi

    if grep -q "ERROR SUMMARY: 0 errors" "${BUILD_DIR}/valgrind_server.log"; then
        ok "Valgrind server: 0 memory errors"
    else
        warn "Valgrind server: найдены ошибки - смотри ${BUILD_DIR}/valgrind_server.log"
    fi
else
    warn "valgrind not found, skipping"
fi

section "Build complete"
ok "Binaries:"
echo "  Release:  ${SERVER_BIN}, ${CLIENT_BIN}"
if [ -f "${SERVER_ASAN_BIN}" ]; then
    echo "  ASan:     ${SERVER_ASAN_BIN}, ${CLIENT_ASAN_BIN}"
fi
echo ""
info "To run manually:"
echo "  ./${SERVER_BIN} &"
echo "  ./${CLIENT_BIN}"
echo ""
info "To run with ASan (ловит memory bugs в рантайме):"
echo "  ./${SERVER_ASAN_BIN} &"
echo "  ./${CLIENT_ASAN_BIN}"
