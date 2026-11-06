#!/usr/bin/env bash
# Usage: ./verify.sh [directory]    (defaults to the current directory)

dir="${1:-.}"
cd "$dir" || { echo "Cannot enter $dir"; exit 1; }

# ANSI colours (disabled automatically if output isn't a terminal)
if [ -t 1 ]; then
  RED=$'\033[1;31m'
  GREEN=$'\033[0;32m'
  RESET=$'\033[0m'
else
  RED=""; GREEN=""; RESET=""
fi

shopt -s nullglob
sumfiles=(*.sha256)
if [ ${#sumfiles[@]} -eq 0 ]; then
  echo "${RED}No .sha256 files found in $PWD${RESET}"
  exit 1
fi

ok=0; failed=0; missing=0

for sf in "${sumfiles[@]}"; do
  echo "== $sf"
  while IFS= read -r line; do
    case "$line" in
      *": OK")
        echo "${GREEN}${line}${RESET}"
        ok=$((ok + 1))
        ;;
      *"FAILED open or read")
        echo "${RED}${line}  [MISSING]${RESET}"
        missing=$((missing + 1))
        ;;
      *": FAILED")
        echo "${RED}${line}  [CORRUPTED]${RESET}"
        failed=$((failed + 1))
        ;;
      sha256sum:*)
        # warnings such as "WARNING: 1 computed checksum did NOT match"
        echo "${RED}${line}${RESET}"
        ;;
      *)
        echo "$line"
        ;;
    esac
  done < <(sha256sum -c "$sf" 2>&1)
done

echo
echo "Summary: ${GREEN}$ok OK${RESET}, ${RED}$failed corrupted${RESET}, ${RED}$missing missing${RESET}  :::: TOTAL: $ok + $failed + $missing >> === $((ok + failed + missing)) === "

[ $((failed + missing)) -eq 0 ] && exit 0 || exit 1
