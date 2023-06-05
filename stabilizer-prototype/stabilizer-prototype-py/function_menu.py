from typing import *

__all__ = [
    'FuncRef', 'show_menu', 'show_menu_run_once'
]

class FuncRef:
    def __init__(self, func, *args, **kwargs):
        self.func = func
        self.args = args
        self.kwargs = kwargs

    def __call__(self, *args, **kwargs):
        self.func(*self.args, *args, **self.kwargs, **kwargs)

    def __repr__(self):
        largs_repr = map(str, self.args)
        kwargs_repr = [f'{k}={v}' for k, v in self.kwargs.items()]
        args_repr = ', '.join([*largs_repr, *kwargs_repr])
        return f'{self.func}({args_repr})'


def show_menu(options: list[FuncRef]):
    while True:
        show_menu_run_once(options)

def show_menu_run_once(options: list[FuncRef]):
    while True:
        print("Select an option or q to quit (default=0):")
        for i, v in enumerate(options):
            print(f'    {i}: {v}')
        print('\n > ', end='')

        option_id = input()
        if option_id == 'q':
            break
        elif not option_id.strip():
            option_id = '0'

        try:
            option = options[int(option_id)]
        except RuntimeError as e:
            print(str(e))
            continue

        option()
        break