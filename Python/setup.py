#!python3
# -*- coding: utf-8 -*-
"""
Timed Sparse Matrices
Copyright (C) 2023 Anton Sobinov
https://github.com/nishbo/timed_sparse_matrix

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

"""

from setuptools import setup, find_packages

setup(
    name='tsm',
    packages=find_packages(),
    version='0.1.0',
    description='Loading and saving timed sparse tensors.',
    long_description=open('README.md').read(),
    long_description_content_type="text/markdown",
    url='https://github.com/nishbo/timed_sparse_matrix',
    install_requires=[
        'numpy'
    ],
    author='Anton R Sobinov',
    author_email='an.sobinov@gmail.com',
    license='GPL',
    zip_safe=False,
    include_package_data=True,
    data_files=[(
        '.', ['tsm/TSMDLL.dll']
    )],
)
