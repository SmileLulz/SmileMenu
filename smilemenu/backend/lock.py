import os
import fcntl
import tempfile
from pathlib import Path


class SingleInstanceLock:
    def __init__(self, app_name="smilemenu"):
        self.app_name = app_name
        self.lock_dir = Path(tempfile.gettempdir()) / f"{app_name}-lock"
        self.lock_file = self.lock_dir / "lock.pid"
        self.lock_fd = None

    def _ensure_lock_dir(self):
        self.lock_dir.mkdir(parents=True, exist_ok=True)

    def try_lock(self):
        self._ensure_lock_dir()
        
        try:
            self.lock_fd = open(self.lock_file, "w")
            fcntl.flock(self.lock_fd.fileno(), fcntl.LOCK_EX | fcntl.LOCK_NB)
            self.lock_fd.write(str(os.getpid()))
            self.lock_fd.flush()
            return True
        except (IOError, OSError):
            if self.lock_fd:
                self.lock_fd.close()
                self.lock_fd = None
            return False

    def release(self):
        if self.lock_fd:
            try:
                fcntl.flock(self.lock_fd.fileno(), fcntl.LOCK_UN)
                self.lock_fd.close()
                self.lock_file.unlink(missing_ok=True)
            except Exception:
                pass
            self.lock_fd = None

    def is_running(self):
        try:
            with open(self.lock_file, "r") as f:
                pid = int(f.read().strip())
                os.kill(pid, 0)
                return True
        except (FileNotFoundError, ValueError, ProcessLookupError, OSError):
            return False

    def get_pid(self):
        try:
            with open(self.lock_file, "r") as f:
                return int(f.read().strip())
        except (FileNotFoundError, ValueError):
            return None