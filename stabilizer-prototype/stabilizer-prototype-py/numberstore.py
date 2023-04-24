from typing import *


class NumberStore:
    class ConfigItem:
        """
        keybinding: 'aAbBcCdD', where aAbB is for field 1, cCdD is for field 2, etc. Lowercase is the base kfey, uppercase is the modifier.
        To disable a certain key, use a space.

        """

        def __init__(self,
                     name: str,
                     keybinding: str,
                     *,
                     size=None,
                     step=3,
                     mod_scale=5,
                     bounds: Union[tuple[int, int], list[tuple[int, int]]] = None,
                     display_func=lambda x: x):
            self.name = name
            self.size = size or len(keybinding) // 4
            self.keybinding = keybinding
            self.step = step
            self.mod_scale = mod_scale
            self.display_func = display_func
            self.bounds = bounds

            self.reset()

        def reset(self):
            self.data = [0] * self.size
        
        @property
        def bounds(self):
            return self.__bounds
        
        @bounds.setter
        def bounds(self, bounds):
            if bounds is None:
                self.__bounds = None
            elif isinstance(bounds, tuple):
                self.__bounds = [bounds] * self.size
            else:
                self.__bounds = bounds.copy()

        def _conform_to_bounds(self):
            if self.bounds is None:
                return
            for i in range(self.size):
                self.data[i] = min(
                    max(self.bounds[i][0], self.data[i]), self.bounds[i][1])

        def __repr__(self):
            return f'ConfigItem({self.name}): {self.data}'

    def __init__(self, *, data_fields, ignored_keys=list(' ')):
        self.data_fields = data_fields
        self.ignored_keys = ignored_keys

        self.__validate_fields()

    def __validate_fields(self):
        keybindings = {}
        for field in self.data_fields:
            for keybinding in field.keybinding:
                if (keybinding not in self.ignored_keys) and keybinding in keybindings:
                    raise Exception(
                        f'keybinding {keybinding} is already bound to {keybindings[keybinding]}')
                keybindings[keybinding] = field.name


    def update(self, dict_value):
        self.config.update(dict_value)

    def setkw(self, **kwargs):
        self.config.update(kwargs)

    def set(self, keybinding, value):
        self.config[keybinding] = value

    def get_field(self, keybinding) -> Optional[ConfigItem]:
        for field in self.data_fields:
            if keybinding == field.name:
                return field
        return None

    def get_field_by_name(self, name) -> Optional[ConfigItem]:
        for field in self.data_fields:
            if name == field.name:
                return field
        return None

    def __getitem__(self, keybinding):
        field = self.get_field(keybinding)
        if field is None:
            raise KeyError(keybinding)
        return field.data

    def __setitem__(self, keybinding, value):
        field = self.get_field(keybinding)
        if field is None:
            raise KeyError(keybinding)
        field.data = value

    def get_field_from_keystroke(self, keystroke) -> tuple[Optional[ConfigItem], int]:
        if isinstance(keystroke, int):
            keystroke = chr(keystroke & 0xFF)
        assert len(str(keystroke)) == 1
        if keystroke in self.ignored_keys:
            return None, -1
        for field in self.data_fields:
            idx = field.keybinding.find(keystroke)
            if idx >= 0:
                return field, idx
        return None, -1

    def handle_key(self, keystroke):
        f, idx = self.get_field_from_keystroke(keystroke)
        if f is None:
            return None
        alt_idx = idx // 4
        direction = idx % 4 // 2
        mod = idx % 2

        f.data[alt_idx] += f.step * (1 if direction == 1 else -1) * \
            (1 if mod == 0 else f.mod_scale)
        f._conform_to_bounds()
        return f

    def reset(self, field=None):
        if field is None:
            for field in self.data_fields:
                field.reset()
        else:
            self.get_field(field).reset()


ConfigItem = NumberStore.ConfigItem
