#!/usr/bin/env python3

import skbuild
skbuild.setup(
    name = 'my-package',
    description = 'Example package with scikit-build',
    url = 'https://github.com/equinor/scikit-build-example',
    packages = [
        'my_package',
    ],
    license = 'BSD',
    platforms = 'any',
    install_requires = [
    ],
    setup_requires = [
        'setuptools_scm',
    ],
    tests_require = [
    ],
    cmake_args = [
        # we can safely pass OSX_DEPLOYMENT_TARGET as it's ignored on
        # everything not OS X. We depend on C++11, which makes our minimum
        # supported OS X release 10.9
        '-DCMAKE_OSX_DEPLOYMENT_TARGET=10.9',
    ],
    use_scm_version = True,
)
