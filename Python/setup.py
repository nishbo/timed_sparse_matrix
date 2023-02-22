#!python3
# -*- coding: utf-8 -*-
"""
Timed Sparse Matrices
Copyright 2023 Anton Sobinov
https://github.com/nishbo/timed_sparse_matrix
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
    license='MIT',
    zip_safe=False
)
