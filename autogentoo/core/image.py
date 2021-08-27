from core.stage3 import Stage3


class Image:
    name: str
    stage3: Stage3

    def __init__(self,
                 name: str,
                 stage3: Stage3):
        self.name = name
        self.stage3 = stage3

    def build(self):
        """
        Build the Docker image with the following steps

        
        :return:
        """
