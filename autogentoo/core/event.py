from typing import Dict, Callable, Set, Tuple


class EventListener:
    """
    Object that can bind to an EventHost
    Handle specific event broadcasts from
    the host to run hooks.
    """

    _signal_table: Dict[str, Callable]

    def __init__(self):
        self._signal_table = {}

    def bind_event(self, signal_name: str, handler: Callable):
        self._signal_table[signal_name] = handler

    def handle_event(self, host: "EventHost", signal_name: str, args: Tuple, kwargs: Dict):
        # Only run if the signal is bound
        if signal_name in self._signal_table:
            self._signal_table[signal_name](host=host, *args, **kwargs)


class EventHost:
    """
    Able to broadcast events to its listeners
    """

    listeners: Set[EventListener]

    def __init__(self):
        self.listeners = set()

    def bind(self, listener: EventListener):
        self.listeners.add(listener)

    def broadcast(self, signal_name: str, *args, **kwargs):
        for i in self.listeners:
            i.handle_event(self, signal_name, args, kwargs)
