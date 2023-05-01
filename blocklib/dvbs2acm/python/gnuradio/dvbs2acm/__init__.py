import os
try:
    from .dvbs2acm_python import *
except ImportError:
    dirname, filename = os.path.split(os.path.abspath(__file__))
    __path__.append(os.path.join(dirname, "bindings"))
    from .dvbs2acm_python import *
