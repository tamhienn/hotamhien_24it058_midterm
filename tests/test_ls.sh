#!/bin/sh

# NetBSD ls compatibility regression tests
# Run from the project root: sh tests/test_ls.sh

set -u

cd "$(dirname "$0")/.." || exit 1

if [ ! -x ./myls ]; then
    echo "Executable ./myls not found."
    echo "Run make first."
    exit 1
fi

tmpdir=$(mktemp -d "${TMPDIR:-/tmp}/myls-test.XXXXXXXX") || exit 1
trap 'rm -rf "$tmpdir"' EXIT HUP INT TERM

mkdir -p "$tmpdir/special/subdir" || exit 1
touch "$tmpdir/special/normal.txt"
touch "$tmpdir/special/file with spaces.txt"
touch "$tmpdir/special/.hidden"
touch "$tmpdir/special/subdir/inside.txt"
ln -s normal.txt "$tmpdir/special/link.txt"

pass=0
fail=0

for dir in src "$tmpdir/special"; do
    for opt in -A -a -c -d -F -f -h -i -k -l -n -q -R -r -S -s -t -u -w -lh -ln -ls -lsh -ks -tr -tu -tc; do

        ./myls "$opt" "$dir" > "$tmpdir/my.out" 2> "$tmpdir/my.err"
        my_status=$?

        /bin/ls "$opt" "$dir" > "$tmpdir/native.out" 2> "$tmpdir/native.err"
        native_status=$?

        if cmp -s "$tmpdir/my.out" "$tmpdir/native.out" &&
           [ "$my_status" -eq "$native_status" ]; then
            pass=$((pass + 1))
        else
            echo "FAIL: $opt $dir"
            diff -u "$tmpdir/native.out" "$tmpdir/my.out" || :
            fail=$((fail + 1))
        fi
    done
done

echo "===================="
echo "PASS: $pass"
echo "FAIL: $fail"
echo "TOTAL: $((pass + fail))"

if [ "$fail" -ne 0 ]; then
    exit 1
fi

exit 0
