import ctypes as c
from collections.abc import Sized
from pyarts.workspace.api import arts_api as lib

from pyarts.classes.GriddedField2 import ArrayOfGriddedField2
from pyarts.classes.Vector import Vector, ArrayOfVector
from pyarts.classes.io import correct_save_arguments, correct_read_arguments
from pyarts.classes.ArrayBase import array_base


class XsecRecord:
    """ ARTS XsecRecord data

    Properties:
        version:
            Version (Index)

        spec:
            Species (Species)

        fitminpressures:
            Mininum pressures of fit

        fitmaxpressures:
            Maximum pressures of fit

        fitmintemperatures:
            Mininum temperatures of fit

        fitmaxtemperatures:
            Mininum temperatures of fit

        fitcoeffs:
            Fit coefficients
    """

    def __init__(self, data=None):
        if isinstance(data, c.c_void_p):
            self.__delete__ = False
            self.__data__ = data
        else:
            self.__delete__ = True
            self.__data__ = c.c_void_p(lib.createXsecRecord())
            if data is not None:
                raise RuntimeError("Only supports void initialization")

    @property
    def version(self):
        """ Version (Index) """
        return lib.getVersionXsecRecord(self.__data__)

    @version.setter
    def version(self, val):
        lib.setVersionXsecRecord(self.__data__, int(val))

    @property
    def spec(self):
        """ Species (Species) """
        return Species(c.c_void_p(lib.getSpeciesXsecRecord(self.__data__)))

    @spec.setter
    def spec(self, val):
        spec = val if isinstance(val, Species) else Species(val)
        lib.setSpeciesXsecRecord(self.__data__, spec.__data__)

    @property
    def fitminpressures(self):
        """ Minimum pressures of fit (Vector) """
        return Vector(c.c_void_p(lib.getFitMinPressuresXsecRecord(self.__data__)))

    @fitminpressures.setter
    def fitminpressures(self, val):
        self.fitminpressures.set(val)

    @property
    def fitmaxpressures(self):
        """ Maximum pressures of fit (Vector) """
        return Vector(c.c_void_p(lib.getFitMaxPressuresXsecRecord(self.__data__)))

    @fitmaxpressures.setter
    def fitmaxpressures(self, val):
        self.fitmaxpressures.set(val)

    @property
    def fitmintemperatures(self):
        """ Minimum temperatures of fit (Vector) """
        return Vector(c.c_void_p(lib.getFitMinTemperaturesXsecRecord(self.__data__)))

    @fitmintemperatures.setter
    def fitmintemperatures(self, val):
        self.fitmintemperatures.set(val)

    @property
    def fitmaxtemperatures(self):
        """ Maximum temperatures of fit (Vector) """
        return Vector(c.c_void_p(lib.getFitMaxTemperaturesXsecRecord(self.__data__)))

    @fitmaxtemperatures.setter
    def fitmaxtemperatures(self, val):
        self.fitmaxtemperatures.set(val)

    @property
    def fitcoeffs(self):
        """ Fit coefficients (ArrayOfGriddedField2) """
        return ArrayOfGriddedField2(c.c_void_p(lib.getFitCoeffsXsecRecord(self.__data__)))

    @fitcoeffs.setter
    def fitcoeffs(self, val):
        self.fitcoeffs.set(val)

    @staticmethod
    def name():
        return "XsecRecord"

    def print(self):
        """ Print to cout the ARTS representation of the class """
        lib.printXsecRecord(self.__data__)

    def __del__(self):
        if self.__delete__:
            lib.deleteXsecRecord(self.__data__)

    def __repr__(self):
        return "ARTS XsecRecord"

    def set(self, other):
        """ Sets this class according to another python instance of itself """
        if isinstance(other, XsecRecord):
            lib.setVersionXsecRecord(self.__data__, int(other.version))
            lib.setSpeciesXsecRecord(self.__data__, int(other.spec))
            self.fitminpressures = other.fitminpressures
            self.fitmaxpressures = other.fitmaxpressures
            self.fitmintemperatures = other.fitmintemperatures
            self.fitmaxtemperatures = other.fitmaxtemperatures
            self.fitcoeffs = other.fitcoeffs
        else:
            raise TypeError("Expects XsecRecord")

    def readxml(self, file):
        """ Reads the XML file

        Input:
            file:
                Filename to valid class-file (str)
        """
        if lib.xmlreadXsecRecord(self.__data__, correct_read_arguments(file)):
            raise OSError("Cannot read {}".format(file))

    def savexml(self, file, type="ascii", clobber=True):
        """ Saves the class to XML file

        Input:
            file:
                Filename to writable file (str)

            type:
                Filetype (str)

            clobber:
                Allow clobbering files? (any boolean)
        """
        if lib.xmlsaveXsecRecord(self.__data__, *correct_save_arguments(file, type, clobber)):
            raise OSError("Cannot save {}".format(file))

    def __eq__(self, other):
        if isinstance(other, XsecRecord) and \
                self.version == other.version and \
                self.spec == other.spec and \
                self.fitminpressures == other.fitminpressures and \
                self.fitmaxpressures == other.fitmaxpressures and \
                self.fitmintemperatures == other.fitmintemperatures and \
                self.fitmaxtemperatures == other.fitmaxtemperatures and \
                self.fitcoeffs == other.fitcoeffs:
            return True
        else:
            return False


exec(array_base(XsecRecord))


lib.createXsecRecord.restype = c.c_void_p
lib.createXsecRecord.argtypes = []

lib.deleteXsecRecord.restype = None
lib.deleteXsecRecord.argtypes = [c.c_void_p]

lib.printXsecRecord.restype = None
lib.printXsecRecord.argtypes = [c.c_void_p]

lib.xmlreadXsecRecord.restype = c.c_long
lib.xmlreadXsecRecord.argtypes = [c.c_void_p, c.c_char_p]

lib.xmlsaveXsecRecord.restype = c.c_long
lib.xmlsaveXsecRecord.argtypes = [c.c_void_p, c.c_char_p, c.c_long, c.c_long]

lib.getVersionXsecRecord.restype = c.c_long
lib.getVersionXsecRecord.argtypes = [c.c_void_p]

lib.getSpeciesXsecRecord.restype = c.c_long
lib.getSpeciesXsecRecord.argtypes = [c.c_void_p]

lib.setSpeciesXsecRecord.restype = None
lib.setSpeciesXsecRecord.argtypes = [c.c_void_p, c.c_void_p]

lib.getFitMinPressuresXsecRecord.restype = c.c_void_p
lib.getFitMinPressuresXsecRecord.argtypes = [c.c_void_p]

lib.getFitMaxPressuresXsecRecord.restype = c.c_void_p
lib.getFitMaxPressuresXsecRecord.argtypes = [c.c_void_p]

lib.getFitMinTemperaturesXsecRecord.restype = c.c_void_p
lib.getFitMinTemperaturesXsecRecord.argtypes = [c.c_void_p]

lib.getFitMaxTemperaturesXsecRecord.restype = c.c_void_p
lib.getFitMaxTemperaturesXsecRecord.argtypes = [c.c_void_p]

lib.getFitCoeffsXsecRecord.restype = c.c_void_p
lib.getFitCoeffsXsecRecord.argtypes = [c.c_void_p]
