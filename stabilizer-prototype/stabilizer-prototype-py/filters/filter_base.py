from abc import *
from typing import *
from collections import namedtuple

from gyro_data import GyroData

import numpy as np


class __FalseObject:
    def __bool__(self):
        return False


class AxisAccessor:

    def __init__(self, data: Iterable, caller_arg):
        """Usage

        ::

            AxisAccessor(data, ['caller_arg'])._some_getter_(x) --> data._some_getter_(x)['caller_arg'] 
            AxisAccessor(data, 'caller_arg')._some_getter_(x) --> data._some_getter_(x).caller_arg
            AxisAccessor(data, ['caller_arg'])._some_setter_(x, y) --> data._some_getter_(x)['caller_arg'] = y


        Args:
            param1: test
        """

        self.__data = data
        assert (
            isinstance(caller_arg, list) or isinstance(caller_arg, str)
        ), "caller_arg must either be a list (used as subscript) or a str (used as attribute name)"

        self.__caller_arg = caller_arg

        if isinstance(caller_arg, list):
            assert len(caller_arg) >= 1

            if len(caller_arg) == 1:
                self.__caller_arg = caller_arg[0]
            else:
                self.__caller_arg = tuple(caller_arg)

            self.__object_getter = self.__subscript_getter
            self.__object_setter = self.__subscript_setter
        elif isinstance(caller_arg, str):
            self.__object_getter = self.__attr_getter
            self.__object_setter = self.__attr_setter
    
            

    def __subscript_getter(self, obj):
        return obj[self.__caller_arg]

    def __subscript_setter(self, obj, value):
        obj[self.__caller_arg] = value

    def __attr_getter(self, obj):
        return obj.__getattribute__(self.__caller_arg)

    def __attr_setter(self, obj, value):
        return obj.__setattr__(self.__caller_arg, value)

    def __getattr__(self, name):
        if name.startswith('_'):
            return super().__getattribute__(name)
        return self.__object_getter(self.__data.__getattribute__(name))

    def __setattr__(self, name: str, value):
        if name.startswith('_'):
            return super().__setattr__(name, value)
        return self.__object_setter(self.__data.__getattribute__(name), value)

    def __getitem__(self, name):
        return self.__object_getter(self.__data.__getitem__(name))

    def __setitem__(self, name, value):
        return self.__object_setter(self.__data.__getitem__(name), value)

    def __iter__(self):
        for value in self.__data:
            yield self.__object_getter(value)


class GyroDataFilterBase(metaclass=ABCMeta):
    """
    Works offline only
    """

    def __init__(self):
        pass


    @abstractmethod
    def apply_filter(self, gyro_data: GyroData) -> GyroData:
        pass
