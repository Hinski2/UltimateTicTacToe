import subprocess 
from typing import List, Tuple

class Compiler:
    def __init__(self, bot_0_path: str, bot_1_path: str, flags: List[str] | None = None):
        self.bot_0_path = bot_0_path
        self.bot_1_path = bot_1_path
        self.flags = flags or []
        
    
    def build(self) -> Tuple[str, str]:
        self._compile_bots(self.bot_0_path, "bot_0")
        self._compile_bots(self.bot_1_path, "bot_1")
        
        return ["bots/bot_0", "bots/bot_1"]
        
    def _compile_bots(self, path, exec_name):
        compile_command = ["g++", path, "-o", f"bots/{exec_name}"] + self.flags
        subprocess.run(compile_command, check=True)