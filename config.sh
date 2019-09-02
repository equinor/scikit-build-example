function run_tests {
    set -x
    python -c "import my_package; print('div:', my_package.div(4, 2))"
}
